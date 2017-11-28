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

#ifndef ACE_BUILD_SVC_DLL
 #define ACE_BUILD_SVC_DLL
#endif

#include "ace/Containers.h"
#include "ace/Get_Opt.h"
#include "ace/Singleton.h"
#include "ace/Auto_Ptr.h"
#include "NameHandler.h"
#include "ace/Signal.h"
#include "ace/OS_NS_string.h"
#include <ace/ARGV.h>
#include <ace/Get_Opt.h>

#include <ace/SString.h>

#include "smartNSValueType.hh"

#include "NameAcceptor.h"
#include "RemoteNSTask.h"

#if ACE_MAJOR_VERSION < 6 
ACE_RCSID(lib,
          Name_Handler,
          "$Id: Name_Handler.cpp 27 2011-02-16 17:07:52Z alotz $")
#endif

NameHandler::NameHandler(ACE_Thread_Manager *mg)
:	is_remote_ns_proxy(false)
,	mutex()
,	naming_context_(NULL)
{
	ACE_TRACE (ACE_TEXT ("NameHandler::NameHandler"));
}

NameHandler::~NameHandler()
{
	ACE_TRACE (ACE_TEXT ("NameHandler::~NameHandler"));
}

int NameHandler::open (void * v)
{
	ACE_TRACE (ACE_TEXT ("NameHandler::open"));

	// create the local naming-context database
	NameAcceptor* acceptor_ = static_cast<NameAcceptor*>(v);
	naming_context_ = acceptor_->naming_context();

	// Call to our parent which registers the READ_MASKin the reactor.
	if (SmartACE::ServiceHandler::open (0) == -1)
	{
		ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_TEXT ("%p\n"),
                       ACE_TEXT ("open")),
                      -1);
	}

	return 0;
}

// Create and send a reply to the client.
int NameHandler::send_reply (int status, int err)
{
	ACE_TRACE (ACE_TEXT ("NameHandler::send_reply"));

	ACE_OutputCDR cdr(ACE_CDR::DEFAULT_BUFSIZE);

	cdr << static_cast<ACE_CDR::Long>(status);
	cdr << static_cast<ACE_CDR::Long>(err);

	return send_command_message(this->name_request_.msg_type(), cdr.current());
}

int NameHandler::send_request (ACE_Name_Request &request)
{
	ACE_TRACE (ACE_TEXT ("NameHandler::send_request"));

	ACE_OutputCDR cdr(ACE_CDR::DEFAULT_BUFSIZE);

	ACE_NS_WString wname(request.name(), request.name_len()/sizeof(ACE_WCHAR_T));
	ACE_NS_WString wvalue(request.value(), request.value_len()/sizeof(ACE_WCHAR_T));

	ACE_CString name;
	if(wname.length() > 0) {
		name.set(ACE_Wide_To_Ascii(wname.fast_rep()).char_rep());
	}
	ACE_CString value( ACE_Wide_To_Ascii(wvalue.fast_rep()).char_rep() );
	ACE_CString type(request.type(), request.type_len());

	cdr << name;
	cdr << value;
	cdr << type;

	return send_command_message(request.msg_type(), cdr.current());
}

int NameHandler::handle_timeout (const ACE_Time_Value &, const void *)
{
  ACE_TRACE (ACE_TEXT ("NameHandler::handle_timeout"));
  return this->send_reply (-1, errno);
}

ACE_HANDLE NameHandler::get_handle (void) const
{
  ACE_TRACE (ACE_TEXT ("NameHandler::get_handle"));
  return this->peer().get_handle();
}

ACE_Naming_Context* NameHandler::naming_context (void)
{
  return naming_context_;
}

void NameHandler::handle_local_rebind(const ACE_CString &name, const ACE_CString &value, const ACE_CString &type)
{
	ACE_Guard<SmartACE::SmartMutex> g(mutex);
	ACE_TRACE (ACE_TEXT ("NameHandler::handle_local_rebind"));

	this->name_request_ =
			ACE_Name_Request(ACE_Name_Request::REBIND,
					ACE_TEXT_ALWAYS_WCHAR(name.fast_rep()),
					name.length() * sizeof (ACE_WCHAR_T),
					ACE_TEXT_ALWAYS_WCHAR(value.fast_rep()),
					value.length() * sizeof (ACE_WCHAR_T),
					type.c_str(),
					type.length());

	if(!is_remote_ns_proxy) {
		// propagate the regular binds/rebinds to all connected remote naming service endpoints
		int command = ACE_Name_Request::REBIND;
		REMOTE_NS::instance()->propagateRemoteCommand(command, name.c_str(), value.c_str(), type.c_str());
	}

	this->generic_bind(true);
}

int NameHandler::handle_incomming_message(ACE_CDR::Long command, ACE_InputCDR &cmd_is, ACE_InputCDR &msg_is)
{
	ACE_Guard<SmartACE::SmartMutex> g(mutex);
	ACE_TRACE (ACE_TEXT ("NameHandler::handle_incomming_message"));

	int result = 0;

	ACE_CString name, value, type;
	ACE_CString ip_str;

	switch(command) {
	case ACE_Name_Request::BIND:
	case ACE_Name_Request::REBIND:
		cmd_is >> name;
		cmd_is >> value;
		cmd_is >> type;

		// ACE_DEBUG((LM_DEBUG, ACE_TEXT("BIND/REBIND -- NAME: %s, VALUE: %s, TYPE: %s\n"), name.c_str(), value.c_str(), type.c_str()));

		if(type == SmartACE::NSValueType::getSmartType())
		{
			if(is_remote_ns_proxy)
			{
				size_t delimiter_pos = value.find(':');
				if(delimiter_pos != ACE_CString::npos) {
					ACE_CString curr_ip = value.substr(0, delimiter_pos);
					if(curr_ip == ACE_TEXT("127.0.0.1") || curr_ip == ACE_TEXT("localhost")) {
						// localhost IPs must be replaced by the remote address
						ACE_INET_Addr addr;
						this->peer().get_remote_addr(addr);
						ip_str = addr.get_host_addr();
						value = ip_str + value.substr(delimiter_pos);
					}
				}
			} else {
				ACE_INET_Addr addr;
				this->peer().get_remote_addr(addr);
				ip_str = addr.get_host_addr();
				//<alexej 2013-11-21>
				if(value.find(':') != ACE_CString::npos) {
					value = ip_str + value.substr(value.find(':'));
				}else{
					// this is for backwards compatibility
					// just in case some old components try to communicate with the new naming service
					value = ip_str + ':' + value;
				}
				//</alexej>
			}
			// ACE_DEBUG((LM_DEBUG, ACE_TEXT("exchanged VALUE: %s\n"), value.c_str()));

			if(!REMOTE_NS::instance()->isInitialized()) {
				// naming service is not yet fully initialized
				// postpone the regular rebinds (of local components) until the NS
				// is fully initialized
				REMOTE_NS::instance()->postponeLocalRebind(this, name, value, type);
				return 0;
			}

		} else if(type == REMOTE_NS::instance()->getRemoteNsType()) {
			// set the message type (required to properly send reply messages, see below)
			this->name_request_.msg_type(command);

			if(name == "CREATE_ENDPOINT") {
				is_remote_ns_proxy = true;
				ACE_INET_Addr addr;
				this->peer().get_remote_addr(addr);
				ip_str = ACE_CString(addr.get_host_addr()) + ":" + value;

				send_reply( REMOTE_NS::instance()->createEndpoint(ip_str.c_str()) );
			} else if(name == "SET_ENDPOINT_FLAG") {
				is_remote_ns_proxy = true;
				send_reply(0);
			} else if(name == "CAN_YOU_SYNC?") {
				if(REMOTE_NS::instance()->isInitialized()) {
					send_reply(0);
				} else {
					send_reply(-1);
				}
			}
			// do not continue as this is a special (internal) NS-to-NS communication
			return 0;
		}

		this->name_request_ =
				ACE_Name_Request(command,
						ACE_TEXT_ALWAYS_WCHAR(name.fast_rep()),
						name.length() * sizeof (ACE_WCHAR_T),
						ACE_TEXT_ALWAYS_WCHAR(value.fast_rep()),
						value.length() * sizeof (ACE_WCHAR_T),
						type.c_str(),
						type.length());

		if(!is_remote_ns_proxy) {
			// propagate the regular binds/rebinds to all connected remote naming service endpoints
			REMOTE_NS::instance()->propagateRemoteCommand(command, name.c_str(), value.c_str(), type.c_str());
		}

		break;
	case ACE_Name_Request::UNBIND:
	case ACE_Name_Request::RESOLVE:
		// fall through
	case ACE_Name_Request::LIST_NAMES:
	case ACE_Name_Request::LIST_VALUES:
	case ACE_Name_Request::LIST_TYPES:
	case ACE_Name_Request::LIST_NAME_ENTRIES:
	case ACE_Name_Request::LIST_VALUE_ENTRIES:
	case ACE_Name_Request::LIST_TYPE_ENTRIES:

		// get the name (it is either a key to unbind/resolve or the search pattern)
		cmd_is >> name;

		this->name_request_ =
				ACE_Name_Request(command,
						ACE_TEXT_ALWAYS_WCHAR(name.fast_rep()),
						name.length() * sizeof (ACE_WCHAR_T),
						0, 0,
						0, 0);

		if(!is_remote_ns_proxy && command == ACE_Name_Request::UNBIND) {
			// propagate the regular binds/rebinds to all connected remote naming service endpoints
			REMOTE_NS::instance()->propagateRemoteCommand(command, name.c_str());
		}

		break;
	default:
		this->send_reply (-1, errno);
		break;
	}

	// now call the appropriate functions
	switch(command)
	{
	case ACE_Name_Request::BIND:
		result = this->generic_bind(false);
		break;
	case ACE_Name_Request::REBIND:
		result = this->generic_bind(true);
		break;
	case ACE_Name_Request::UNBIND:
		result = this->unbind();
		break;
	case ACE_Name_Request::RESOLVE:
		result = this->resolve();
		break;
	case ACE_Name_Request::LIST_NAMES:
	case ACE_Name_Request::LIST_VALUES:
	case ACE_Name_Request::LIST_TYPES:
		result = generic_list_x();
		break;
	case ACE_Name_Request::LIST_NAME_ENTRIES:
	case ACE_Name_Request::LIST_VALUE_ENTRIES:
	case ACE_Name_Request::LIST_TYPE_ENTRIES:
		result = generic_list_x_entries();
		break;
	default:
		// do nothing
		break;
	}

	return result;
}

int NameHandler::generic_bind(const bool &rebind)
{
	ACE_TRACE (ACE_TEXT ("NameHandler::generic_bind"));

	// convert name and value into NS_WString representation
	ACE_NS_WString a_name (this->name_request_.name (),
	                       this->name_request_.name_len () / sizeof (ACE_WCHAR_T));
	ACE_NS_WString a_value(this->name_request_.value (),
	                       this->name_request_.value_len () / sizeof (ACE_WCHAR_T));
	ACE_CString ctype(this->name_request_.type(), this->name_request_.type_len());

	if(rebind == true)
	{
		// propagate the rebind request to internal (local) name-space
		int result = this->naming_context()->rebind(a_name, a_value, ctype.fast_rep());
		if(result == 1 || result == 0) {
			// all good
			return this->send_reply(0);
		}

	} else {
		// rebind == false => bind only
		// propagate the bind request to internal (local) name-space
		int result = this->naming_context()->bind(a_name, a_value, ctype.fast_rep());
		if(result == 0) {
			return this->send_reply(0);
		}
	}

	return this->send_reply(-1);
}

int NameHandler::resolve()
{
	ACE_TRACE (ACE_TEXT ("NameHandler::resolve"));

	int result = 0;

	ACE_NS_WString a_name(this->name_request_.name(),
	                      this->name_request_.name_len() / sizeof(ACE_WCHAR_T));

	// successful resolve is indicated to the remote end-point by
	// replying with a RESOLVE message containing the value

	ACE_NS_WString avalue;
	char *atype;
	if(this->naming_context()->resolve(a_name, avalue, atype) == 0)
	{
		ACE_Auto_Basic_Array_Ptr<ACE_WCHAR_T> avalue_urep (avalue.rep ());
		ACE_Name_Request nrq (ACE_Name_Request::RESOLVE,
	                          0,
	                          0,
	                          avalue_urep.get (),
	                          avalue.length () * sizeof (ACE_WCHAR_T),
	                          atype, ACE_OS::strlen (atype));
		delete[] atype;
		return this->send_request (nrq);
	}

	ACE_Name_Request nrq (ACE_Name_Request::BIND, 0, 0, 0, 0, 0, 0);
	return this->send_request (nrq);
}

int NameHandler::unbind(void)
{
  ACE_TRACE (ACE_TEXT ("NameHandler::unbind"));

  ACE_NS_WString a_name (this->name_request_.name (),
                         this->name_request_.name_len () / sizeof (ACE_WCHAR_T));

  // propagate the unbind request to the internal (local) name-space
  // then according to the result, return a reply
  if (this->naming_context()->unbind(a_name) == 0)
	  return this->send_reply(0);
  else
	  return this->send_reply(-1);
}

int NameHandler::generic_list_x(void)
{
  ACE_TRACE (ACE_TEXT ("NameHandler::generic_list_x"));

  ACE_PWSTRING_SET set;
  ACE_NS_WString pattern (this->name_request_.name (),
                          this->name_request_.name_len () / sizeof (ACE_WCHAR_T));
  ACE_Name_Request end_rq (ACE_Name_Request::MAX_ENUM, 0, 0, 0, 0, 0, 0);

  int result = 0;

  int curr_msg_type = this->name_request_.msg_type();

  switch(curr_msg_type)
  {
	case ACE_Name_Request::LIST_NAMES:
		result = this->naming_context()->list_names(set, pattern);
		break;
	case ACE_Name_Request::LIST_VALUES:
		result = this->naming_context()->list_values(set, pattern);
		break;
	case ACE_Name_Request::LIST_TYPES:
		result = this->naming_context()->list_types(set, pattern);
		break;
	default:
		// send an empty result as msg_type is incompatible
		result = this->send_request(end_rq);
		break;
  }

  if(result == 0)
  {
	  ACE_PWSTRING_ITERATOR set_iterator (set);

      // iterate the resulting set
      for (ACE_NS_WString *entry = 0; set_iterator.next(entry) !=0; set_iterator.advance())
      {
    	  ACE_Auto_Basic_Array_Ptr<ACE_WCHAR_T> entry_urep (entry->rep());

    	  if(curr_msg_type == ACE_Name_Request::LIST_NAMES) {
    		  // create a name-entry
    		  ACE_Name_Request nrq(curr_msg_type,
    				  entry_urep.get(), entry->length() * sizeof(ACE_WCHAR_T), 0,0,0,0);
    		  if(this->send_request(nrq) == -1) {
    			  return -1;
    		  }
    	  } else if(curr_msg_type == ACE_Name_Request::LIST_VALUES) {
    		  // create a value-entry
    		  ACE_Name_Request nrq(curr_msg_type,
    				  0,0, entry_urep.get(), entry->length() * sizeof(ACE_WCHAR_T), 0,0);
    		  if(this->send_request(nrq) == -1) {
    			  return -1;
    		  }
    	  } else if(curr_msg_type == ACE_Name_Request::LIST_TYPES) {
    		  // create a type-entry
    		  ACE_Name_Request nrq(curr_msg_type,
    				  0,0,0,0, entry->char_rep(), entry->length());
    		  if(this->send_request(nrq) == -1) {
    			  return -1;
    		  }
    	  }
      }
  }

  // return the stop message
  return this->send_request(end_rq);
}

int NameHandler::generic_list_x_entries(void)
{
	ACE_TRACE (ACE_TEXT ("NameHandler::generic_list_x_entries"));

	ACE_BINDING_SET set;
	ACE_NS_WString pattern (this->name_request_.name (),
	                        this->name_request_.name_len () / sizeof (ACE_WCHAR_T));
	ACE_Name_Request end_rq (ACE_Name_Request::MAX_ENUM, 0, 0, 0, 0, 0, 0);

	int result = 0;

	switch(this->name_request_.msg_type())
	{
	case ACE_Name_Request::LIST_NAME_ENTRIES:
		result = this->naming_context()->list_name_entries(set, pattern);
		break;
	case ACE_Name_Request::LIST_VALUE_ENTRIES:
		result = this->naming_context()->list_value_entries(set, pattern);
		break;
	case ACE_Name_Request::LIST_TYPE_ENTRIES:
		result = this->naming_context()->list_type_entries(set, pattern);
		break;
	default:
		// send an empty result as msg_type is incompatible
		result = this->send_request(end_rq);
		break;
	}

	if (result == 0)
	{
		ACE_BINDING_ITERATOR set_iterator(set);

		// iterate the resulting binding set and send each entry one after another
		for(ACE_Name_Binding *one_entry = 0; set_iterator.next(one_entry) != 0; set_iterator.advance())
		{
			ACE_Auto_Basic_Array_Ptr<ACE_WCHAR_T> name_urep(one_entry->name_.rep());
			ACE_Auto_Basic_Array_Ptr<ACE_WCHAR_T> value_urep(one_entry->value_.rep());
			ACE_Name_Request mynrq(this->name_request_.msg_type(),
                                  name_urep.get(),
                                  one_entry->name_.length() * sizeof(ACE_WCHAR_T),
                                  value_urep.get(),
                                  one_entry->value_.length() * sizeof(ACE_WCHAR_T),
                                  one_entry->type_,
                                  ACE_OS::strlen(one_entry->type_));

			if (this->send_request (mynrq) == -1) {
				return -1;
			}
        } // end for-each entry
    }

	// send a stop message in any case at the end
	return this->send_request(end_rq);
}

