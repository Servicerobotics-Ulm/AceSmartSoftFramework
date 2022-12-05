// --------------------------------------------------------------------------
//
//  Copyright (C) 2014 Alex Lotz
//
//        lotz@hs-ulm.de
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//
//  This file is part of ACE/SmartSoft.
//
//  ACE/SmartSoft is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  ACE/SmartSoft is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with ACE/SmartSoft.  If not, see <http://www.gnu.org/licenses/>.
//
// --------------------------------------------------------------------------

#include "RemoteNSTask.h"

#include <ace/Name_Request_Reply.h>

#ifdef WITH_SYSTEM_D
	#include <systemd/sd-daemon.h> 
#endif

#include <sstream>

RemoteNSTask::RemoteNSTask()
:	SmartACE::Task(0)
,	first_call_of_handle_signal(true)
,	initialized(false)
,	synchronized(false)
,	acceptor(NULL)
,	remote_ns_mutex()
,	parameter()
,	local_rebind_mutex()
,	requsts_mutex()
,	request_sema(0)
{
	ACE_Reactor::instance()->register_handler(SIGTERM, this);
	ACE_Reactor::instance()->register_handler(SIGINT, this);
}

RemoteNSTask::~RemoteNSTask() {  }

void RemoteNSTask::setAcceptor(NameAcceptor *acc) {
	acceptor = acc;
}

int RemoteNSTask::init(const int &argc, char** argv)
{
	// load parameters
	try
	{
		// if paramfile is given as argument
		if(parameter.tryAddFileFromArgs(argc,argv,"filename", 'f'))
		{
			std::cout << "parameter file is loaded from an argv argument \n";
		}
		// else load standard paramfile
		else
		{
			std::ifstream ifs;
			if(parameter.searchFile("ns_config.ini", ifs) == true) {
				std::cout << "load ns_config.ini parameter file\n";
				parameter.addFile(ifs);
			}
		}

		// add command line arguments to allow overwriting of parameters
		// from file
		parameter.addCommandLineArgs(argc,argv,"NamingService");

		parameter.print();
	} catch (const SmartACE::IniParameterError & e) {
		std::cerr << e.what() << std::endl;
	} catch (const std::exception &ex) {
		std::cerr << "Uncaught std::exception: " << ex.what() << std::endl;
	} catch (...) {
		std::cerr << "Uncaught exception" << std::endl;
	}

	// finally start the task execution
	return this->start();
}

int RemoteNSTask::fini() {
	ACE_Guard<SmartACE::SmartMutex> g(remote_ns_mutex);
	std::map<std::string, SmartACE::NSProxy*>::const_iterator it;
	for(it=this->remote_ns_list.begin(); it!=this->remote_ns_list.end(); ++it) {
		it->second->trigger_shutdown();
	}
	return 0;
}

int RemoteNSTask::createEndpoint(const std::string &addr)
{
	ACE_Guard<SmartACE::SmartMutex> nsg(remote_ns_mutex);
	ACE_INET_Addr ns_address;

	if(ns_address.string_to_addr(addr.c_str()) != 0) {
		// could not parse the ip-string (ip-string is invalid)
		std::cerr << "Warning: RemoteNS IP address " << addr << " (given in the ini file) seems to be missformed -> ignoring" << std::endl;
		return -1;
	}

	//check and filter out the own address
	size_t own_addr_nbr;
	ACE_INET_Addr* own_addrs;
	if(ACE::get_ip_interfaces(own_addr_nbr, own_addrs) == 0) {
		bool is_own_address = false;
		std::string remote_host_addr = ns_address.get_host_addr();
		for(size_t i=0; i<own_addr_nbr; ++i) {
			std::string own_host_addr = own_addrs[i].get_host_addr();
			if(remote_host_addr == own_host_addr) {
				is_own_address = true;
				break;
			}
		}
		delete[] own_addrs;
		if(is_own_address) {
			std::cout << "Info: skip connecting to address " << addr << " as this is the own address" << std::endl;
			return -1;
		}
	}


	// setup the connection options to break up after one second trying
	ACE_Time_Value timeout(1);
	ACE_Synch_Options options(ACE_Synch_Options::USE_TIMEOUT, timeout);

	// now create the client side end-point
	SmartACE::NSProxy* remote_ns = 0;
	ACE_NEW_RETURN(remote_ns, SmartACE::NSProxy, -1);

	std::cout << "Info: Trying to connect to remote naming-service with TCP/IP: " << addr << std::endl;

	// finally do the actual connection
	if(connector.connect(remote_ns, ns_address, options) != 0) {
		// could not connect the remote naming service (maybe it is not yet started, or not available at all)
		// -> default strategy is just to skip staring it and continue with the next TCP/IP in the list (clean up the created proxy)
         if(errno == ECONNREFUSED || errno == ETIME || errno == EWOULDBLOCK) {
            // remote acceptor is not available or ping times are too large
        	std::cerr << "Warning: connection to remote naming-service with TCP/IP: " << addr << " timed out after one second -> this end-point is ignored!" << std::endl;
         } else {
        	 std::cerr << "Error: connection to remote naming-service with TCP/IP: " << addr << " failed due to unknown reason -> this end-point is ignored!" << std::endl;
         }
		remote_ns->trigger_shutdown();
		return -1;
	} else {
		if(this->remote_ns_list.count(addr) > 0) {
			// a remote ns connection to the same ip already exists
			// -> close the connection before assigning the new ns_proxy
			this->remote_ns_list[addr]->trigger_shutdown();
			std::cerr << "Warning: remote NS with TCP/IP: " << addr << " already was connected - reconnecting using this address." << std::endl;
		} else {
			std::cout << "Info: Successfully connected to remote naming-service with TCP/IP: " << addr << std::endl;
		}

		// save the connection pointer in the list
		this->remote_ns_list[addr] = remote_ns;

		ACE_NS_WString wname(ACE_TEXT("SET_ENDPOINT_FLAG"));
		ACE_NS_WString wvalue;
		if( remote_ns->bind(wname, wvalue, this->getRemoteNsType().c_str()) != 0) {
			// something went wrong
			std::cerr << "Warning: SET_ENDPOINT_FLAG failed for TCP/IP: " << addr << " - continuing" << std::endl;
		}
	}
	return 0;
}

void RemoteNSTask::postponeLocalRebind(NameHandler *ptr, const ACE_CString &name, const ACE_CString &value, const ACE_CString &type)
{
	ACE_Guard<SmartACE::SmartMutex> g(local_rebind_mutex);

	LocalRebind entry;
	entry.ptr = ptr;
	entry.name = name;
	entry.value = value;
	entry.type = type;

	this->postponed_rebinds.push_back(entry);
}

void RemoteNSTask::finish_postponed_rebinds()
{
	LocalRebind entry;
	entry.ptr = NULL;

	do {
		ACE_Guard<SmartACE::SmartMutex> g(local_rebind_mutex);
			if(!postponed_rebinds.empty()) {
				entry = postponed_rebinds.front();
				postponed_rebinds.pop_front();
			}
		g.release();

		if(entry.ptr != NULL) {
			ACE_InputCDR message_cdr((size_t) 0);
			entry.ptr->handle_local_rebind(entry.name, entry.value, entry.type);
		}
	} while(entry.ptr != NULL);
}

void RemoteNSTask::propagateRemoteCommand(const int &cmdId, const std::string &name, const std::string &value, const std::string &type)
{
	ACE_Guard<SmartACE::SmartMutex> g(requsts_mutex);
	Request req;
//	req.caller = NULL;
	req.command = cmdId;
	req.name = name;
	req.value = value;
	req.type = type;

	this->requests.push_back(req);

	this->request_sema.release();
}

int RemoteNSTask::task_execution()
{
	// first get the list with IP addresses for remote naming services from the ini-file and try to connect to each of them
	if(parameter.checkIfParameterExists("RemoteNS", "ip")) {
		std::list<std::string> ips = parameter.getStringList("RemoteNS", "ip");
		std::list<std::string>::const_iterator it;
		for(it=ips.begin(); it!=ips.end(); ++it)
		{
			if(this->createEndpoint(*it) == 0) {
				ACE_Guard<SmartACE::SmartMutex> nsg(remote_ns_mutex);

				ACE_NS_WString wname(ACE_TEXT("CREATE_ENDPOINT"));
				ACE_INET_Addr local_addr;
				acceptor->acceptor().get_local_addr(local_addr);
				std::stringstream port_ss;
				port_ss << local_addr.get_port_number();
				ACE_NS_WString wvalue(port_ss.str().c_str());
				// special bind request (internally caught due to this->getRemoteNsType() signature)
				if( remote_ns_list[*it]->bind(wname, wvalue, this->getRemoteNsType().c_str()) != 0) {
					// something went wrong
					std::cerr << "Warning: Could not communicate with remote naming service with TCP/IP: " << *it << " - skipping" << std::endl;
					continue;
				}

				if(!synchronized) {
					// local NameSpace is not yet synchronized, try to do so with the currently established remote connection
					ACE_NS_WString wname2(ACE_TEXT("CAN_YOU_SYNC?"));
					// first ask the remote ns whether it is ready to sync, or is maybe itself not jet fully initialized
					// in the latter case a return value != 0 is replied
					if( remote_ns_list[*it]->bind(wname2, wvalue, this->getRemoteNsType().c_str()) == 0) {
						nsg.release();

						// synchronization method internally checks if already synchronized and skips synchronizing if so.
						if( synchronize_with_remote_ns(*it) == 0) {
							// successfully synchronized (can be skipped in all the next iterations)
							this->synchronized = true;
						}
					}
				}
			}
		}
	} else {
		std::cerr << "Info: No optional ip entries found in ini file" << std::endl;
	}

	finish_postponed_rebinds();

	// from here on the NamingService is fully initialized
	this->initialized = true;

	std::cout << "Info: NamingService fully initialized!" << std::endl;
	
	//when NS-Daemon is compiled with SYSTEMD, signal the finished initialization	
	#ifdef WITH_SYSTEM_D
		sd_notify(0, "READY=1");
	#endif

	// now execute requests from the local end-points
	while(true) {
		this->request_sema.acquire();

		Request req;

		ACE_Guard<SmartACE::SmartMutex> rg(requsts_mutex);
			req = this->requests.front();
			this->requests.pop_front();
		rg.release();

		// a regular bind was triggered
		ACE_NS_WString wname(req.name.c_str());
		int command = req.command;
		ACE_NS_WString wvalue(req.value.c_str());

		if(command == ACE_Name_Request::BIND || command == ACE_Name_Request::REBIND) {
			insertLocalEntry(req.name);
		} else if(command == ACE_Name_Request::UNBIND) {
			removeLocalEntry(req.name);
		}


		ACE_Guard<SmartACE::SmartMutex> nsg(remote_ns_mutex);
			std::map<std::string, SmartACE::NSProxy*>::const_iterator it;
			for(it=this->remote_ns_list.begin(); it!=this->remote_ns_list.end(); ++it) {
				if(command == ACE_Name_Request::BIND || command == ACE_Name_Request::REBIND) {
					//std::cout << "Info: propagate rebind(" << req.name <<", " <<req.value<<") to IP: " << it->first << std::endl;
					if( it->second->rebind(wname, wvalue, req.type.c_str()) != 0 ) {
						std::cerr << "Error: could not propagate the rebind: " << req.name << std::endl;
					}
				} else if(command == ACE_Name_Request::UNBIND) {
					//std::cout << "Info: propagate unbind(" << req.name << ") to IP: " << it->first << std::endl;
					if( it->second->unbind(wname) != 0 ) {
						std::cerr << "Error: could not propagate the unbind: " << req.name << std::endl;
					}
				} else {
					std::cout << "Info: command Nr. " << command << " is not implemented for Remote NS synchronization" << std::endl;
				}
			}
		nsg.release();
	}
	return 0;
}

int RemoteNSTask::synchronize_with_remote_ns(const std::string &address)
{
	ACE_Guard<SmartACE::SmartMutex> nsg(remote_ns_mutex);

	if(acceptor == NULL) {
		std::cerr << "Error: acceptor not specified in RemoteNSTask (needed to access the local Name_Space)" << std::endl;
		return -1;
	}

	//std::cout << "Info: Synchronizing with remote naming service with TCP/IP " << address << std::endl;

	ACE_BINDING_SET all_entries;
	if(remote_ns_list[address]->list_type_entries(all_entries, "smart") != 0) {
		return -1;
	} else {
		ACE_BINDING_ITERATOR iterator(all_entries);
		for(ACE_Name_Binding *entry=0; !iterator.done(); iterator.advance())
		{
			iterator.next(entry);

			ACE_TString tname = ACE_TEXT_WCHAR_TO_TCHAR(entry->name_.fast_rep());
			ACE_TString tvalue = ACE_TEXT_WCHAR_TO_TCHAR(entry->value_.fast_rep());
			ACE_TString ttype = entry->type_;

			ACE_NS_WString out_wvalue = entry->value_;

			std::string cvalue(tvalue.c_str());
			if(cvalue.find("localhost") != std::string::npos || cvalue.find("127.0.0.1") != std::string::npos)
			{
				// The value contains an IP which is localhost and thus useless for remote communication.
				// For this case the assumption can be made that the corresponding component can be accessed
				// using the same IP as the remote naming service has (because this component is registered to this
				// naming service and thus must be on the same node. This assumption might be not true for cases
				// where the naming service synchronizes over several hops. For example, NS-C is started first, then
				// NS-B is started and it syncs with NS-C, then NS-A is started which syncs with NS-B. In this case,
				// the addresses in NS-A coming from NS-C might not be accessible on the NS-A machine, which however
				// is considered to be a network configuration problem and thus not treated here.
				size_t delimeter_pos = cvalue.find(':');
				if(delimeter_pos != std::string::npos) {
					// the structure of cvalue is assumed to be as follows:
					// <ip-address>:<port-nbr>#<hash-string>
					// first the leading ip-address is removed
					std::string rest = cvalue.substr(delimeter_pos);

					size_t delimeter_pos2 = address.find(':');
					if(delimeter_pos2 != std::string::npos) {
						std::string ns_ip_address = address.substr(0, delimeter_pos2);
						// the corrected value now contains the changed ip-address (from the remote ns) + the original value
						// (without the leading localhost ip-address)
						cvalue =  ns_ip_address + rest;
						//std::cout << "remote value: " << cvalue << " | " << "corrected value: " << corrected_value << std::endl;
						out_wvalue = ACE_TEXT_ALWAYS_WCHAR(cvalue.c_str());
					}
				}
			}

		    ACE_NS_WString in_wvalue;
		    char *type = 0;
			if(acceptor->naming_context()->resolve(entry->name_, in_wvalue, type) == 0) {
				// found a the key -> overwrite its value
				ACE_TString tloc_value = ACE_TEXT_WCHAR_TO_TCHAR(in_wvalue.fast_rep());
				std::cerr << "Warning: found local entry " << tname.c_str() << "; local value " << tloc_value.c_str() << " will be overwritten by " << cvalue << std::endl;
				acceptor->naming_context()->rebind(entry->name_, out_wvalue, entry->type_);
			} else {
				// regular bind of a new value
				std::cout << "Info: bind remote entry: " << tname << ", " << cvalue << std::endl;
				acceptor->naming_context()->bind(entry->name_, out_wvalue, entry->type_);
			}
			if(type != 0) delete[] type;
		}
	}

	return 0;
}


void RemoteNSTask::insertLocalEntry(const std::string &name)
{
	ACE_Guard<SmartACE::SmartMutex> nsg(remote_ns_mutex);
	local_entries.insert(name);
}
void RemoteNSTask::removeLocalEntry(const std::string &name)
{
	ACE_Guard<SmartACE::SmartMutex> nsg(remote_ns_mutex);
	local_entries.erase(name);
}
void RemoteNSTask::cleanUpAllLocalEntries()
{
	ACE_Guard<SmartACE::SmartMutex> nsg(remote_ns_mutex);

	std::set<std::string>::iterator it;
	for(it=local_entries.begin(); it!=local_entries.end(); it++) {
		ACE_NS_WString wname(it->c_str());

		// unbind local entry on all remote NSes
		std::map<std::string, SmartACE::NSProxy*>::const_iterator it;
		for(it=this->remote_ns_list.begin(); it!=this->remote_ns_list.end(); ++it) {
			it->second->unbind(wname);
		}

		// unbind local entry from local database
		acceptor->naming_context()->unbind(wname);
	}
}

int RemoteNSTask::handle_signal (int signum, siginfo_t *, ucontext_t *)
{

  if (signum == SIGINT || signum == SIGTERM)
  {
	  // 1) Make shure the signal is handeled only once
	  if(!first_call_of_handle_signal) return 0;

	  ACE_DEBUG((LM_DEBUG, ACE_TEXT("RemoteNSTask::handle_signal(SIGINT || SIGTERM)\n")));


	  first_call_of_handle_signal = false;

	  cleanUpAllLocalEntries();

	  ACE_Reactor::instance()->end_reactor_event_loop();
  }

  return 0;
}
