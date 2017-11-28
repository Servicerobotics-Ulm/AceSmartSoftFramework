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

#include "smartPushClientPattern.hh"

namespace SmartACE
{

   /////////////////////////////////////////////////////////////////////////
   //
   // ServiceHandler part
   //
   /////////////////////////////////////////////////////////////////////////

   PushClientServiceHandler::PushClientServiceHandler(ACE_Reactor *reactor)
   :  SmartACE::ServiceHandler(reactor)
   , hndUpdate(0)
   , hndAckConn(0)
   , hndServDisc(0)
   , hndAckDisc(0)
   , hndAckSubscr(0)
   , hndServerInfo(0)
   , hndAcktivState(0)
   , lthis(0)
   {
      // nothing to do here so far, parent constructor of SmartServiceHandler is doing the work
   }

   PushClientServiceHandler::~PushClientServiceHandler()
   {
      // all dynamic memory objects are handled by parent class
      // so no memory clean up is necessary here
   }

   void PushClientServiceHandler::setCallbackFkts(void *ptr,
      void (*callbackUpdate)(void *, const SmartACE::SmartMessageBlock*,int),
      void (*callbackAckConn)(void *, int, int),
      void (*callbackSrvDisc)(void *, int),
      void (*callbackAckDisc)(void *),
      void (*callbackAckSubscribe)(void*,int),
      void (*callbackServerInfo)(void*,unsigned long,int),
      void (*callbackActivationState)(void *,int&))
   {
      lthis = ptr;

      hndUpdate      = callbackUpdate;
      hndAckConn     = callbackAckConn;
      hndServDisc    = callbackSrvDisc;
      hndAckDisc     = callbackAckDisc;
      hndAckSubscr   = callbackAckSubscribe;
      hndServerInfo  = callbackServerInfo;
      hndAcktivState = callbackActivationState;
   }

   int PushClientServiceHandler::handle_incomming_message(ACE_CDR::Long command, ACE_InputCDR &cmd_is, ACE_InputCDR &msg_is)
   {
      //<alexej date="2010-03-09">
      ACE_CDR::Long cid = 0;
      ACE_CDR::Long i_temp = 0;
      //</alexej>

      //double cycle;
      ACE_CDR::ULongLong ulong_cycle = 0;

      //
      // command - switch case
      //
      switch(command) {

   case SmartACE::CMD_UPDATE:
      if(cmd_is.length() > 0 && msg_is.length() > 0)
      {
         // get session id
         cmd_is >> i_temp;

         //printf("client->handle_incomming_message->CMD_UPDATE\n");
         this->hndUpdate(lthis, msg_is.start(), i_temp);
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


   case SmartACE::CMD_ACK_SUBSCRIBE:
      if(cmd_is.length() > 0)
      {
         // get activation state
         cmd_is >> i_temp;
         //printf("client->handle_incomming_message->CMD_ACK_SUBSCRIBE\n");
         this->hndAckSubscr(lthis, i_temp);
      }
      break;

   case SmartACE::CMD_SERVER_INFO:
      if(cmd_is.length() > 0)
      {
         cmd_is >> ulong_cycle;
         cmd_is >> i_temp;

         //printf("client->handle_incomming_message->CMD_SERVER_INFO\n");
         this->hndServerInfo(lthis, ulong_cycle, i_temp);
      }
      break;

   case SmartACE::CMD_ACTIVATION_STATE:
      if(cmd_is.length() > 0)
      {
         cmd_is >> i_temp;

         //printf("client->handle_incomming_message->CMD_ACTIVATION_STATE\n");
         this->hndAcktivState(lthis, i_temp);
      }
      break;
      };

      return 0;
   }

   Smart::StatusCode PushClientServiceHandler::subscribe(int cycle, int sid)
   {
      Smart::StatusCode result = Smart::SMART_OK;

      size_t length = ACE_CDR::LONG_SIZE+ACE_CDR::LONG_SIZE;

      ACE_OutputCDR cdr(length);
      cdr << ACE_CDR::Long(ACE_Utils::truncate_cast<ACE_CDR::Long>(cycle));;
      cdr << ACE_CDR::Long(ACE_Utils::truncate_cast<ACE_CDR::Long>(sid));

      if( this->send_command_message(SmartACE::CMD_SUBSCRIBE, cdr.current()) != 0 ) {
        if(this->is_disconnected()) {
            result = Smart::SMART_DISCONNECTED;
         }else{
            result = Smart::SMART_ERROR_COMMUNICATION;
         }
      }

      return result;
   }

   Smart::StatusCode PushClientServiceHandler::unsubscribe()
   {
      Smart::StatusCode result = Smart::SMART_OK;

      if( this->send_command_message(SmartACE::CMD_UNSUBSCRIBE) != 0 ) {
         if(this->is_disconnected()) {
            result = Smart::SMART_DISCONNECTED;
         }else{
            result = Smart::SMART_ERROR_COMMUNICATION;
         }
      }

      return result;
   }

   Smart::StatusCode PushClientServiceHandler::connect(int cid, const ACE_Utils::UUID &serviceID)
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

   Smart::StatusCode PushClientServiceHandler::discard()
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

   Smart::StatusCode PushClientServiceHandler::disconnect()
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

   Smart::StatusCode PushClientServiceHandler::getServerInformation()
   {
      Smart::StatusCode result = Smart::SMART_OK;

      if( this->send_command_message(SmartACE::CMD_GET_SERVER_INFO) != 0 ) {
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

