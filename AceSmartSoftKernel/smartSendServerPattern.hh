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

#ifndef SMARTSENDSERVERPATTERN_HH_
#define SMARTSENDSERVERPATTERN_HH_

#include "smartErrno.hh"
#include "smartCmdNo.hh"

#include "smartSendPattern.hh"

#include "smartServiceHandler.hh"

namespace SmartACE {

	class SendServerAcceptor;

   /////////////////////////////////////////////////////////////////////////
   //
   // ServiceHandler part
   //
   /////////////////////////////////////////////////////////////////////////
   class SendServerServiceHandler
      :	public SmartACE::ServiceHandler,
      public SmartSendClientInterface	// SmartSendPattern interface for Client-Commands!
   {
   private:
      SendServerAcceptor *acceptor;

      int handle_incomming_message(ACE_CDR::Long command, ACE_InputCDR &cmd_is, ACE_InputCDR &msg_is);

   public:
      /** Default constructor and destructor
      */
      SendServerServiceHandler(ACE_Reactor *reactor=ACE_Reactor::instance());
      virtual ~SendServerServiceHandler();

      virtual int open(void *handler = 0);

      Smart::StatusCode acknowledgmentConnect(int cid, int status);
      Smart::StatusCode serverInitiatedDisconnect(int cid);
      Smart::StatusCode acknowledgmentDisconnect();
   };


   /////////////////////////////////////////////////////////////////////////
   //
   // Acceptor part
   //
   /////////////////////////////////////////////////////////////////////////
   class SendServerAcceptor
      :	public ACE_Acceptor<SendServerServiceHandler, ACE_SOCK_ACCEPTOR>
   {
   protected:
      /** Callback function pointers
      */
      void (*hndCmdPtr)(void *, const ACE_Message_Block *);
      void (*hndCnctPtr)(void *, const SendServerServiceHandler*,int,const ACE_Utils::UUID&);
      void (*hndDiscrdPtr)(void *, const SendServerServiceHandler*);
      void (*hndDisconPtr)(void *, const SendServerServiceHandler*);
      void *lthis;

      ACE_Reactor *_reactor;

   public:
      SendServerAcceptor(void *,
         void (*)(void *, const ACE_Message_Block *),                             // command handler
         void (*)(void *, const SendServerServiceHandler*,int,const ACE_Utils::UUID&),	// connect handler
         void (*)(void *, const SendServerServiceHandler*),                      // discard handler
         void (*)(void *, const SendServerServiceHandler*),                      // disconnect handler
         ACE_Reactor *reactor
      );

      virtual ~SendServerAcceptor();

      inline void handleCommand(const SmartACE::SmartMessageBlock *message)
      {
    	  this->hndCmdPtr(lthis, message);
      }
      inline void handleConnect(const SendServerServiceHandler *sh, int cid, const ACE_Utils::UUID &uuid)
      {
    	  this->hndCnctPtr(lthis, sh, cid, uuid);
      }
      inline void handleDiscard(const SendServerServiceHandler *sh)
      {
    	  this->hndDiscrdPtr(lthis, sh);
      }
      inline void handleDisconnect(const SendServerServiceHandler *sh)
      {
    	  this->hndDisconPtr(lthis, sh);
      }

      int close_handler();
   };

} // END namespace SmartACE

#endif // SMARTSENDSERVERPATTERN_HH_
//</alexej>
