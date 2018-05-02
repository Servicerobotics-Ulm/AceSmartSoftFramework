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

#ifndef NAMEHANDLER_H
#define NAMEHANDLER_H

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include <ace/SString.h>
#include <ace/Naming_Context.h>
#include <ace/Name_Request_Reply.h>
#include <ace/Null_Mutex.h>
#include <ace/svc_export.h>

#include "smartServiceHandler.hh"
#include "smartOSMapping.hh"

#include <iostream>


#if defined ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION_EXPORT
template class ACE_Svc_Export ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>;
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION_EXPORT */

/**
 * @class NameHandler
 *
 * @brief Product object created by <ACE_Name_Acceptor>.  An
 * <NameHandler> exchanges messages with a <ACE_Name_Proxy>
 * object on the client-side.
 *
 * This class is the main workhorse of the <ACE_Name_Server>.  It
 * handles client requests to bind, rebind, resolve, and unbind
 * names.  It also schedules and handles timeouts that are used to
 * support "timed waits."  Clients used timed waits to bound the
 * amount of time they block trying to get a name.
 */
class NameHandler : public SmartACE::ServiceHandler
{
public:
	/// Default constructor.
	NameHandler(ACE_Thread_Manager *mg=0);

	/// Activate this instance of the <ACE_Name_Handler> (called by the
	/// <ACE_Strategy_Acceptor>).
	virtual int open (void * = 0);

	void handle_local_rebind(const ACE_CString &name, const ACE_CString &value, const ACE_CString &type);

protected:
	/// Create and send a reply to the client.
	virtual int send_reply (int status, int errnum = 0);

	/// Special kind of reply
	virtual int send_request (ACE_Name_Request &);

	// = Demultiplexing hooks.
	/// Return the underlying <ACE_HANDLE>.
	virtual ACE_HANDLE get_handle (void) const;

	/// Callback method invoked by the <ACE_Reactor> when client events
	/// arrive.
	virtual int handle_incomming_message(ACE_CDR::Long command, ACE_InputCDR &cmd_is, ACE_InputCDR &msg_is);

	virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

	// = Timer hook.
	/// Enable clients to limit the amount of time they wait for a name.
	virtual int handle_timeout (const ACE_Time_Value &tv, const void *arg);

	// ensure dynamic allocation
	virtual ~NameHandler();

private:
	bool is_remote_ns_proxy;

	enum REPLY_ID { REPLY_OK=0, REPLY_ERROR=-1, REPLY_ALREADY_BOUND=-2, REPLY_ERROR_COMMUNICATION=-3 };

	/// Cache request from the client.
	ACE_Name_Request name_request_;

	/// Special kind of reply for resolve and list names.
	ACE_Name_Request name_request_back_;

	/// Cache reply to the client.
	ACE_Name_Reply name_reply_;

	/// Address of client we are connected with.
	ACE_INET_Addr addr_;

	int generic_bind(const bool &rebind=true);

	/// Handle find requests.
	int resolve (void);

	/// Handle unbind requests.
	int unbind (void);

	/// Handle LIST_NAMES, LIST_VALUES, and LIST_TYPES requests.
	int generic_list_x (void);

	/// Handle LIST_NAME_ENTRIES, LIST_VALUE_ENTRIES, and
	/// LIST_TYPE_ENTRIES requests.
	int generic_list_x_entries (void);
};

#endif /* NAMEHANDLER_H */
