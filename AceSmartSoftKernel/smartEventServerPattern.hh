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

#ifndef _SMARTEVENTSERVERPATTERN_H_
#define _SMARTEVENTSERVERPATTERN_H_

#include "smartErrno.hh"
#include "smartCmdNo.hh"

#include "smartEventPattern.hh"

#include "smartServiceHandler.hh"

namespace SmartACE {

   class EventServerAcceptor;

   /////////////////////////////////////////////////////////////////////////
   //
   // ServiceHandler part
   //
   /////////////////////////////////////////////////////////////////////////
   class EventServerServiceHandler
      :	public SmartACE::ServiceHandler,
      public SmartEventClientInterface	// SmartSendPattern interface for Client-Commands!
   {
   private:
      EventServerAcceptor *acceptor;

      int handle_incomming_message(ACE_CDR::Long command, ACE_InputCDR &cmd_is, ACE_InputCDR &msg_is);

   public:
      /** Default constructor and destructor
      */
      EventServerServiceHandler(ACE_Reactor *reactor=ACE_Reactor::instance());
      virtual ~EventServerServiceHandler();

      virtual int open(void *handler = 0);

      // D
      Smart::StatusCode event(const SmartACE::SmartMessageBlock *user, int id);

      // R3:
      Smart::StatusCode serverInitiatedDisconnect(int cid);
      // A0:
      Smart::StatusCode acknowledgmentConnect(int cid, int status);
      // A2:
      Smart::StatusCode acknowledgmentDisconnect();
      // A4:
      Smart::StatusCode acknowledgmentActivate(int status);
   };


   /////////////////////////////////////////////////////////////////////////
   //
   // Acceptor part
   //
   /////////////////////////////////////////////////////////////////////////
   class EventServerAcceptor
      :	public ACE_Acceptor<EventServerServiceHandler, ACE_SOCK_ACCEPTOR>
   {
   protected:
      /** Callback function pointers
      */
      void (*hndCnctPtr)(void *, const EventServerServiceHandler*,int,const ACE_Utils::UUID&);
      void (*hndDiscrdPtr)(void *, const EventServerServiceHandler*);
      void (*hndDisconPtr)(void *, const EventServerServiceHandler*);
      void (*hndActivatePtr)(void *, const EventServerServiceHandler*, const int&, const int&, const SmartMessageBlock *);
      void (*hndDeactivatePtr)(void *, const EventServerServiceHandler*, const int&);
      void *lthis;

      ACE_Reactor *_reactor;

   public:
      EventServerAcceptor(void *,
         void (*)(void *, const EventServerServiceHandler*,int,const ACE_Utils::UUID&), // connect handler
         void (*)(void *, const EventServerServiceHandler*),                      // discard handler
         void (*)(void *, const EventServerServiceHandler*),                      // disconnect handler
         void (*)(void *, const EventServerServiceHandler*, const int&, const int&, const SmartMessageBlock *), // activation handler
         void (*)(void *, const EventServerServiceHandler*, const int&),          // deactivation handler
         ACE_Reactor *reactor
      );

      virtual ~EventServerAcceptor();

      inline void handleConnect(const EventServerServiceHandler *sh, int cid, const ACE_Utils::UUID &uuid)
      {
    	  this->hndCnctPtr(lthis, sh, cid, uuid);
      }
      inline void handleDiscard(const EventServerServiceHandler *sh)
      {
    	  this->hndDiscrdPtr(lthis, sh);
      }
      inline void handleDisconnect(const EventServerServiceHandler *sh)
      {
    	  this->hndDisconPtr(lthis, sh);
      }
      inline void handleActivate(const EventServerServiceHandler *sh, const int &mode, const int &aid, const SmartMessageBlock *param)
      {
    	  this->hndActivatePtr(lthis, sh, mode, aid, param);
      }
      inline void handleDeactivate(const EventServerServiceHandler *sh, const int &aid)
      {
    	  this->hndDeactivatePtr(lthis, sh, aid);
      }

      int close_handler();
   };

} // end namespace SmartACE

#endif // _SMARTEVENTSERVERPATTERN_H_
//</alexej>

