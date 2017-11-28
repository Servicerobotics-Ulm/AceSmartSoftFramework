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

#include "smartPushServerPattern.hh"

namespace SmartACE
{

/////////////////////////////////////////////////////////////////////////
//
// ServiceHandler part
//
/////////////////////////////////////////////////////////////////////////

PushServerServiceHandler::PushServerServiceHandler(ACE_Reactor *reactor)
:  SmartACE::ServiceHandler(reactor)
, acceptor(0)
{
	// nothing to do here so far, parent constructor of SmartServiceHandler is doing the work
}

PushServerServiceHandler::~PushServerServiceHandler()
{
	// all dynamic memmory objects are handled by parent class
	// so no memmory clean up is necessary here
}

int PushServerServiceHandler::open(void *handler)
{
	if (handler != 0) {
		acceptor = static_cast<PushServerAcceptor*>(handler);
	}
	return SmartACE::ServiceHandler::open(handler);
}

int PushServerServiceHandler::handle_incomming_message(ACE_CDR::Long command, ACE_InputCDR &cmd_is, ACE_InputCDR &msg_is)
{
	//<alexej date="2010-03-18">
	// local variables
	ACE_CDR::Long cid = 0;

	ACE_Utils::UUID uuid;
	ACE_CString uuid_str;
	//</alexej>

	ACE_CDR::Long sid = 0;

	if (acceptor == 0) {
		// well, this case is quite unlikely, but just in case...
		return 0;
	}

	ACE_CDR::Long i_temp = 0;

	//
	// command - switch case
	//
	switch(command) {

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

	case SmartACE::CMD_SUBSCRIBE:
		if(cmd_is.length() > 0)
		{
			cmd_is >> i_temp;
			cmd_is >> sid;

//printf("server->handle_incomming_message->CMD_SUBSCRIBE\n");
		   // subscribe client to the server and
		   // reply the current state to the client immediately
			acceptor->handleSubscribe(this, i_temp, sid) ;
		}
		break;

	case SmartACE::CMD_UNSUBSCRIBE:
//printf("server->handle_incomming_message->CMD_UNSUBSCRIBE\n");
		acceptor->handleUnsubscribe(this);
		break;

	case SmartACE::CMD_GET_SERVER_INFO:
//printf("server->handle_incomming_message->CMD_GET_SERVER_INFO\n");
		// get server information data
		acceptor->handleServerInformation(this);
		break;
	};

	return 0;
}

Smart::StatusCode PushServerServiceHandler::update(SmartMessageBlock *message, int sid)
{
	Smart::StatusCode result = Smart::SMART_OK;

	ACE_OutputCDR cdr(ACE_CDR::LONG_SIZE);
	cdr << ACE_Utils::truncate_cast<ACE_CDR::Long>(sid);

	if ( this->send_command_message(SmartACE::CMD_UPDATE, cdr.current(), message) != 0 ) {
		if(this->is_disconnected()) {
			result = Smart::SMART_DISCONNECTED;
		}else{
			result = Smart::SMART_ERROR_COMMUNICATION;
		}
	}

	return result;
}

Smart::StatusCode PushServerServiceHandler::serverInitiatedDisconnect(int cid)
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

Smart::StatusCode PushServerServiceHandler::acknowledgmentConnect(int cid, int status)
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

Smart::StatusCode PushServerServiceHandler::acknowledgmentDisconnect()
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

Smart::StatusCode PushServerServiceHandler::acknowledgmentSubscribe(int active)
{
	Smart::StatusCode result = Smart::SMART_OK;

	size_t length = sizeof(int);

	ACE_OutputCDR cdr(length);
	cdr << ACE_Utils::truncate_cast<ACE_CDR::Long>(active);

	if( this->send_command_message(SmartACE::CMD_ACK_SUBSCRIBE, cdr.current()) != 0 ) {
		if(this->is_disconnected()) {
			result = Smart::SMART_DISCONNECTED;
		}else{
			result = Smart::SMART_ERROR_COMMUNICATION;
		}
	}

	return result;
}

Smart::StatusCode PushServerServiceHandler::serverInformation(unsigned long cycle, int active)
{
	Smart::StatusCode result = Smart::SMART_OK;

	size_t length = ACE_CDR::LONG_SIZE + ACE_CDR::LONG_SIZE;

	ACE_OutputCDR cdr(length);
	cdr << ACE_Utils::truncate_cast<ACE_CDR::ULongLong>(cycle);
	cdr << ACE_Utils::truncate_cast<ACE_CDR::Long>(active);

	if( this->send_command_message(SmartACE::CMD_SERVER_INFO, cdr.current()) != 0 ) {
		if(this->is_disconnected()) {
			result = Smart::SMART_DISCONNECTED;
		}else{
			result = Smart::SMART_ERROR_COMMUNICATION;
		}
	}

	return result;
}

Smart::StatusCode PushServerServiceHandler::activationState(int active)
{
	Smart::StatusCode result = Smart::SMART_OK;

	ACE_OutputCDR cdr(ACE_CDR::LONG_SIZE);
	cdr << ACE_Utils::truncate_cast<ACE_CDR::Long>(active);

	if( this->send_command_message(SmartACE::CMD_ACTIVATION_STATE, cdr.current()) != 0 ) {
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

PushServerAcceptor::PushServerAcceptor(void    *ptr,
      void (*callbackCnct)       (void *, const PushServerServiceHandler*,int,const ACE_Utils::UUID&),
      void (*callbackDiscrd)     (void *, const PushServerServiceHandler*),
      void (*callbackDiscon)     (void *, const PushServerServiceHandler*),
      void (*callbackSubscr)     (void *, const PushServerServiceHandler*,const int&,const int&),
      void (*callbackUnsubscr)   (void *, const PushServerServiceHandler*),
      void (*callbackServerInfo) (void *, const PushServerServiceHandler*),
      ACE_Reactor *reactor)
:  ACE_Acceptor<PushServerServiceHandler, ACE_SOCK_ACCEPTOR>(reactor)
,  _reactor(reactor)
{
   lthis = ptr;

   hndCnctPtr       = callbackCnct;
   hndDiscrdPtr     = callbackDiscrd;
   hndDisconPtr     = callbackDiscon;
   hndSubscrPtr     = callbackSubscr;
   hndUnsubscrPtr   = callbackUnsubscr;
   hndServerInfoPtr = callbackServerInfo;
}

PushServerAcceptor::~PushServerAcceptor()
{
	// no dynamic objects used...
}

int PushServerAcceptor::close_handler() {
	//ACE_DEBUG((LM_DEBUG, ACE_TEXT("PushServerAcceptor::close_handler()\n") ));
	_reactor->remove_handler(this, ACE_Event_Handler::READ_MASK);
	return 0;
}

} // END namespace SmartACE
//</alexej>
