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

#ifndef SMARTSENDPATTERN_HH_
#define SMARTSENDPATTERN_HH_

#include "smartOSMapping.hh"
#include "smartErrno.hh"

namespace SmartACE {

class SmartSendClientInterface
{
  public:
   virtual ~SmartSendClientInterface() {  }

  virtual Smart::StatusCode serverInitiatedDisconnect(int cid) = 0;
  // clients are requested to disconnect themselves from the server
  // in case the server gets destroyed while there are still connected
  // clients

  virtual Smart::StatusCode acknowledgmentConnect(int cid, int status) = 0;

  virtual Smart::StatusCode acknowledgmentDisconnect() = 0;
};

//forward declaration
//class SendServerServiceHandler;

class SmartSendServerInterface
{
  public:
   virtual ~SmartSendServerInterface() {  }

  virtual Smart::StatusCode command(const SmartACE::SmartMessageBlock *message) = 0;
  // message   : user data type from the request communication object
  //
  // "oneway" is needed to finish the send command before the
  // server handler finishes executing the request.
  // Also works without "oneway" due to the carefully implemented
  // locking mechanisms.

  //virtual Smart::StatusCode connect(int cid) = 0;
  virtual Smart::StatusCode connect(int cid, const ACE_Utils::UUID &serviceID) = 0;
  // add client to list of clients at server
  //
  // client : reference to the client object
  //
  // status : indicates whether connect has been accepted
  //    SMART_OK                 : everything ok
  //    SMART_SERVICEUNAVAILABLE : server in destruction and therefore didn't accept new connect

  virtual Smart::StatusCode discard() = 0;

  //virtual void disconnect(SendServerServiceHandler *client) = 0;
  virtual Smart::StatusCode disconnect() = 0;
  // remove client from list of clients
  //
  // client : reference to the client object
};

} // END namespace SmartACE

#endif /*SMARTSENDPATTERN_HH_*/
