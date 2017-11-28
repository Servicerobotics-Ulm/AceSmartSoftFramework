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

#ifndef _SMARTPUSHSERVERPATTERN_H_
#define _SMARTPUSHSERVERPATTERN_H_

#include "smartCmdNo.hh"

#include "smartPushPattern.hh"

#include "smartServiceHandler.hh"

namespace SmartACE {

	class PushServerAcceptor;

   /////////////////////////////////////////////////////////////////////////
   //
   // ServiceHandler part
   //
   /////////////////////////////////////////////////////////////////////////
   class PushServerServiceHandler
      :	public SmartACE::ServiceHandler,
      public SmartPushClientInterface	// SmartSendPattern interface for Client-Commands!
   {
   private:
      PushServerAcceptor *acceptor;

      int handle_incomming_message(ACE_CDR::Long command, ACE_InputCDR &cmd_is, ACE_InputCDR &msg_is);

   public:
      /** Default constructor and destructor
      */
      PushServerServiceHandler(ACE_Reactor *reactor=ACE_Reactor::instance());
      virtual ~PushServerServiceHandler();

      virtual int open(void *handler = 0);

      Smart::StatusCode update(SmartMessageBlock *, int sid);
      Smart::StatusCode serverInitiatedDisconnect(int cid);
      Smart::StatusCode acknowledgmentConnect(int cid, int status);
      Smart::StatusCode acknowledgmentDisconnect();

      Smart::StatusCode acknowledgmentSubscribe(int active);
      Smart::StatusCode serverInformation(unsigned long cycle, int active);
      Smart::StatusCode activationState(int active);
   };


   /////////////////////////////////////////////////////////////////////////
   //
   // Acceptor part
   //
   /////////////////////////////////////////////////////////////////////////
   class PushServerAcceptor
      :	public ACE_Acceptor<PushServerServiceHandler, ACE_SOCK_ACCEPTOR>
   {
   protected:
      /** Callback function pointers
      */
      void (*hndCnctPtr)(void *, const PushServerServiceHandler*,int,const ACE_Utils::UUID&);
      void (*hndDiscrdPtr)(void *, const PushServerServiceHandler*);
      void (*hndDisconPtr)(void *, const PushServerServiceHandler*);
      void (*hndSubscrPtr)(void *, const PushServerServiceHandler*,const int&,const int&);
      void (*hndUnsubscrPtr)(void *, const PushServerServiceHandler*);
      void (*hndServerInfoPtr)(void*, const PushServerServiceHandler*);
      void *lthis;

      ACE_Reactor *_reactor;

   public:
      PushServerAcceptor(void *,
         void (*)(void *, const PushServerServiceHandler*,int,const ACE_Utils::UUID&),	// connect handler
         void (*)(void *, const PushServerServiceHandler*),                       // discard handler
         void (*)(void *, const PushServerServiceHandler*),                       // disconnect handler
         void (*)(void *, const PushServerServiceHandler*,const int&,const int&), // subscribe handler
         void (*)(void *, const PushServerServiceHandler*),                       // unsunscribe handler
         void (*)(void *, const PushServerServiceHandler*),                       // get server info handler
         ACE_Reactor *reactor
      );

      virtual ~PushServerAcceptor();

      inline void handleConnect(const PushServerServiceHandler *sh, int cid, const ACE_Utils::UUID &uuid)
      {
    	  this->hndCnctPtr(lthis, sh, cid, uuid);
      }
      inline void handleDiscard(const PushServerServiceHandler *sh)
      {
    	  this->hndDiscrdPtr(lthis, sh);
      }
      inline void handleDisconnect(const PushServerServiceHandler *sh)
      {
    	  this->hndDisconPtr(lthis, sh);
      }
      inline void handleSubscribe(const PushServerServiceHandler *sh, const int &sid, const int &freq)
      {
    	  this->hndSubscrPtr(lthis, sh, sid, freq);
      }
      inline void handleUnsubscribe(const PushServerServiceHandler *sh)
      {
    	  this->hndUnsubscrPtr(lthis, sh);
      }
      inline void handleServerInformation(const PushServerServiceHandler *sh)
      {
    	  this->hndServerInfoPtr(lthis, sh);
      }

      int close_handler();
   };

} // end namespace SmartACE

#endif // _SMARTPUSHSERVERPATTERN_H_
//</alexej>

