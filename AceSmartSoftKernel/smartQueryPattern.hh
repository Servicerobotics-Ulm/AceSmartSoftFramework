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

// - those interfaces are used internally in the communication
//   patterns and are not visible to the user
// - each communication pattern like query / send etc. has such
//   an individual interface describing the interaction between
//   the client and the server part of the pattern at the system
//   level
// - Extending a pattern might result in additional methods at
//   this system level
// - Implementing the communication patterns on top of a message
//   based system would require to replace the methods by messages
//   and provide appropriate message dispatching mechanisms. In
//   CORBA this is already available and not visible to the
//   developer.

#ifndef _SMARTQUERYPATTERN_H_
#define _SMARTQUERYPATTERN_H_

#include "smartOSMapping.hh"
#include "smartErrno.hh"

namespace SmartACE {

class SmartQueryClientInterface
{
  public:
   virtual ~SmartQueryClientInterface() {  }

  //oneway void answer(in any user,in long id);
  virtual Smart::StatusCode answer(SmartACE::SmartMessageBlock*, int) = 0;
  // user : user data type from the answer communication object
  // id   : returned query identifier previously provided in the request

  //void serverInitiatedDisconnect();
  virtual Smart::StatusCode serverInitiatedDisconnect(int cid) = 0;
  // clients are requested to disconnect themselves from the server
  // in case the server gets destroyed while there are still connected
  // clients

  virtual Smart::StatusCode acknowledgmentConnect(int cid, int status) = 0;

  //<alexej date="26.11.2008">
  virtual Smart::StatusCode acknowledgmentDisconnect() = 0;
  //</alexej>
};

class SmartQueryServerInterface
{
  public:
   virtual ~SmartQueryServerInterface() {  }

  //oneway void request(in any user,in SmartQueryClientPattern client, in long id);
  virtual Smart::StatusCode request(SmartACE::SmartMessageBlock*, int id) = 0;
  // user   : user data type from the request communication object
  // client : reference to the client object
  // id     : query identifier needed by client to distribute answer
  //          correct pending query since queries can be submitted
  //          interleaved
  //
  // - "oneway" attribute is needed to decouple the execution of the
  //   query server handler from the request invocation since otherwise
  //   the queryRequest would return ONLY AFTER the complete handler
  //   has been executed. queryReceiveWait could then be called only
  //   after having already received the answer, since the handler
  //   calls the answer-method before returning to the client side
  //   to finish the queryRequest call.
  //   The implementation however works also without "oneway" but is
  //   then not as efficient as possible. The internal structures and
  //   locks of critical sections in the client can cope with the
  //   situation when a request provides the answer while the request
  //   is still in progress.

  //long connect(in SmartQueryClientPattern client);
  //int connect(SmartQueryClientPattern client) = 0;
  virtual Smart::StatusCode connect(int cid, const ACE_Utils::UUID &serviceID) = 0;
  // add client to list of clients at server
  //
  // client : reference to the client object
  //
  // status : indicates whether connect has been accepted
  //    SMART_OK                 : everything OK
  //    SMART_SERVICEUNAVAILABLE : server in destruction and therefore didn't accept new connect

  virtual Smart::StatusCode discard() = 0;

  //void disconnect(in SmartQueryClientPattern client);
  virtual Smart::StatusCode disconnect() = 0;
  // mark pending query at server as disconnected and remove client
  // from list of clients of server
  //
  // client : reference to the client object
};

} // end namespace SmartACE

#endif // _SMARTQUERYPATTERN_H_
