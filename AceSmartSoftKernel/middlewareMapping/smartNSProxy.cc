// --------------------------------------------------------------------------
//
//  Copyright (C) 2012-2014 Alex Lotz
//
//        lotz@hs-ulm.de
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


#include "smartNSProxy.hh"

#include <ace/SString.h>
#include <ace/Name_Request_Reply.h>

namespace SmartACE {

NSProxy::NSProxy()
:  SmartACE::ServiceHandler()
{   }

NSProxy::~NSProxy()
{   }

int NSProxy::handle_incomming_message(ACE_CDR::Long command, ACE_InputCDR &cmd_is, ACE_InputCDR &msg_is)
{
	int response = REPLY_ERROR;

	ACE_CDR::Long status, errnum;

	switch(command) {
	case ACE_Name_Request::BIND:
	case ACE_Name_Request::REBIND:
	case ACE_Name_Request::UNBIND:
		cmd_is >> status;
		cmd_is >> errnum;

		response = status;
		break;
	case ACE_Name_Request::RESOLVE:
		cmd_is >> this->incoming_name;
		cmd_is >> this->incoming_value;
		cmd_is >> this->incoming_type;

		response = REPLY_OK;
		break;

	case ACE_Name_Request::LIST_NAMES:
		cmd_is >> this->incoming_name;
		response = REPLY_CONTINUE;
		break;
	case ACE_Name_Request::LIST_VALUES:
		cmd_is >> this->incoming_value;
		response = REPLY_CONTINUE;
		break;
	case ACE_Name_Request::LIST_TYPES:
		cmd_is >> this->incoming_type;
		response = REPLY_CONTINUE;
		break;
	case ACE_Name_Request::LIST_NAME_ENTRIES:
	case ACE_Name_Request::LIST_VALUE_ENTRIES:
	case ACE_Name_Request::LIST_TYPE_ENTRIES:
		cmd_is >> this->incoming_name;
		cmd_is >> this->incoming_value;
		cmd_is >> this->incoming_type;
		response = REPLY_CONTINUE;
		break;

	case ACE_Name_Request::MAX_ENUM:
		// last element in the list requested
		response = REPLY_OK;
		break;

	default:
		ACE_DEBUG((LM_INFO, ACE_TEXT("SmartACE::NSProxy::handle_incomming_message(): ERROR unknown command: %d\n"), command));
		response = REPLY_ERROR;
		break;
	}

	return response;
}

int NSProxy::open (void *handler)
{
	// The default implementation of the ACE_Avc_Handler::open() method activates the "read mask".
	// This behavior is unwanted here, since we implement a simple request/response
	// communication mechanism without using the rich functionality of a service handler.
	// Therefore we simply return 0 without any further initialization.
	return 0;
}

int NSProxy::bind (const ACE_NS_WString &name_in,
	                    const ACE_NS_WString &value_in,
	                    const char *type_in)
{
	int result = REPLY_OK;

	ACE_CString name = ACE_TEXT_WCHAR_TO_TCHAR(name_in.fast_rep());
	ACE_CString value = ACE_TEXT_WCHAR_TO_TCHAR(value_in.fast_rep());
	ACE_CString type(type_in);


	ACE_OutputCDR cdr(ACE_CDR::DEFAULT_BUFSIZE);

	cdr << name;
	cdr << value;
	cdr << type;

	if ( this->send_command_message(ACE_Name_Request::BIND, cdr.current()) != REPLY_OK ) {
		result = REPLY_ERROR;
	} else {
		if(this->handle_input() != REPLY_OK) result = REPLY_ERROR;
	}

	return result;
}

int NSProxy::rebind (const ACE_NS_WString &name_in,
	                    const ACE_NS_WString &value_in,
	                    const char *type_in)
{
	int result = REPLY_OK;

	ACE_CString name = ACE_TEXT_WCHAR_TO_TCHAR(name_in.fast_rep());
	ACE_CString value = ACE_TEXT_WCHAR_TO_TCHAR(value_in.fast_rep());
	ACE_CString type(type_in);


	ACE_OutputCDR cdr(ACE_CDR::DEFAULT_BUFSIZE);

	cdr << name;
	cdr << value;
	cdr << type;

	if ( this->send_command_message(ACE_Name_Request::REBIND, cdr.current()) != REPLY_OK ) {
		result = REPLY_ERROR;
	} else {
		if(this->handle_input() != REPLY_OK) result = REPLY_ERROR;
	}

	return result;
}

int NSProxy::unbind (const ACE_NS_WString &name_in)
{
	int result = REPLY_OK;

	ACE_CString name = ACE_TEXT_WCHAR_TO_TCHAR(name_in.fast_rep());

	ACE_OutputCDR cdr(ACE_CDR::DEFAULT_BUFSIZE);

	cdr << name;

	if ( this->send_command_message(ACE_Name_Request::UNBIND, cdr.current()) != REPLY_OK ) {
		result = REPLY_ERROR;
	} else {
		if(this->handle_input() != REPLY_OK) result = REPLY_ERROR;
	}
	return result;
}

int NSProxy::resolve (const ACE_NS_WString &name_in,
		ACE_NS_WString &value_out,
        char *&type_out)
{
	int result = REPLY_OK;

	ACE_CString name = ACE_TEXT_WCHAR_TO_TCHAR(name_in.fast_rep());


	ACE_OutputCDR cdr(ACE_CDR::DEFAULT_BUFSIZE);

	cdr << name;

	if ( this->send_command_message(ACE_Name_Request::RESOLVE, cdr.current()) != REPLY_OK ) {
		result = REPLY_ERROR;
	} else {
		if(this->handle_input() != REPLY_OK) result = REPLY_ERROR;

		value_out = ACE_TEXT_ALWAYS_WCHAR(incoming_value.c_str());
		type_out = incoming_type.rep();
	}

	return result;
}

int NSProxy::list_names (ACE_WSTRING_SET &set_out,
                        const ACE_NS_WString &pattern_in)
{
	return lists(set_out, pattern_in, ACE_Name_Request::LIST_NAMES);
}


int NSProxy::list_values (ACE_WSTRING_SET &set_out,
                         const ACE_NS_WString &pattern_in)
{
	return lists(set_out, pattern_in, ACE_Name_Request::LIST_VALUES);
}


int NSProxy::list_types (ACE_WSTRING_SET &set_out,
                        const ACE_NS_WString &pattern_in)
{
	return lists(set_out, pattern_in, ACE_Name_Request::LIST_TYPES);
}

int NSProxy::lists(ACE_WSTRING_SET &set_out,
        const ACE_NS_WString &pattern_in,
        const ACE_Name_Request::Constants &command_in)
{
	int result = REPLY_OK;

	ACE_CString pattern = ACE_TEXT_WCHAR_TO_TCHAR(pattern_in.fast_rep());

	ACE_OutputCDR cdr(ACE_CDR::DEFAULT_BUFSIZE);

	cdr << pattern;

	if ( this->send_command_message(command_in, cdr.current()) != REPLY_OK ) {
		result = REPLY_ERROR;
	} else {
		result = handle_input();
		while( result  == REPLY_CONTINUE ) {
			ACE_NS_WString temp;
			switch(command_in) {
			case ACE_Name_Request::LIST_NAMES:
				temp = ACE_TEXT_ALWAYS_WCHAR(incoming_name.c_str());
				break;
			case ACE_Name_Request::LIST_VALUES:
				temp = ACE_TEXT_ALWAYS_WCHAR(incoming_value.c_str());
				break;
			case ACE_Name_Request::LIST_TYPES:
				temp = ACE_TEXT_ALWAYS_WCHAR(incoming_type.c_str());
				break;
			default:
				break;
			};
			set_out.insert (temp);

			// if(result == REPLY_CONTINUE): continue to receive the next element in the list
			// else if(result == REPLY_OK): the last element received -> stop the loop
			// else: error occurred -> stop the loop
			result = handle_input();
		}
		if(result != REPLY_OK) result = REPLY_ERROR;
	}

	return result;
}

int NSProxy::list_name_entries (ACE_BINDING_SET &set_out,
                               const ACE_NS_WString &pattern_in)
{
	return this->list_entries(set_out, pattern_in, ACE_Name_Request::LIST_NAME_ENTRIES);
}


int NSProxy::list_value_entries (ACE_BINDING_SET &set_out,
                                const ACE_NS_WString &pattern_in)
{
	return this->list_entries(set_out, pattern_in, ACE_Name_Request::LIST_VALUE_ENTRIES);
}


int NSProxy::list_type_entries (ACE_BINDING_SET &set_out,
                               const ACE_NS_WString &pattern_in)
{
	return this->list_entries(set_out, pattern_in, ACE_Name_Request::LIST_TYPE_ENTRIES);
}

int NSProxy::list_entries(ACE_BINDING_SET &set_out,
        const ACE_NS_WString &pattern_in,
        const ACE_Name_Request::Constants &command_in)
{
	int result = REPLY_OK;

	ACE_CString name = ACE_TEXT_WCHAR_TO_TCHAR(pattern_in.fast_rep());

	ACE_OutputCDR cdr(ACE_CDR::DEFAULT_BUFSIZE);

	cdr << name;

	if ( this->send_command_message(command_in, cdr.current()) != 0 ) {
		result = REPLY_ERROR;
	} else {
		result = handle_input();
		while( result  == REPLY_CONTINUE ) {
 			ACE_NS_WString wname ( ACE_TEXT_ALWAYS_WCHAR(incoming_name.c_str()) );
 			ACE_NS_WString wvalue ( ACE_TEXT_ALWAYS_WCHAR(incoming_value.c_str()) );

 			ACE_Name_Binding entry (wname, wvalue, incoming_type.c_str());
			set_out.insert (entry);

			// if(result == REPLY_CONTINUE): continue to receive the next element in the list
			// else if(result == REPLY_OK): the last element received -> stop the loop
			// else: error occurred -> stop the loop
			result = handle_input();
		}
		if(result != REPLY_OK) result = REPLY_ERROR;
	}

	return result;
}

void NSProxy::dump (void) const
{
	ACE_DEBUG((LM_INFO, ACE_TEXT("SmartACE::NSProxy::dump()\n")));
}

} /* namespace Smart */
