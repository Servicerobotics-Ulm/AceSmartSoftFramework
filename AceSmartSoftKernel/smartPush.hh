// --------------------------------------------------------------------------
//
//  Copyright (C) 2015 Alex Lotz
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

#ifndef _SMARTPUSH_HH
#define _SMARTPUSH_HH

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <string>
#include <vector>

#include <smartIPushClientPattern_T.h>
#include <smartIPushServerPattern_T.h>

#include "smartErrno.hh"
#include "smartComponent.hh"
#include "smartPushServerPattern.hh"
#include "smartPushClientPattern.hh"

//
// needed for the add() / remove() interface for wiring
//
#include "smartCommWiring.hh"

namespace SmartACE {
  class WiringSlave;
}

/////////////////////////////////////////////////////////////////////////
//
// client part
//
/////////////////////////////////////////////////////////////////////////


//
// client push timed template
//
namespace SmartACE {
  /** Client part of the Push pattern to provide a flexible push
   *  service. Clients can subscribe to regularly get every n-th
   *  update. This class implements the Subject part of the
   *  Observer design pattern (see also PushClientObserver class).
   *
   *  Template parameters
   *    - <b>T</b>: Pushed value class (Communication Object)
   *
   *  Demonstrated in <a href="/drupal/?q=node/51#eigth-example">eigth example</a>
   */
  template<class DataType> class PushClient : public Smart::IPushClientPattern<DataType> {
  private:
    DataType             data;
    SmartCVwithoutMemory cond;

    /// 0/1 to indicate whether server is stopped / started
    int statusServer;

    /// 0/1 to indicate whether client is not subscribed / is subscribed to server
    int statusSubscribed;

    /// 0/1 to indicate whether data is not available / is available
    int dataAvailable;

    /// mutexConnection protects critical sections from being executed in
    ///                 parallel. These are all related to changing connections
    ///                 while client/server interactions are active. The mutex
    ///                 avoids racing conditions when for example a subscribe
    ///                 and a disconnect are called in parallel.
    SmartRecursiveMutex mutexConnection;

    //<alexej date="2010-03-18">
    /// Administrative monitor for handling connects
    SmartAdministrativeMonitor monitorConnect;

    // Administrative Monitor for handling disconnects
    SmartAdministrativeMonitor monitorDisconnect;

    /// Monitor for "server info" request-reply procedure
    SmartAdministrativeMonitor monitorServerInfo;

    /// Monitor for subscription acknowledgment
    SmartAdministrativeMonitor monitorSubscription;
    //</alexej>

    /// management class of the component
    SmartComponent *component;

    //<alexej date="09.10.2008">
    /*
    /// the CORBA client object
    PushClient_impl *corbaClient;

    /// the CORBA client reference transferred to server
    SmartPushClientPattern_ptr h;

    /// the CORBA server connection
    SmartPushServerPattern_var corbaServant;
    */
    /// ACE_Connector implementation for SendClients connector feature
    PushClientConnector *connector;

    /// ServiceHandler handles all low level communication with server side
    PushClientServiceHandler *serverProxy;

    /// Session ID is used to distinguish various subscription sessions
    int session_id;
    //</alexej>

    //<alexej date="2010-03-18" description="connection id changed from UUID to int">
    /// connection-id is used to recognize invalid connection (see PhD of Schlegel on page 158)
    int connectionID;
    //</alexej>

    /// 0/1 to indicate whether client is not connected / is connected to server
    int statusConnected;

    /// 0/1 to indicate whether port is not contained / is contained in the set
    /// of externally accessible client wiring
    int statusManagedPort;

    //<alexej date="2010-03-19">
    /// cycle time and activation infos of server, used by hndServerInformation handler function
    SmartTimeValue minCycleTime;
    //</alexej>

    /// denotes the name of the port if client can be wired from other components
    std::string portname;

    /// stores pointer to wiring slave when managed port
    WiringSlave *wiringslave;

    //<alexej date="09.10.2008">
    /*
    /// private static functions to be accessed from the CORBA layer
    static void hndUpdate(void*, const CORBA::Any &);

    /// private static functions to be accessed from the CORBA layer
    static void hndServDisc(void*);

    /// private static functions to be accessed from the CORBA layer
    static void hndServerStateChange(void*,int &);
	*/

    /// private static functions to be accessed from the ACE layer
    static void hndUpdate(void*, const SmartMessageBlock *, int sid);

	 /// private static functions to be accessed from the ACE layer
    //<alexej date="2010-03-09">
    static void hndAckConn(void*, int cid, int status);
    static void hndServDisc(void*, int cid);
    //</alexej>
	 static void hndAckDisc(void*);
    static void hndAckSubscribe(void*,int);

    static void hndServerInformation(void *, unsigned long cycleTimeMs, int active);
    static void hndServerStateChange(void*,int &);
    //</alexej>

    /// Default constructor
    ///
    /// throws exception if someone tries to use this constructor
    ///   SMART_ERROR : this constructor is forbidden
    PushClient() throw(SmartACE::SmartError);

    /** (Interface used by wiring service. Requires ordinary pointer since
     *   client patterns have different types and even have different template
     *   parameters. Therefore member function pointers in wiring service would
     *   require type information. For further details on return status see connect()
     *   and disconnect() methods.)
     *
     *   internalConnect @return status code
     *     - SMART_OK                  : everything is OK
     *     - SMART_SERVICEUNAVAILABLE  : the requested server/service is not available and therefore
     *                                   no connection can be established.
     *     - SMART_INCOMPATIBLESERVICE : the denoted service is incompatible (wrong communication
     *                                   pattern or wrong communication objects) and can therefore
     *                                   not be connected. Client is now not connected to any server.
     *     - SMART_ERROR               : something went wrong
     *
     *   internalDisconnect @return status code
     *     - SMART_OK                  : everything is OK
     *     - SMART_ERROR_COMMUNICATION : something went wrong
     */
    static Smart::StatusCode internalConnect(void* ptr, const std::string& server, const std::string& service);
    static Smart::StatusCode internalDisconnect(void* ptr);

    /** @internal
     * Interface used by component management to decouple/handle serverInitiatedDisconnect.
     */
    //<alexej date="2010-03-09">
      static void internalServerInitiatedDisconnect(void*, int cid);
    //</alexej>

  public:
    /** Constructor (exposed as port and wireable from outside by other components).
     *  add()/remove() and connect()/disconnect() can always be used to change the
     *  status of the instance. Instance is not connected to a service provider.
     *
     *  Throws exception if port name is already in use.
     *    - SMART_PORTALREADYUSED : port name already in use, instance not created
     *    - SMART_ERROR           : something went wrong, instance not created
     *
     * @param component  management class of the component
     * @param port       name of the client-port (visible in <i>wiring-slave</i>)
     * @param slave      reference to wiring-slave
     */
    PushClient(SmartComponent* component, const std::string& port, WiringSlave* slave) throw(SmartACE::SmartError);

    /** Constructor (not wired with service provider and not exposed as port).
     *  add()/remove() and connect()/disconnect() can always be used to change
     *  the status of the instance. Instance is not connected to a service provider
     *  and is not exposed as port wireable from outside the component.
     *
     *  (Currently exception not thrown)
     *
     * @param component  management class of the component
     */
    PushClient(SmartComponent* component) throw(SmartACE::SmartError);

    /** Constructor (wired with specified service provider).
     *  Connects to the denoted service and blocks until the connection
     *  has been established. Blocks infinitely if denoted service becomes
     *  not available since constructor performs retries. Blocking is useful to
     *  simplify startup of components which have mutual dependencies.
     *  add()/remove() and connect()/disconnect() can always be used to change
     *  the status of the instance.
     *
     *  Throws exception if denoted service is incompatible (wrong communication
     *  pattern or wrong communication objects).
     *    - SMART_INCOMPATIBLESERVICE : the denoted service is incompatible (wrong communication
     *                                  pattern or wrong communication objects) and can therefore
     *                                  not be connected. Instance is not created.
     *    - SMART_ERROR               : something went wrong, instance not created
     *
     * @param component  management class of the component
     * @param server     name of the server
     * @param service    name of the service
     */
    PushClient(SmartComponent* component, const std::string& server, const std::string& service) throw(SmartACE::SmartError);

    /** Destructor.
     *  The destructor calls remove() and disconnect() and therefore properly cleans up
     *  every pending data reception and removes the instance from the set of wireable ports.
     */
    virtual ~PushClient() throw();

    /** Add this instance to the set of ports wireable via the
     *  wiring pattern from outside the component. Already
     *  established connections keep valid. If this service
     *  requestor is already exposed as port, it is first
     *  removed and then added with the new port name.
     *  add()/remove() and connect()/disconnect() can always
     *  be used to change the status of this instance.
     *
     *  @param slave  wiring slave of this component
     *  @param port   name of port used for wiring
     *
     *  @return status code
     *   - SMART_OK                  : everything is OK and this instance
     *                                 added to the set of ports wireable
     *                                 from outside the component
     *   - SMART_PORTALREADYUSED     : port name already in use and this
     *                                 instance now not available as port
     *   - SMART_ERROR               : something went wrong
     */
    Smart::StatusCode add(WiringSlave* slave, const std::string& port) throw();

    /** Remove this service requestor from the set of ports wireable
     *  via the wiring pattern from outside the component. Already
     *  established connections keep valid but can now be changed
     *  only from inside and not from outside this component anymore.
     *
     *  @return status code
     *   - SMART_OK                  : everything is OK and instance not
     *                                 exposed as port anymore (or was not
     *                                 registered as port).
     *   - SMART_ERROR               : something went wrong but this instance
     *                                 is removed from the set of ports in
     *                                 any case.
     */
    Smart::StatusCode remove() throw();

    /** Connect this service requestor to the denoted service provider. An
     *  already established connection is first disconnected. See disconnect()
     *  for implications on pending data reception in that case.
     *
     *  It is no problem to change the connection to a service provider at any
     *  point of time irrespective of any calls to getUpdate() / getUpdateWait().
     *
     *  @param server   name of the server
     *  @param service  name of the service
     *
     *  @return status code
     *   - SMART_OK                  : everything is OK and connected to the specified service.
     *   - SMART_SERVICEUNAVAILABLE  : the specified service is currently not available and the
     *                                 requested connection can not be established. Service
     *                                 requestor is now not connected to any service provider.
     *   - SMART_INCOMPATIBLESERVICE : the specified service provider is not compatible (wrong communication
     *                                 pattern or wrong communication objects) to this service requestor and
     *                                 can therefore not be connected. Service requestor is now not connected
     *                                 to any service provider.
     *   - SMART_ERROR_COMMUNICATION : communication problems, service requestor is now not connected to any
     *                                 service provider.
     *   - SMART_ERROR               : something went wrong, service requestor is now not connected to any
     *                                 service provider.
     */
    Smart::StatusCode connect(const std::string& server, const std::string& service) throw();

    /** Disconnect the service requestor from the service provider.
     *
     *  A disconnect always first performs an unsubscribe. See unsubsribe() for
     *  implications on getUpdate() and getUpdateWait().
     *
     *  It is no problem to change the connection to a service provider at any
     *  point of time irrespective of any calls to getUpdate() / getUpdateWait().
     *
     *  @return status code
     *   - SMART_OK                  : everything is OK and service requestor is disconnected from
     *                                 the service provider.
     *   - SMART_ERROR_COMMUNICATION : something went wrong at the level of the intercomponent
     *                                 communication. At least the service requestor is in the
     *                                 disconnected state irrespective of the service provider
     *                                 side clean up procedures.
     *   - SMART_ERROR               : something went wrong. Again at least the service requestor
     *                                 is in the disconnected state.
     */
    Smart::StatusCode disconnect() throw();

    /** Subscribe at the server to periodically get every n-th update. A
     *  newly subscribed client gets the next available new data and is
     *  then updated with regard to its individual update cycle.
     *
     *  @param prescale  whole-numbered value to set the update rate to
     *                   every n-th value
     *
     *  @return status code
     *    - SMART_OK                  : everything is ok and client is subscribed
     *    - SMART_DISCONNECTED        : client is not connected to a server and can therefore
     *                                  not subscribe for updates, not subscribed
     *    - SMART_ERROR_COMMUNICATION : communication problems, not subscribed
     *    - SMART_ERROR               : something went wrong, not subscribed
     */
    Smart::StatusCode subscribe(const int &prescale = 1) throw();

    /** Unsubscribe to get no more updates. All blocking calls are aborted with the appropriate
     *  status and yet received and still buffered data is deleted to avoid returning old data.
     *
     *  @return status code
     *    - SMART_OK                  : everything is ok and client is now unsubscribed or
     *                                  has already been unsubscribed
     *    - SMART_ERROR_COMMUNICATION : communication problems, not unsubscribed
     *    - SMART_ERROR               : something went wrong, not unsubscribed
     *
     * (can not return SMART_DISCONNECTED since then client is for sure also unsubscribed
     *  which results in SMART_OK)
     */
    Smart::StatusCode unsubscribe() throw();

    /** Allow or abort and reject blocking calls.
     *
     *  If blocking is set to false all blocking calls return with SMART_CANCELLED. This can be
     *  used to abort blocking calls.
     *
     *  @param b (blocking)  true/false
     *
     *  @return status code
     *   - SMART_OK                  : new mode set
     *   - SMART_ERROR               : something went wrong
     */
     Smart::StatusCode blocking(const bool b) throw();

    /** Non-blocking call to immediately return the latest available
     *  data buffered at the client side from the most recent update.
     *
     *  No data is returned as long as no update is received since
     *  subscription. To avoid returning old data, no data is
     *  returned after the client is unsubscribed or when the
     *  server is not active.
     *
     * @param d is set to the newest currently available data
     *
     * @return status code
     *   - SMART_OK                  : everything ok and latest data since client got subscribed
     *                                 is returned.
     *   - SMART_NODATA              : client has not yet received an update since subscription and
     *                                 therefore no data is available and no data is returned.
     *   - SMART_NOTACTIVATED        : the server is currently not active and does therefore not
     *                                 provide updates at the expected rate. No valid data returned.
     *   - SMART_UNSUBSCRIBED        : no data available since client is not subscribed and can
     *                                 therefore not receive updates. Method does not return old data from
     *                                 last subscription since these might be based on too old parameter
     *                                 settings. To get data one has to be subscribed.
     *   - SMART_DISCONNECTED        : no data returned since client is even not connected to a server.
     *   - SMART_ERROR               : something went wrong
     */
     Smart::StatusCode getUpdate(DataType& d) throw();

    /** Blocking call which waits until the next update is received.
     *
     *  Blocking is aborted with the appropriate status if either the
     *  server gets deactivated, the client gets unsubscribed or disconnected
     *  or if blocking is not allowed any more at the client.
     *
     *  @param d is set to the newest currently available data
     *  @param timeout allows to release the blocking wait after a given timeout time (the default max_time means infinite blocking)
     *
     *  @return status code
     *   - SMART_OK                  : everything is ok and just received data is returned.
     *   - SMART_CANCELLED           : blocking is not allowed or is not allowed anymore. Waiting for the
     *                                 next update is aborted and no valid data is returned.
     *   - SMART_NOTACTIVATED        : the server is currently not active and does therefore not provide updates
     *                                 at the expected rate. No valid data returned.
     *   - SMART_UNSUBSCRIBED        : returns immediately without data if the client is not subscribed.
     *   - SMART_DISCONNECTED        : returns immediately without data since client is even not connected
     *                                 to a server.
     *   - SMART_ERROR               : something went completely wrong and no valid data returned.
     */
     Smart::StatusCode getUpdateWait(DataType& d, const std::chrono::steady_clock::duration &timeout=std::chrono::steady_clock::duration::zero()) throw();

    /** Get cycle time and server state.
     *
     *  Returns cycle time of server in [seconds] and server state indicating
     *  whether server is activated or not. The client can decide on the cycle
     *  time on its individual update rate at subscription.
     *
     * @param t is set to the server cycle time [seconds]
     * @param r indicates whether server is started [true, false]
     *
     * @return status code
     *   - SMART_OK                  : everything is ok and returned values are valid.
     *   - SMART_DISCONNECTED        : client is not connected to a server and
     *                                 therefore not get any valid server info.
     *   - SMART_ERROR_COMMUNICATION : communication problems and returned server info is not valid.
     *   - SMART_ERROR               : something went completely wrong and no valid server info returned.
     */
     Smart::StatusCode getServerInfo(SmartTimeValue& t,bool& r) throw();
  };
}


/////////////////////////////////////////////////////////////////////////
//
// server part
//
/////////////////////////////////////////////////////////////////////////

//
// server push timed template
//
namespace SmartACE {

  /** Server part of the Push pattern to provide a push timed
   *  service. A user provided handler method is regularly triggered
   *  to initiate data acquisition. Newly acquired data is provided
   *  to the server by the put-method. It immediately distributes
   *  the new data to subscribed clients taking into account their
   *  individual update cycles.
   *
   *  Template parameters
   *    - <b>T</b>: Pushed value class (Communication Object)
   *
   *  Demonstrated in <a href="/drupal/?q=node/51#eigth-example">eigth example</a>
   */
  template<class DataType> class PushServer : public Smart::IPushServerPattern<DataType> {
  private:
    typedef struct PushClientList {
      //<alexej date="2010-03-09">
      PushServerServiceHandler    *clientProxy;
      int                              connection_id;
      int                              subscription_id;
      //</alexej>
      int                              ctrReset;
      int                              ctrCurrent;
      int                              subscribed;
      PushClientList              *next;
    }PushClientList;

    SmartRecursiveMutex mutexClientList;

    //<alexej date="2010-03-18">
    /// Administrative Monitor for handling server-initiated-disconnect
    SmartMonitor monitorServerInitiatedDisconnect;
    //</alexej>

    //<alexej date="2010-03-18">
    /// UUID generator is used to generate unique service-identifier
    ACE_Utils::UUID_Generator uuid_genrator;

    /// universal unique ID is used to ignore dead-ends in naming-service
    ACE_Utils::UUID *service_identifier;
    //</alexej>

    PushClientList *subscriptions;

    /// management class of the component
    SmartComponent *component;

    /// name of service
    std::string service;

    /// is used for destruction of server with proper handling of
    /// still connected clients. See destructor ...
    int serverReady;

    /// cycleTime of the service in seconds
    SmartTimeValue minCycleTime;
    SmartTimeValue lastPushTime;

    /// number of the timer returned with activation from TimerQueue
    int timer;

    //<alexej date="09.10.2008">
    /*
    /// the CORBA server object
    PushServer_impl *corbaServant;

    /// private functions
    static SmartACE::StatusCode hndCnct(void*, const SmartPushClientPattern_ptr);

    /// private functions
    static void hndDisc(void*, const SmartPushClientPattern_ptr);

    /// private functions
    static void hndSubscribe(void*, const SmartPushClientPattern_ptr,int,int&);

    /// private functions
    static void hndUnsubscribe(void*, const SmartPushClientPattern_ptr);
    */

    /// the ACE Acceptor class
    PushServerAcceptor *acceptor;

    //<alexej date="2010-03-18">
    /// private handler function
    static void hndConnect(void*, const PushServerServiceHandler *,int,const ACE_Utils::UUID&);

    /// private handler function
    static void hndDiscard(void*, const PushServerServiceHandler *);

    /// private handler function
    static void hndDisconnect(void*, const PushServerServiceHandler *);

    /// private functions (returns server state)
    static void hndSubscribe(void*, const PushServerServiceHandler*,const int&, const int&);

    /// private functions
    static void hndUnsubscribe(void*, const PushServerServiceHandler*);
    //</alexej>


    /// private functions
    static void hndServerInfo(void*, const PushServerServiceHandler*);

    // method implementing the server-initiated-disconnect (SID) procedure
    virtual void serverInitiatedDisconnect();

    /// Default constructor
    ///
    /// throws exception if someone tries to use this constructor
    ///   SMART_ERROR : this constructor forbidden
    PushServer() throw(SmartACE::SmartError);

  public:
    /** Constructor.
     *
     * @param component  management class of the component
     * @param service    name of the service
     * @param handler    is called every \e cycle seconds to push the information
     * @param cycle      cycle time of the service in [seconds]
     *
     * (Currently exception not thrown)
     */
    PushServer(SmartComponent* component, const std::string& svc, const SmartTimeValue &minCycleTime=SmartTimeValue::zero) throw(SmartACE::SmartError);

    /** Destructor.
     *  Properly disconnects all service requestors in case of destruction
     *  such that all connected and subscribed clients are unsubscribed and
     *  disconnected properly.
     */
    virtual ~PushServer() throw();

    /** Provide new data which is sent to all subscribed clients
     *  taking into account their individual update cycles. Update
     *  cycles are always whole-numbered multiples of the server
     *  update cycle.
     *
     *  (Individual update interval counters are incremented each
     *   time this member function is called irrespectively of the
     *   elapsed time. One should use the time triggered handler to
     *   call the put() member function with the appropriate timing.)
     *
     *  PushHandler::handlePushRequest() directly or indirectly
     *  use this method to provide new data.
     *
     *  @param d contains the newly acquired data to be sent as
     *           update.
     *
     *  @return status code
     *    - SMART_OK                  : everything is ok
     *    - SMART_NOTACTIVATED        : server is stopped and does therefore not
     *                                  distribute any data to clients. In that
     *                                  case update interval counters are not
     *                                  touched.
     *    - SMART_ERROR_COMMUNICATION : communication problems caused by at least
     *                                  one client. The other clients are updated
     *                                  correctly.
     *    - SMART_ERROR               : something went completely wrong with at least one
     *                                  client. Some clients might still been
     *                                  updated correctly.
     */
    Smart::StatusCode put(const DataType& d) throw();
  };
}

////////////////////////////////////////////////////////////////////////
//
// include template code
//
////////////////////////////////////////////////////////////////////////
#include "smartPush.th"

#endif    // _SMARTPUSH_HH

