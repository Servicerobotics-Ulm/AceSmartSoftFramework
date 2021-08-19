// --------------------------------------------------------------------------
//
//  Copyright (C) 2012 Alex Lotz
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

#include "smartNSAdapterACE.hh"

#include "smartNSProxy.hh"

#include <ace/Service_Config.h>
#include <ace/Dynamic_Service.h>
#include <ace/Auto_Ptr.h>
#include <ace/Log_Msg.h>
#include <ace/OS_NS_sys_stat.h>

#if ACE_MAJOR_VERSION < 6
#include <ace/OS.h>
#else
#include <ace/OS_main.h>
#endif

#include <ace/Get_Opt.h>
#include <ace/OS_Memory.h>

namespace {
    int convertStringToNSKey(const ACE_TString &name, SmartACE::NSKeyType &key)
    {
        ACE_TRACE ("convertStringToNSKey");
        size_t openBracket=0, closeBracket=0;
        ssize_t start=0, length=0;

        // parse with the syntax: "<CompName><namePattern><...>"

        // get component-name
        openBracket = name.find('<');
        closeBracket = name.find('>');
        // check if both brackets were successfully found
        if(openBracket == ACE_TString::npos || closeBracket == ACE_TString::npos) return -1;
        start = openBracket + 1;
        length = closeBracket - start;
        if(length <= 0) return -1;
        key.names[SmartACE::NSKeyType::COMP_NAME] = name.substr(start, length);
        ACE_TString rest = name.substr(length+2);

        // get pattern-name
        openBracket = rest.find('<');
        closeBracket = rest.find('>');
        // check if both brackets were successfully found
        if(openBracket == ACE_TString::npos || closeBracket == ACE_TString::npos) return -1;
        start = openBracket + 1;
        length = closeBracket - start;
        if(length <= 0) return -1;
        key.names[SmartACE::NSKeyType::PATTERN_NAME] = rest.substr(start, length);
        rest = rest.substr(length+2);

        // get service-name
        openBracket = rest.find('<');
        closeBracket = rest.find('>');
        // check if both brackets were successfully found
        if(openBracket == ACE_TString::npos || closeBracket == ACE_TString::npos) return -1;
        start = openBracket + 1;
        length = closeBracket - start;
        if(length <= 0) return -1;
        key.names[SmartACE::NSKeyType::SERVICE_NAME] = rest.substr(start, length);
        rest = rest.substr(length+2);

        // get commObj1-name
        openBracket = rest.find('<');
        closeBracket = rest.find('>');
        // check if both brackets were successfully found
        if(openBracket == ACE_TString::npos || closeBracket == ACE_TString::npos) return -1;
        start = openBracket + 1;
        length = closeBracket - start;
        if(length <= 0) return -1;
        key.names[SmartACE::NSKeyType::COMMOBJ1_NAME] = rest.substr(start, length);
        rest = rest.substr(length+2);

        // get commObj2-name
        openBracket = rest.find('<');
        closeBracket = rest.find('>');
        // check if both brackets were successfully found
        if(openBracket == ACE_TString::npos || closeBracket == ACE_TString::npos) {
            key.names[SmartACE::NSKeyType::COMMOBJ2_NAME] = "*";
        } else {
            start = openBracket + 1;
            length = closeBracket - start;
            if(length <= 0) return -1;
            key.names[SmartACE::NSKeyType::COMMOBJ2_NAME] = rest.substr(start, length);
        }

        return 0;
    }
}



SmartACE::NSKeyType::operator ACE_TString() const
{
    ACE_TRACE ("SmartACE::NSKeyType::operator ACE_TString");
    ACE_TString key;

    // assemble naming-service key containing:
    // <component-name><pattern-type><service-name><communication-object>
    key = ACE_TEXT("<");
    key += names[SmartACE::NSKeyType::COMP_NAME].c_str();
    key += ACE_TEXT("><");
    key += names[SmartACE::NSKeyType::PATTERN_NAME].c_str();
    key += ACE_TEXT("><");
    key += names[SmartACE::NSKeyType::SERVICE_NAME].c_str();
    key += ACE_TEXT(">");

    if (names[SmartACE::NSKeyType::COMMOBJ1_NAME] != "*") {
        key += ACE_TEXT("<");
        key += names[SmartACE::NSKeyType::COMMOBJ1_NAME].c_str();
        key += ACE_TEXT(">");
    }

    // some patterns consist of a second communication object
    if (names[SmartACE::NSKeyType::COMMOBJ2_NAME] != "*") {
        key += ACE_TEXT("<");
        key += names[SmartACE::NSKeyType::COMMOBJ2_NAME].c_str();
        key += ACE_TEXT(">");
    }

    // return complete key
    return key;
}



SmartACE::NSAdapterACE::NSAdapterACE()
:	ns_address()
,	component_ip()
,	use_component_ip(false)
,	connector()
,	namingService(0)
,	contextInitialized(false)
{
    ACE_TRACE ("SmartACE::NSAdapterACE::NSAdapterACE");
}

SmartACE::NSAdapterACE::NSAdapterACE(const int &argc, char** argv)
:	ns_address()
,	component_ip()
,	use_component_ip(false)
,	connector()
,	namingService(0)
,	contextInitialized(false)
{
    ACE_TRACE ("SmartACE::NSAdapterACE::NSAdapterACE");
    this->init(argc, argv);
}

SmartACE::NSAdapterACE::~NSAdapterACE()
{
    ACE_TRACE ("SmartACE::NSAdapterACE::~NSAdapterACE");
    this->fini();
}


int SmartACE::NSAdapterACE::init(const int &argc, char** argv)
{
    this->checkForHelpArg(argc, argv); 
    return this->init(parseCommandLineParams(argc,argv));
}

int SmartACE::NSAdapterACE::init(const std::map<std::string,std::string> &parameters)
{
    ACE_TRACE ("SmartACE::NSAdapterACE::init");
    ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, guard, mutex, -1);
    int retval = -1;

    if( contextInitialized ) {
        // already initialized
        retval = 0;
    } else {
    	if(this->setConfiguration(parameters) == 0) {
			ACE_NEW_RETURN(namingService, SmartACE::NSProxy, -1);
			if(connector.connect(namingService, ns_address) == 0) {
				if(!use_component_ip) {
					namingService->peer().get_local_addr(component_ip);
				}
				contextInitialized = true;
				retval = 0;
			} else {
				ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "connect()"),1);
				retval = -1;
			}
    	} else {
    		retval = -1;
    	}

    	/*
        // get a new instance of the naming context
        namingService = ACE_Dynamic_Service<SmartACE::NSProxy>::instance(serviceName.c_str());

        // check if the naming context was initialized successfully
        if (ACE_Service_Repository::instance()->find(serviceName.c_str()) == 0) {
            //ACE_DEBUG ((LM_INFO, ACE_TEXT("%s: ready and running...\n"), service_name.c_str() ));
            retval = 0;
            contextInitialized = true;
        }
        */
    }

    return retval;
}

void SmartACE::NSAdapterACE::fini(void)
{
    ACE_TRACE ("SmartACE::NSAdapterACE::fini");
    ACE_GUARD(ACE_Recursive_Thread_Mutex, guard, mutex);

    if( contextInitialized )
    {
        // close the instance of the naming-context
    	namingService->trigger_shutdown();

        contextInitialized = false;
    }
}

void SmartACE::NSAdapterACE::checkForHelpArg(const int &argc, char * const * argv, std::ostream &oss)
{
	bool help_arg_found = false;
	for(int i=0; i<argc; ++i) {
		std::string arg = argv[i];
		if(arg == "-h" || arg == "--help" || arg == "-help") {
			help_arg_found = true;
			break;
		}
	}

	if(help_arg_found) {
	    oss << "\nThis is the help for SmartSoft's command line parameters." << std::endl;
	    oss << "In the following, the accepted parameters are listed:" << std::endl;
	    oss << "-h or --help" << std::endl;
	    oss << "\tPrints out this help." << std::endl;
	    oss << "-i <IP> or --ns-ip <IP> or --ns-ip=<IP>" << std::endl;
	    oss << "\tIs used to define the TCP/IP address of the naming" << std::endl;
	    oss << "\tservice. Replace <IP> by the actual TCP/IP." << std::endl;
	    oss << "\te.g. --ns-ip 127.0.0.1 (a DNS name is also allowed)" << std::endl;
	    oss << "-p <nbr> or --ns-port <nbr> or --ns-port=<nbr>" << std::endl;
	    oss << "\tIs used to define the port number of the naming service." << std::endl;
	    oss << "\tReplace <nbr> by the decimal value for the port number." << std::endl;
	    oss << "\te.g. --ns-port 12345" << std::endl;
	    oss << "-a <IP>:<nbr> or --ns-addr <IP>:<nbr> or --ns-addr=<IP>:<nbr>" << std::endl;
	    oss << "\tIs used to define both, the TCP/IP address and Port number" << std::endl;
	    oss << "\tof the naming service, separated by a ':'. Replace <IP>" << std::endl;
	    oss << "\tby the actual TCP/IP and <nbr> by the corresp. port number." << std::endl;
	    oss << "\tIf this param is given, --ns-ip and --ns-port are ignored." << std::endl;
	    oss << "\te.g. --ns-addr=127.0.0.1:12345 (environment variables" << std::endl;
	    oss << "\tare also allowed, e.g. --ns-addr=$SMART_NS_ADDR)" << std::endl;
	    oss << "-c <IP> or --component-ip <IP> or --component-ip=<IP>" << std::endl;
	    oss << "\tIs used to define the TCP/IP address for this component." << std::endl;
	    oss << "\tThis IP must be accessible from everywhere in the network," << std::endl;
	    oss << "\twhere other SmartSoft components are executed. The port" << std::endl;
	    oss << "\tnumber is automatically defined by SmartSoft." << std::endl;
	    oss << "\te.g. --component-ip 127.0.0.1 (a DNS name is also allowed)" << std::endl;
	    oss << "-f <file> or --filename <file> or --filename=<file>" << std::endl;
	    oss << "\tIs used to define the ini file, which can be given just by" << std::endl;
	    oss << "\tthe name or in addition with the relative/full path." << std::endl;
	    oss << "\te.g. -f $SMART_ROOT/etc/MyComponent.ini" << std::endl;
	    oss << std::endl;
	    oss << "\tIf some or all the parameters are missing, default" << std::endl;
	    oss << "\tvalues are chosen. Thereby, the following strategy for" << std::endl;
	    oss << "\teach of the parameters is used. If a console parameter is" << std::endl;
	    oss << "\tabsent, it is then searched in the ini file. In the ini file" << std::endl;
	    oss << "\tlong options (without preceding hyphens) are expected for" << std::endl;
	    oss << "\tparameter names inside of the category [component]." << std::endl;
	    oss << "\tThe ini file could look similar as follows:" << std::endl;
	    oss << "[smartsoft]" << std::endl;
	    oss << "[component]" << std::endl;
	    oss << "ns-ip 127.0.0.1" << std::endl;
	    oss << "ns-port 12345" << std::endl;
	    oss << "# or" << std::endl;
	    oss << "ns-addr 127.0.0.1:12345" << std::endl;
	    oss << std::endl;
	    oss << "component-ip 192.168.0.1" << std::endl;
	    oss << "# etc." << std::endl;
	    oss << std::endl;
	    oss << "\tIf a parameter is also missing in the ini file, a" << std::endl;
	    oss << "\tcorrsponding environment variable (below) is searched:" << std::endl;
	    oss << "$SMART_NS_ADDR=<IP:PORT> for --ns-addr" << std::endl;
	    oss << "\tThis variable is used for both values, IP and port number," << std::endl;
	    oss << "\twhich must be separated by a ':' (e.g. 127.0.0.1:12345)." << std::endl;
	    oss << "$SMART_IP=<IP> for --component-ip" << std::endl;
	    oss << "\tThe port number for component IP is not required, it" << std::endl;
	    oss << "\tis automatically chosen by SmartSoft." << std::endl;
	    oss << std::endl;
	    oss << "\tIn case none of the previously mentioned variants were used," << std::endl;
	    oss << "\tframework's default values are taken. These are:" << std::endl;
	    oss << "\t--ns-ip: " << ACE_LOCALHOST << std::endl;
	    oss << "\t--ns-port: " << ACE_DEFAULT_SERVER_PORT << std::endl;
	    oss << "\t--component-ip: " << ACE_LOCALHOST << std::endl;
	    oss << "\tIf no --filename is specified, the component executable" << std::endl;
	    oss << "\tcontains compiled default values, which are then used." << std::endl;
	    oss << std::endl;
	    oss << "In general the parameters can also be assigned directly:" << std::endl;
	    oss << "--<param-group-name>.<param-name>=<value>" << std::endl;
	    oss << "e.g. --settings.speed=5" << std::endl;
	    oss << "<<END OF PARAMETER HELP>>" << std::endl;
	    oss << std::endl;
	}
}


int SmartACE::NSAdapterACE::bind(const SmartACE::NSKeyType &name, const NSValueType &value)
{
    ACE_TRACE ("SmartACE::NSAdapterACE::bind");
    ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, guard, mutex, -1);

    if( !contextInitialized ) return -1;

    unsigned countValues=0;
    for(unsigned i=0; i < SmartACE::NSKeyType::NUMBER_ELEMENTS; ++i) {
        if(name.names[i] == "*") break;

        countValues++;
    }

    // at least compName, patternName, serviceName and the name of the first communication-object must
    // be specified (not as asterisk)
    if(countValues < SmartACE::NSKeyType::COMMOBJ1_NAME) return -1;

    // get middleware specific key representation
    ACE_TString name_str = name;

    //<alexej 2013-11-21>
    ACE_TString type = NSValueType::getSmartType();
    NSValueType fixed_value = value;

    // in case the component IP was given, we exchange the ip and restore to port number
    if(use_component_ip) {
    	type = "smartsoft";
    	fixed_value.setInetAddr(component_ip);
    	fixed_value.setPortNumber(value.getPortNumber());
    }
    //</alexej>

    if (namingService->bind(name_str.c_str(), fixed_value.toString().c_str(), type.c_str()) != 0)
    {
        ACE_DEBUG((LM_INFO,
                ACE_TEXT(
                        "CommPattern (%s): WARNING: service %s in %s already exists (will NOT be rebound) !\n"),
                        name.names[SmartACE::NSKeyType::PATTERN_NAME].c_str(),
                        name.names[SmartACE::NSKeyType::SERVICE_NAME].c_str(),
                        name.names[SmartACE::NSKeyType::COMP_NAME].c_str()));
        return -1;
    }

    return 0;
}


int SmartACE::NSAdapterACE::rebind(const SmartACE::NSKeyType &name, const NSValueType &value)
{
    ACE_TRACE ("SmartACE::NSAdapterACE::rebind");
    ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, guard, mutex, -1);

    if( !contextInitialized ) return -1;

    unsigned countValues=0;
    for(unsigned i=0; i < SmartACE::NSKeyType::NUMBER_ELEMENTS; ++i) {
        if(name.names[i] == "*") break;

        countValues++;
    }

    // at least compName, patternName, serviceName and the name of the first communication-object must
    // be specified (not as asterisk)
    if(countValues < SmartACE::NSKeyType::COMMOBJ1_NAME) return -1;

    // get middleware specific key representation
    ACE_TString name_str = name;

    //<alexej 2013-11-21>
    ACE_TString type = NSValueType::getSmartType();
    NSValueType fixed_value = value;

    // in case the component IP was given, we exchange the ip and restore to port number
    if(use_component_ip) {
    	type = "smartsoft";
    	fixed_value.setInetAddr(component_ip);
    	fixed_value.setPortNumber(value.getPortNumber());
    }
    //</alexej>

    if (namingService->bind(name_str.c_str(), fixed_value.toString().c_str(), type.c_str()) != 0)
    {
    	ACE_DEBUG((LM_INFO,
                ACE_TEXT(
                        "CommPattern (%s): WARNING: service %s in %s rebound !\n"),
                        name.names[SmartACE::NSKeyType::PATTERN_NAME].c_str(),
                        name.names[SmartACE::NSKeyType::SERVICE_NAME].c_str(),
                        name.names[SmartACE::NSKeyType::COMP_NAME].c_str()));
        if (namingService->rebind(name_str.c_str(), fixed_value.toString().c_str(), type.c_str()) != 0)
            ACE_ERROR_RETURN(
                    (LM_ERROR, "%p rebind failed!\n", "Naming::rebind"),
                    -1);
    }

    return 0;
}


int SmartACE::NSAdapterACE::resolve(const SmartACE::NSKeyType &name, NSValueType &value)
{
    ACE_TRACE ("SmartACE::NSAdapterACE::resolve");
    ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, guard, mutex, -1);

    if( !contextInitialized ) return -1;

    // get middleware specific key representation
    ACE_TString name_str = name;

    ACE_NS_WString n(name_str.c_str());
    ACE_NS_WString val;
    char *type = 0;

    // resolve name
    int retval = namingService->resolve(n, val, type);

    delete[] type;

    if (retval != 0) {
        //ACE_ERROR_RETURN ((LM_ERROR,
        //                  "%p Resolve failed!\n",
        //                  "SmartACE::NSAdapterACE_OLD::resolve",
        //                 -1);
        return -1;
    }

    // convert whide string into correct string representation
    return value.setFromString( ACE_TEXT_WCHAR_TO_TCHAR(val.fast_rep()) );
}


int SmartACE::NSAdapterACE::unbind(const SmartACE::NSKeyType &name)
{
    ACE_TRACE ("SmartACE::NSAdapterACE::unbind");
    ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, guard, mutex, -1);

    if( !contextInitialized ) return -1;

    // get middleware specific key representation
    ACE_TString name_str = name;
    return namingService->unbind(name_str.c_str());
}


int SmartACE::NSAdapterACE::unbindEntriesForMatchingPattern(const SmartACE::NSKeyType &searchPattern)
{
    ACE_TRACE ("SmartACE::NSAdapterACE::unbindEntriesForMatchingPattern");
    ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, guard, mutex, -1);

    if( !contextInitialized ) return -1;

    ACE_Unbounded_Queue<SmartACE::NSKeyType> matching_entries = getEntriesForMatchingPattern(searchPattern);
    int retVal = 0;

    for(ACE_Unbounded_Queue_Iterator<SmartACE::NSKeyType> iter(matching_entries);
        !iter.done(); iter.advance() )
    {
          SmartACE::NSKeyType *entry = 0;
          iter.next (entry);

          if( unbind( *(entry) ) != 0 )
          {
              retVal = -1;
          }
    }

    return retVal;
}


ACE_Unbounded_Queue<SmartACE::NSKeyType> SmartACE::NSAdapterACE::getEntriesForMatchingPattern(
        const SmartACE::NSKeyType &searchPattern) const
{
    ACE_TRACE ("SmartACE::NSAdapterACE::getEntriesForMatchingPattern");
    ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, guard, mutex,
            ACE_Unbounded_Queue<SmartACE::NSKeyType>());

    if( !contextInitialized ) return ACE_Unbounded_Queue<SmartACE::NSKeyType>();

    ACE_Unbounded_Queue<SmartACE::NSKeyType> matching_entries;
    ACE_BINDING_SET all_entries;

    // 1) get all elements from naming-context that match the type "smart" (which fitts for both valid types "smartip" and "smartsoft")
    if(namingService->list_type_entries(all_entries, "smart") != 0) {
        return matching_entries;
    } else {
        // 2) iterate through the list of entries
        ACE_BINDING_ITERATOR iterator(all_entries);
        SmartACE::NSKeyType curr_name;

        for(ACE_Name_Binding *entry=0; !iterator.done(); iterator.advance())
        {
            iterator.next(entry);

            // we are interested in the name part (value and type are ignored here)
            ACE_TString name = ACE_TEXT_WCHAR_TO_TCHAR(entry->name_.fast_rep());
            if( convertStringToNSKey(name, curr_name) != 0 ) {
                continue;
            } else {
                // check component name first -> continue with next item on missmatch
                if(searchPattern.names[SmartACE::NSKeyType::COMP_NAME] != "*") {
                    if( curr_name.names[SmartACE::NSKeyType::COMP_NAME] != searchPattern.names[SmartACE::NSKeyType::COMP_NAME])
                        continue;
                }

                // next check pattern name -> continue with next item on missmatch
                if(searchPattern.names[SmartACE::NSKeyType::PATTERN_NAME] != "*") {
                    if( curr_name.names[SmartACE::NSKeyType::PATTERN_NAME] != searchPattern.names[SmartACE::NSKeyType::PATTERN_NAME])
                        continue;
                }

                // next check service name -> continue with next item on missmatch
                if(searchPattern.names[SmartACE::NSKeyType::SERVICE_NAME] != "*") {
                    if( curr_name.names[SmartACE::NSKeyType::SERVICE_NAME] != searchPattern.names[SmartACE::NSKeyType::SERVICE_NAME])
                        continue;
                }

                // next check commobj1 name -> continue with next item on missmatch
                if(searchPattern.names[SmartACE::NSKeyType::COMMOBJ1_NAME] != "*") {
                    if( curr_name.names[SmartACE::NSKeyType::COMMOBJ1_NAME] != searchPattern.names[SmartACE::NSKeyType::COMMOBJ1_NAME])
                        continue;
                }

                // next check commobj2 name -> continue with next item on missmatch
                if(searchPattern.names[SmartACE::NSKeyType::COMMOBJ2_NAME] != "*") {
                    // if the current serach-pattern does not have commobj2, the following works correctly,
                    // because in this case COMMOBJ2_NAME equals to "*" and is thus ignored!
                    if( curr_name.names[SmartACE::NSKeyType::COMMOBJ2_NAME] != searchPattern.names[SmartACE::NSKeyType::COMMOBJ2_NAME])
                        continue;
                }

                // now the current entry passed all filter steps -> we found one match!
                matching_entries.enqueue_tail(curr_name);
            }
        } // end for-each entry in binding set
    }

    return matching_entries;
}



std::map<std::string,std::string> SmartACE::NSAdapterACE::parseCommandLineParams(const int &argc, char ** argv, const int &skip_args) const
{
    ACE_TRACE ("SmartACE::NSAdapterACE::parseCommandLineParams");
    std::map<std::string,std::string> result;
    ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, guard, mutex, result);

	bool is_key = false;
	std::string param = "";
	std::string value = "";

	for(int i=skip_args; i<argc; ++i) {
		std::string arg = argv[i];
		if(arg.size()>0 && arg.at(0) == '-') {
			// this seems to be a console parameter
			size_t key_pos = arg.find_first_not_of('-');
			if(key_pos == std::string::npos) continue; // this is an ill formed param, get the next one
			else is_key = true;

			// check whether the value is directly assigned to the parameter
			size_t assign_pos = arg.find('=');
			if(assign_pos != std::string::npos && key_pos < assign_pos) {
				if((assign_pos+1) < arg.size()) {
					// param also has a value directly assigned
					// e.g. --filename=/some/path
					param = arg.substr(key_pos, (assign_pos - key_pos) );
					value = arg.substr(assign_pos+1);
					result[param] = value;
					is_key = false; // reset the key part to look for the next key
				} else {
					// the value seems to be empty, just save the key and ignore the value
					param = arg.substr(key_pos, (assign_pos - key_pos));
				}
			} else {
				// there is a key but no value, save the param name without the preceding '-'
				// e.g. --filename /some/path
				param = arg.substr(key_pos);
			}
		} else if(is_key) {
			// the current arg does not start with a '-' and we had a key already found
			// thus the current arg must be the corresponding value
			value = arg;
			result[param] = value;
			is_key = false; // reset the key part to look for the next key
			// if a param have several values, just the first one is used and the others are ignored
			// e.g. --filename /some/path /some/other/path
		}
	}

    return result;
}

int SmartACE::NSAdapterACE::setConfiguration(const std::map<std::string,std::string> &parameters)
{
    ACE_TRACE ("SmartACE::NSAdapterACE::setConfiguration");
    ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, guard, mutex, -1);
    int result = 0;

    std::string ns_ip = "";
    int ns_port = -1;
    std::string component_ip_str = "";
    bool addr_found = false;

    std::map<std::string,std::string>::const_iterator it;
    for(it=parameters.begin(); it!=parameters.end(); it++)
    {
    	if(it->first == "a" || it->first == "ns-addr") {
    		std::string addr = it->second;
    		size_t ip_pos = addr.find(':');
    		if(ip_pos != std::string::npos) {
    			ns_ip = addr.substr(0,ip_pos);
    			if( (ip_pos+1) < addr.size()) {
    				std::string port_str = addr.substr(ip_pos+1);
    				ns_port = ACE_OS::atoi(port_str.c_str());
    				addr_found = true;
    			}
    		}
    	} else if(it->first == "c" || it->first == "component-ip") {
    		component_ip_str = it->second;
    	} else if(!addr_found) {
    		if(it->first == "i" || it->first == "ns-ip") {
    			ns_ip = it->second;
    		} else if(it->first == "p" || it->first == "ns-port") {
    			std::string port_str = it->second;
    			ns_port = ACE_OS::atoi(port_str.c_str());
    		}
    	}
    } // end for each param

	const char* env_ns_ip = ACE_OS::getenv("SMART_NS_ADDR");
	// if environment variable is defined, try to extract the argument
	if(env_ns_ip != 0) {
		std::string env_var = env_ns_ip;
		// try to parse the string from the format '<hostname>:<port-nbr>'
		size_t separator_pos = env_var.find(':');
		if(separator_pos != std::string::npos && (separator_pos+1) < env_var.size())
		{
			if(ns_ip == "") {
				ns_ip = env_var.substr(0, separator_pos);
			}
			if(ns_port == -1) {
				ns_port = ACE_OS::atoi(env_var.substr(separator_pos+1).c_str());
			}
		}
	}

	const char *env_cip = ACE_OS::getenv("SMART_IP");
	if(env_cip != 0) {
		if(component_ip_str == "") {
			component_ip_str = env_cip;
		}
	}

	if(component_ip_str != "") {
		component_ip.set((u_short)0, component_ip_str.c_str());
		use_component_ip = true;
	} else {
		component_ip.set((u_short)0, ACE_LOCALHOST);
		use_component_ip = false;
	}

	if(ns_ip == "") {
		ns_ip = ACE_LOCALHOST;
	}

	if(ns_port == -1) {
		ns_port = ACE_DEFAULT_SERVER_PORT;
	}

	if(ns_ip != "" && ns_port > 0) {
		ns_address.set((u_short)ns_port, ns_ip.c_str());
		result = 0;
	}

    return result;
}

