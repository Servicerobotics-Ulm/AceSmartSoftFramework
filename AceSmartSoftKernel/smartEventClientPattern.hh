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

//<alexej date="09.10.2008">
#ifndef _SMARTEVENTCLIENTPATTERN_H_
#define _SMARTEVENTCLIENTPATTERN_H_

#include "smartErrno.hh"
#include "smartCmdNo.hh"

#include "smartEventPattern.hh"

#include "smartServiceHandler.hh"

namespace SmartACE
{

   /////////////////////////////////////////////////////////////////////////
   //
   // ServiceHandler part
   //
   /////////////////////////////////////////////////////////////////////////
   class EventClientServiceHandler
      :	public SmartACE::ServiceHandler,
      public SmartEventServerInterface
   {
   private:
      void (*hndEvent)(void*, const SmartMessageBlock *,size_t);
      void (*hndAckConn)(void*, int cid, int status);
      void (*hndServDisc)(void*,int cid);
      void (*hndAckDisc)(void*);
      void (*hndAckActivate)(void*,const size_t&);

      void *lthis;

      int handle_incomming_message(ACE_CDR::Long command, ACE_InputCDR &cmd_is, ACE_InputCDR &msg_is);

   public:
      EventClientServiceHandler(ACE_Reactor *reactor=ACE_Reactor::instance());
      virtual ~EventClientServiceHandler();

      void setCallbackFkts(void *,
         void (*)(void *, const SmartACE::SmartMessageBlock*,size_t),  // event handler
         void (*)(void *, int, int),                           // acknowledge connect handler
         void (*)(void *, int),                                // server initiated disconnect handler
         void (*)(void *),                                     // acknowledge disconnect handler
         void (*)(void*,const size_t&));                          // acknowledgement activate handler

      // R4
      Smart::StatusCode activate(int mode, size_t aid, const SmartACE::SmartMessageBlock *parameter);
      // R5
      Smart::StatusCode deactivate(size_t aid);

      // R0
      Smart::StatusCode connect(int cid, const ACE_Utils::UUID &serviceID);
      // R1
      Smart::StatusCode discard();
      // R2
      Smart::StatusCode disconnect();
   };

   /////////////////////////////////////////////////////////////////////////
   //
   // Connector part
   //
   /////////////////////////////////////////////////////////////////////////

   class EventClientConnector
      :	public ACE_Connector<EventClientServiceHandler, ACE_SOCK_CONNECTOR>
   {
   public:
         EventClientConnector(ACE_Reactor *reactor)
         :  ACE_Connector<EventClientServiceHandler, ACE_SOCK_CONNECTOR>(reactor)
         {   }
   };


} // END namespace SmartACE

#endif // _SMARTEVENTCLIENTPATTERN_H_
//</alexej>



