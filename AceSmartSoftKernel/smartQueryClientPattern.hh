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

#ifndef _SMARTQUERYCLIENTPATTERN_H_
#define _SMARTQUERYCLIENTPATTERN_H_

#include "smartErrno.hh"
#include "smartCmdNo.hh"

#include "smartQueryPattern.hh"

#include "smartServiceHandler.hh"

namespace SmartACE
{

   /////////////////////////////////////////////////////////////////////////
   //
   // ServiceHandler part
   //
   /////////////////////////////////////////////////////////////////////////
   class QueryClientServiceHandler
      :	public SmartACE::ServiceHandler,
      public SmartQueryServerInterface
   {
   private:
      void (*hndAnsw)(void *,const SmartACE::SmartMessageBlock *, int);
      void (*hndAckConn)(void *, int, int);
      void (*hndServDisc)(void *, int);
      void (*hndAckDisc)(void *);
      void *lthis;

      int handle_incomming_message(ACE_CDR::Long command, ACE_InputCDR &cmd_is, ACE_InputCDR &msg_is);

   public:
      QueryClientServiceHandler(ACE_Reactor *reactor=ACE_Reactor::instance());
      virtual ~QueryClientServiceHandler();

      void setCallbackFkts(void *,
         void (*)(void *, const SmartACE::SmartMessageBlock*, int),
         void (*)(void *, int, int),
         void (*)(void *, int),
         void (*)(void *));

      Smart::StatusCode request(const SmartACE::SmartMessageBlock *message, int id);

      Smart::StatusCode connect(int cid, const ACE_Utils::UUID &serviceID);
      Smart::StatusCode discard();
      Smart::StatusCode disconnect();
   };

   /////////////////////////////////////////////////////////////////////////
   //
   // Connector part
   //
   /////////////////////////////////////////////////////////////////////////

   class QueryClientConnector
      :	public ACE_Connector<QueryClientServiceHandler, ACE_SOCK_CONNECTOR>
   {
   public:
         QueryClientConnector(ACE_Reactor *reactor)
         :  ACE_Connector<QueryClientServiceHandler, ACE_SOCK_CONNECTOR>(reactor)
         {   }
   };

} // END namespace SmartACE

#endif // _SMARTQUERYCLIENTPATTERN_H_
//</alexej>

