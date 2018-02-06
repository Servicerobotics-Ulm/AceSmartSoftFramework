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

#ifndef _SMARTQUERYSERVERPATTERN_H_
#define _SMARTQUERYSERVERPATTERN_H_

#include "smartErrno.hh"
#include "smartCmdNo.hh"

#include "smartQueryPattern.hh"

#include "smartServiceHandler.hh"

namespace SmartACE {

	class QueryServerAcceptor;

   /////////////////////////////////////////////////////////////////////////
   //
   // ServiceHandler part
   //
   /////////////////////////////////////////////////////////////////////////

   class QueryServerServiceHandler
      :	public SmartACE::ServiceHandler,
      public SmartQueryClientInterface	// SmartSendPattern interface for Client-Commands!
   {
   private:
      QueryServerAcceptor *acceptor;

      int handle_incomming_message(ACE_CDR::Long command, ACE_InputCDR &cmd_is, ACE_InputCDR &msg_is);

   public:
      /** Default constructor and destructor
      */
      QueryServerServiceHandler(ACE_Reactor *reactor=ACE_Reactor::instance());
      virtual ~QueryServerServiceHandler();

      virtual int open(void *handler = 0);

      Smart::StatusCode answer(const SmartMessageBlock *any, int qid);
      Smart::StatusCode acknowledgmentConnect(int cid, int status);
      Smart::StatusCode serverInitiatedDisconnect(int cid);
      Smart::StatusCode acknowledgmentDisconnect();
   };


   /////////////////////////////////////////////////////////////////////////
   //
   // Acceptor part
   //
   /////////////////////////////////////////////////////////////////////////
   class QueryServerAcceptor
      :	public ACE_Acceptor<QueryServerServiceHandler, ACE_SOCK_ACCEPTOR>
   {
   protected:
      /** Callback function pointers
      */
      void (*hndRqstPtr)(void *, const SmartACE::SmartMessageBlock *, const QueryServerServiceHandler*, int);

      void (*hndCnctPtr)(void *, const QueryServerServiceHandler*,int,const ACE_Utils::UUID&);
      void (*hndDiscrdPtr)(void *, const QueryServerServiceHandler*);
      void (*hndDisconPtr)(void *, const QueryServerServiceHandler*);
      void *lthis;

      ACE_Reactor *_reactor;

   public:
      QueryServerAcceptor(void *,
         void (*)(void *, const SmartACE::SmartMessageBlock*,const QueryServerServiceHandler*,int),
         void (*)(void *, const QueryServerServiceHandler*,int,const ACE_Utils::UUID&),	// connect handler
         void (*)(void *, const QueryServerServiceHandler*),                        // discard handler
         void (*)(void *, const QueryServerServiceHandler*),                        // disconnect handler
         ACE_Reactor *reactor
      );

      virtual ~QueryServerAcceptor();

      inline void handleRequest(const SmartACE::SmartMessageBlock *message, const QueryServerServiceHandler *sh, int qid)
      {
    	  this->hndRqstPtr(lthis, message, sh, qid);
      }
      inline void handleConnect(const QueryServerServiceHandler *sh, int cid, const ACE_Utils::UUID &uuid)
      {
    	  this->hndCnctPtr(lthis, sh, cid, uuid);
      }
      inline void handleDiscard(const QueryServerServiceHandler *sh)
      {
    	  this->hndDiscrdPtr(lthis, sh);
      }
      inline void handleDisconnect(const QueryServerServiceHandler *sh)
      {
    	  this->hndDisconPtr(lthis, sh);
      }

      int close_handler();
   };

} // end namespace SmartACE

#endif // _SMARTQUERYSERVERPATTERN_H_
//</alexej>

