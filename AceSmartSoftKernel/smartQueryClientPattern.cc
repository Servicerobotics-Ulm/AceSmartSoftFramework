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

#include "smartQueryClientPattern.hh"

namespace SmartACE
{

/////////////////////////////////////////////////////////////////////////
//
// ServiceHandler part
//
/////////////////////////////////////////////////////////////////////////

QueryClientServiceHandler::QueryClientServiceHandler(ACE_Reactor *reactor)
:  SmartACE::ServiceHandler(reactor)
, hndAnsw(0)
, hndAckConn(0)
, hndServDisc(0)
, hndAckDisc(0)
, lthis(0)
{
	// nothing to do here so far, parent constructor of SmartServiceHandler is doing the work
}

QueryClientServiceHandler::~QueryClientServiceHandler()
{
	// all dynamic memmory objects are handled by parent class
	// so no memmory clean up is necessary here
}

void QueryClientServiceHandler::setCallbackFkts(void *ptr,
                     void (*callbackAnswer)(void *, const SmartACE::SmartMessageBlock*,int),
                     void (*callbackAckConn)(void *, int, int),
                     void (*callbackSrvDisc)(void *, int),
                     void (*callbackAckDisc)(void *))
{
	lthis = ptr;

	hndAnsw = callbackAnswer;
	hndAckConn = callbackAckConn;
	hndServDisc = callbackSrvDisc;
	hndAckDisc = callbackAckDisc;
}

int QueryClientServiceHandler::handle_incomming_message(ACE_CDR::Long command, ACE_InputCDR &cmd_is, ACE_InputCDR &msg_is)
{
   //<alexej date="2010-03-09">
   ACE_CDR::Long cid = 0;
   ACE_CDR::Long i_temp = 0;
   //</alexej>

   //
   // command - switch case
   //
	switch(command) {

	case SmartACE::CMD_ANSWER:
      if(cmd_is.length() > 0 && msg_is.length() > 0)
      {
         // get query id
		   cmd_is >> i_temp;

//printf("client->handle_incomming_message->CMD_ANSWER\n");
         this->hndAnsw(lthis, msg_is.start(), i_temp);
      }
		break;

   case SmartACE::CMD_ACK_CONNECT:
      if(cmd_is.length() > 0)
      {
//ACE_OS::printf("client->handle_incomming_message->CMD_ACK_CONNECT\n");

         //<alexej date="2010-03-09">
         //read connection-id
         cmd_is >> cid;

         // read the status value
         cmd_is >> i_temp;

         this->hndAckConn(lthis, cid, i_temp);
         //</alexej>
      }
      break;

	case SmartACE::CMD_ACK_DISCONNECT:
//printf("client->handle_incomming_message->CMD_ACK_DISCONNECT\n");
		this->hndAckDisc(lthis);
		break;

	case SmartACE::CMD_SERVER_INITIATED_DISCONNECT:
      if(cmd_is.length() > 0)
      {
//printf("client->handle_incomming_message->CMD_SERVER_INITIATED_DISCONNECT\n");

         //<alexej date="2010-03-09">
         //read connection-id
         cmd_is >> cid;

         this->hndServDisc(lthis, cid);
         //</alexej>
      }
		break;

	};

	return 0;
}

Smart::StatusCode QueryClientServiceHandler::request(const SmartACE::SmartMessageBlock *message, int id)
{
	Smart::StatusCode result = Smart::SMART_OK;

	size_t length = 4 + message->size();

	ACE_OutputCDR cdr(length);
	cdr << ACE_Utils::truncate_cast<ACE_CDR::Long>(id);

	if ( this->send_command_message(SmartACE::CMD_REQUEST, cdr.current(), message) != 0 ) {
		if(this->is_disconnected()) {
			result = Smart::SMART_DISCONNECTED;
		}else{
			result = Smart::SMART_ERROR_COMMUNICATION;
		}
	}

	return result;
}

Smart::StatusCode QueryClientServiceHandler::connect(int cid, const ACE_Utils::UUID &serviceID)
{
	Smart::StatusCode result = Smart::SMART_OK;

   const ACE_CString *uuid_str = serviceID.to_string();

   ACE_OutputCDR cdr(ACE_CDR::DEFAULT_BUFSIZE + uuid_str->length());

   // write connection-id
   cdr << ACE_Utils::truncate_cast<ACE_CDR::Long>(cid);
   cdr << *uuid_str;

   if ( this->send_command_message(SmartACE::CMD_CONNECT, cdr.current()) != 0 ) {
		if(this->is_disconnected()) {
			result = Smart::SMART_DISCONNECTED;
		}else{
			result = Smart::SMART_ERROR_COMMUNICATION;
		}
   }

	return result;
}

Smart::StatusCode QueryClientServiceHandler::discard()
{
	Smart::StatusCode result = Smart::SMART_OK;

   if( this->send_command_message(SmartACE::CMD_DISCARD) != 0 ) {
		if(this->is_disconnected()) {
			result = Smart::SMART_DISCONNECTED;
		}else{
			result = Smart::SMART_ERROR_COMMUNICATION;
		}
	}

	return result;
}

Smart::StatusCode QueryClientServiceHandler::disconnect()
{
	Smart::StatusCode result = Smart::SMART_OK;

	if( this->send_command_message(SmartACE::CMD_DISCONNECT) != 0 ) {
		if(this->is_disconnected()) {
			result = Smart::SMART_DISCONNECTED;
		}else{
			result = Smart::SMART_ERROR_COMMUNICATION;
		}
	}

	return result;
}

} // END namespace SmartACE

//</alexej>

