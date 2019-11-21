// --------------------------------------------------------------------------
//
//  Copyright (C) 2002/2004/2008 Christian Schlegel
//                2009 Alex Lotz
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

#include "smartEventServerPattern.hh"

namespace SmartACE
{

/////////////////////////////////////////////////////////////////////////
//
// ServiceHandler part
//
/////////////////////////////////////////////////////////////////////////

EventServerServiceHandler::EventServerServiceHandler(ACE_Reactor *reactor)
:  SmartACE::ServiceHandler(reactor)
, acceptor(0)
{
	// nothing to do here so far, parent constructor of SmartServiceHandler is doing the work
}

EventServerServiceHandler::~EventServerServiceHandler()
{
	// all dynamic memmory objects are handled by parent class
	// so no memmory clean up is necessary here
}

int EventServerServiceHandler::open(void *handler)
{
	if (handler != 0) {
		acceptor = static_cast<EventServerAcceptor*>(handler);
	}
	return SmartACE::ServiceHandler::open(handler);
}

int EventServerServiceHandler::handle_incomming_message(ACE_CDR::Long command, ACE_InputCDR &cmd_is, ACE_InputCDR &msg_is)
{
	//<alexej date="2010-03-18">
	// local variables
	ACE_CDR::Long cid = 0;

	ACE_Utils::UUID uuid;
	ACE_CString uuid_str;
	//</alexej>

	if (acceptor == 0) {
		// well, this case is quite unlikely, but just in case...
		return 0;
	}

	ACE_CDR::Long mode = 0;
	ACE_CDR::ULongLong event_id = 0;

	//
	// command - switch case
	//
	switch(command) {
 	case SmartACE::CMD_ACTIVATE:
         if(cmd_is.length() > 0 && msg_is.length() > 0)
         {
            // get mode value
            cmd_is >> mode;
            // get aid value
            cmd_is >> event_id;

            // call activation handler
            acceptor->handleActivate(this, mode, event_id, msg_is.start());
         }
         break;

	case SmartACE::CMD_DEACTIVATE:
         if(cmd_is.length() > 0)
         {
            // get aid value
            cmd_is >> event_id;

            // call deactivation handler
            acceptor->handleDeactivate(this, event_id);
         }
         break;

	case SmartACE::CMD_CONNECT:
		if (cmd_is.length() > 0) {
			//<alexej date="2011-02-24">
			//read connection-id
			cmd_is >> cid;

			// read uuid string
			cmd_is >> uuid_str;

			// restore uuid from incomming string
			uuid.from_string(uuid_str);

			// callback
			acceptor->handleConnect(this, cid, uuid);
			//</alexej>
		}
		break;

	case SmartACE::CMD_DISCARD:
//printf("server->handle_incomming_message->CMD_DISCARD\n");
		acceptor->handleDiscard(this);
		break;

	case SmartACE::CMD_DISCONNECT:
//printf("server->handle_incomming_message->CMD_DISCONNECT\n");
		acceptor->handleDisconnect(this);
		break;
	};

	return 0;
}

Smart::StatusCode EventServerServiceHandler::event(const SmartACE::SmartMessageBlock *user, size_t id)
{
	Smart::StatusCode result = Smart::SMART_OK;

	ACE_OutputCDR cmd_cdr(ACE_CDR::LONGLONG_SIZE);
	cmd_cdr << ACE_Utils::truncate_cast<ACE_CDR::ULongLong>(id);

	if ( this->send_command_message(SmartACE::CMD_EVENT, cmd_cdr.current(), user) != 0 ) {
		if(this->is_disconnected()) {
			result = Smart::SMART_DISCONNECTED;
		}else{
			result = Smart::SMART_ERROR_COMMUNICATION;
		}
	}

	return result;
}

Smart::StatusCode EventServerServiceHandler::serverInitiatedDisconnect(int cid)
{
	Smart::StatusCode result = Smart::SMART_OK;

	ACE_OutputCDR cdr(ACE_CDR::LONG_SIZE);
	cdr << ACE_Utils::truncate_cast<ACE_CDR::Long>(cid);

	if ( this->send_command_message(SmartACE::CMD_SERVER_INITIATED_DISCONNECT, cdr.current()) != 0 ) {
		if(this->is_disconnected()) {
			result = Smart::SMART_DISCONNECTED;
		}else{
			result = Smart::SMART_ERROR_COMMUNICATION;
		}
	}

	return result;
}

Smart::StatusCode EventServerServiceHandler::acknowledgmentConnect(int cid, int status)
{
	Smart::StatusCode result = Smart::SMART_OK;

	ACE_OutputCDR cdr(ACE_CDR::LONG_SIZE*2);
	cdr << ACE_Utils::truncate_cast<ACE_CDR::Long>(cid);
	cdr << ACE_Utils::truncate_cast<ACE_CDR::Long>(status);

	if ( this->send_command_message(SmartACE::CMD_ACK_CONNECT, cdr.current()) != 0 ) {
		if(this->is_disconnected()) {
			result = Smart::SMART_DISCONNECTED;
		}else{
			result = Smart::SMART_ERROR_COMMUNICATION;
		}
	}

	return result;
}

Smart::StatusCode EventServerServiceHandler::acknowledgmentDisconnect()
{
	Smart::StatusCode result = Smart::SMART_OK;

	if ( this->send_command_message(SmartACE::CMD_ACK_DISCONNECT) != 0 ) {
		if(this->is_disconnected()) {
			result = Smart::SMART_DISCONNECTED;
		}else{
			result = Smart::SMART_ERROR_COMMUNICATION;
		}
	}

	return result;
}

Smart::StatusCode EventServerServiceHandler::acknowledgmentActivate(size_t id)
{
	Smart::StatusCode result = Smart::SMART_OK;

	ACE_OutputCDR cdr(ACE_CDR::LONGLONG_SIZE);
	cdr << ACE_Utils::truncate_cast<ACE_CDR::ULongLong>(id);

	if( this->send_command_message(SmartACE::CMD_ACK_ACTIVATE, cdr.current()) != 0 ) {
		if(this->is_disconnected()) {
			result = Smart::SMART_DISCONNECTED;
		}else{
			result = Smart::SMART_ERROR_COMMUNICATION;
		}
	}

	return result;
}

/////////////////////////////////////////////////////////////////////////
//
// acceptor part
//
/////////////////////////////////////////////////////////////////////////

EventServerAcceptor::EventServerAcceptor(void    *ptr,
   void (*callbackCnct)      (void *, const EventServerServiceHandler*,int,const ACE_Utils::UUID&),
   void (*callbackDiscrd)    (void *, const EventServerServiceHandler*),
   void (*callbackDiscon)    (void *, const EventServerServiceHandler*),
   void (*callbackActivate)  (void *, const EventServerServiceHandler*, const int&, const size_t&, const SmartMessageBlock *), // activation handler
   void (*callbackDeactivate)(void *, const EventServerServiceHandler*, const size_t&),      // deactivation handler
   ACE_Reactor *reactor)
:  ACE_Acceptor<EventServerServiceHandler, ACE_SOCK_ACCEPTOR>(reactor)
,  _reactor(reactor)
{
   lthis = ptr;

   hndCnctPtr       = callbackCnct;
   hndDiscrdPtr     = callbackDiscrd;
   hndDisconPtr     = callbackDiscon;
   hndActivatePtr   = callbackActivate;
   hndDeactivatePtr = callbackDeactivate;
}

EventServerAcceptor::~EventServerAcceptor()
{
	// no dynamic objects used...
}

int EventServerAcceptor::close_handler() {
	//ACE_DEBUG((LM_DEBUG, ACE_TEXT("EventServerAcceptor::close_handler()\n") ));
	_reactor->remove_handler(this, ACE_Event_Handler::READ_MASK);
	return 0;
}

} // END namespace SmartACE
//</alexej>
