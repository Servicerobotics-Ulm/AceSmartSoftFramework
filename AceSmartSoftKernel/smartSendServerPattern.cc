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

//<alexej date="06.11.2008">
#include "smartSendServerPattern.hh"

namespace SmartACE
{

/////////////////////////////////////////////////////////////////////////
//
// ServiceHandler part
//
/////////////////////////////////////////////////////////////////////////

SendServerServiceHandler::SendServerServiceHandler(ACE_Reactor *reactor)
: SmartACE::ServiceHandler(reactor)
, acceptor(0)
{
	// nothing to do here so far, parent constructor of SmartServiceHandler is doing the work
}

SendServerServiceHandler::~SendServerServiceHandler()
{
	// all dynamic memmory objects are handled by parent class
	// so no memmory clean up is necessary here
}

int SendServerServiceHandler::open(void *handler)
{
	if (handler != 0) {
		acceptor = static_cast<SendServerAcceptor*>(handler);
	}
	return SmartACE::ServiceHandler::open(handler);
}

int SendServerServiceHandler::handle_incomming_message(ACE_CDR::Long command, ACE_InputCDR &cmd_is, ACE_InputCDR &msg_is)
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

   //
   // command - switch case
   //
	switch(command) {

	case SmartACE::CMD_COMMAND:
//ACE_OS::printf("server->handle_incomming_message->CMD_COMMAND\n");
      if(msg_is.length() > 0)
      {
         acceptor->handleCommand(msg_is.start());
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

Smart::StatusCode SendServerServiceHandler::acknowledgmentConnect(int cid, int status)
{
	Smart::StatusCode result = Smart::SMART_OK;

	ACE_OutputCDR cdr(ACE_CDR::LONG_SIZE * 2);
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

Smart::StatusCode SendServerServiceHandler::serverInitiatedDisconnect(int cid)
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


Smart::StatusCode SendServerServiceHandler::acknowledgmentDisconnect()
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


/////////////////////////////////////////////////////////////////////////
//
// acceptor part
//
/////////////////////////////////////////////////////////////////////////

SendServerAcceptor::SendServerAcceptor
   (
      void *ptr,
      void (*callbackCmd)   (void *, const ACE_Message_Block*),
      void (*callbackCnct)  (void *, const SendServerServiceHandler*,int,const ACE_Utils::UUID&),
      void (*callbackDiscrd)(void *, const SendServerServiceHandler*),
      void (*callbackDiscon)(void *, const SendServerServiceHandler*),
      ACE_Reactor *reactor)
:  ACE_Acceptor<SendServerServiceHandler, ACE_SOCK_ACCEPTOR>(reactor)
,  _reactor(reactor)
{
   lthis        = ptr;

   hndCmdPtr    = callbackCmd;
   hndCnctPtr   = callbackCnct;
   hndDiscrdPtr = callbackDiscrd;
   hndDisconPtr = callbackDiscon;
}

SendServerAcceptor::~SendServerAcceptor()
{
	// no dynamic objects used...
}

int SendServerAcceptor::close_handler() {
	//ACE_DEBUG((LM_DEBUG, ACE_TEXT("SendServerAcceptor::close_handler()\n") ));
	_reactor->remove_handler(this, ACE_Event_Handler::READ_MASK);
	return 0;
}

} // END namespace SmartACE

//</alexej>
