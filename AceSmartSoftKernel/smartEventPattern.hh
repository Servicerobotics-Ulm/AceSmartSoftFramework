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

#ifndef _SMARTEVENTPATTERN_H_
#define _SMARTEVENTPATTERN_H_

#include "smartOSMapping.hh"
#include "smartErrno.hh"


class SmartEventClientInterface
{
public:
   virtual ~SmartEventClientInterface() { }

  //void event(in any user,in long id);
   // D:
   virtual Smart::StatusCode event(SmartACE::SmartMessageBlock *user, int id) = 0;
  // event fires and therefore transmit event object to client
  // user : user event data
  // id   : activation id from client
  //
  // "oneway" is used to decouple event distribution from executing
  // an event handler at the client side. With "oneway" several
  // events can fire very fast without waiting until the handler
  // of the current firing has been finished at the client side.
  // Also works without "oneway" due to the carefully implemented
  // locking mechanisms.
  // ("oneway" removed since TAO currently shows critical behavior ...)

   // R3:
   virtual Smart::StatusCode serverInitiatedDisconnect(int cid) = 0;
  // clients are requested to disconnect themselves from the server
  // in case the server gets destroyed while there are still connected
  // clients

   // A0:
   virtual Smart::StatusCode acknowledgmentConnect(int cid, int status) = 0;

   // A2:
   virtual Smart::StatusCode acknowledgmentDisconnect() = 0;

   // A4:
   virtual Smart::StatusCode acknowledgmentActivate(int status) = 0;
};

class SmartEventServerInterface
{
public:
   virtual ~SmartEventServerInterface() { }

  //void activate(in SmartEventClientPattern client,in long mode,in long id,in any parameter);
  // R4:
  virtual Smart::StatusCode activate(int mode, int aid, const SmartACE::SmartMessageBlock *parameter) = 0;
  // activate an event
  // client    : reference to the client object
  // mode      : activation mode single or continuous
  // id        : activation id from the client
  // parameter : event activation parameters

  //void deactivate(in SmartEventClientPattern client,in long id);
  // R5:
  virtual Smart::StatusCode deactivate(int aid) = 0;
  // deactivate an event
  // client : reference to the client object
  // id     : activation id from the client

  //long connect(in SmartEventClientPattern client);
  // R0:
  virtual Smart::StatusCode connect(int cid, const ACE_Utils::UUID &serviceID) = 0;
  // add client to list of clients at server
  //
  // client : reference to the client object
  //
  // status : indicates whether connect has been accepted
  //    SMART_OK                 : everything ok
  //    SMART_SERVICEUNAVAILABLE : server in destruction and therefore didn't accept new connect

  //void disconnect(in SmartEventClientPattern client);
  // R2:
  virtual Smart::StatusCode disconnect() = 0;
  // remove client from list of clients at server
  //
  // client : reference to the client object

  // R1:
  virtual Smart::StatusCode discard() = 0;
};

#endif // _SMARTEVENTPATTERN_H_
