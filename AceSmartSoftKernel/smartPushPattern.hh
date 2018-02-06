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

#ifndef _SMARTPUSHPATTERN_H_
#define _SMARTPUSHPATTERN_H_

#include <smartStatusCode.h>

#include "smartOSMapping.hh"

class SmartPushClientInterface
{
public:
   virtual ~SmartPushClientInterface() {  }

   //oneway void update(in any user);
   virtual Smart::StatusCode update(const SmartACE::SmartMessageBlock*, int sid) = 0;
   // push newest data to the client
   // user : user data type from the answer communication object

   virtual Smart::StatusCode serverInitiatedDisconnect(int cid) = 0;
   // clients are requested to disconnect themselves from the server
   // in case the server gets destroyed while there are still connected
   // clients

   virtual Smart::StatusCode acknowledgmentConnect(int cid, int status) = 0;

   virtual Smart::StatusCode acknowledgmentDisconnect() = 0;

   virtual Smart::StatusCode acknowledgmentSubscribe(int active) = 0;

   virtual Smart::StatusCode serverInformation(unsigned long cycleTimeMs, int active) = 0;

   //void serverStateChange(in long active);
   virtual Smart::StatusCode activationState(int active) = 0;
   // tell the client a state change at the server side stopped to started or vice versa
};

class SmartPushServerInterface
{
   public:
   virtual ~SmartPushServerInterface() {  }

   //void subscribe(in SmartPushClientPattern client,in long cycle,out long active);
	virtual Smart::StatusCode subscribe(int cycle, int sid) = 0;
   // subscribe the push service
   // client : reference to the client object
   // cycle  : whole-numbered multiple of the server cycle to specify the individual update cycle
   // active : 0/1 to indicate whether server is stopped / started

   //void unsubscribe(in SmartPushClientPattern client);
   virtual Smart::StatusCode unsubscribe() = 0;
   // unsubscribe the push service
   // client : reference to the client object

   //long connect(in SmartPushClientPattern client);
   virtual Smart::StatusCode connect(int cid, const ACE_Utils::UUID &serviceID) = 0;
   // add client to the list of clients at server
   //
   // client : reference to the client object
   //
   // status : indicates whether connect has been accepted
   //    SMART_OK                 : everything ok
   //    SMART_SERVICEUNAVAILABLE : server in destruction and therefore didn't accept new connect


   virtual Smart::StatusCode discard() = 0;


   //void disconnect(in SmartPushClientPattern client);
   virtual Smart::StatusCode disconnect() = 0;
   // remove client from list of clients at server
   //
   // client : reference to the client object

   //void getServerInfo(out double cycle,out long started);
   virtual Smart::StatusCode getServerInformation() = 0;
   // provide the server cycle time [seconds] to the client and the state whether the server is
   // started or stopped
};

#endif // _SMARTPUSHPATTERN_H_


