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

#ifndef _SMARTQUERY_HH
#define _SMARTQUERY_HH

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <string>

#include <smartIQueryClientPattern_T.h>
#include <smartIQueryServerPattern_T.h>
#include <smartNumericCorrelationId.h>

#include "smartComponent.hh"
#include "smartQueryServerPattern.hh"
#include "smartQueryClientPattern.hh"
#include "smartQueryServerHandler.hh"

//
// needed for the add() / remove() interface for wiring
//
#include "smartWiring.hh"

/////////////////////////////////////////////////////////////////////////
//
// client part
//
/////////////////////////////////////////////////////////////////////////

/** Namespace of the framework */
namespace SmartACE {

  /** Client part of query pattern to perform two-way communication.
   *
   *  Template parameters
   *    - <b>R</b>: request class (Communication Object)
   *    - <b>A</b>: answer (reply) class (Communication Object)
   *
   *  Demonstrated in <a href="/drupal/?q=node/51#first-example">first example</a> and <a href="/drupal/?q=node/51#third-example">third example</a>
   */
  template<class R, class A> class QueryClient : public Smart::IQueryClientPattern<R,A> {
  private:
    typedef struct QueryClientList {
      Smart::QueryIdPtr  id;
      SmartCVwithMemory  cond;
      A                  a;
      Smart::QueryStatus querystatus;
      int                disconnect;
      QueryClientList   *next;
    }QueryClientList;

    /// mutexQueryList   protects the list of pending queries while being
    ///                  accessed since this list is being manipulated from
    ///                  various member functions and handlers called from
    ///                  server side in parallel
    /// mutexConnection  protects critical sections from being executed in
    ///                  parallel. These are all related to changing connections
    ///                  while client/server interactions are active. The mutex
    ///                  avoids racing conditions when for example a queryRequest
    ///                  is being executed and a disconnect is called in parallel.
    QueryClientList     *queries;
    SmartRecursiveMutex mutexQueryList;
    SmartRecursiveMutex mutexConnection;
    Smart::NumericCorrelationId queryCnt;

    //<alexej date="2010-03-18">
    /// Administrative monitor for handling connects
    SmartAdministrativeMonitor monitorConnect;

    // Administrative Monitor for handling disconnects
    SmartAdministrativeMonitor monitorDisconnect;
    //</alexej>


    /// management class of the component
    SmartComponent *component;


    //<alexej date="02.10.2008">
    /*
    /// the CORBA client object
    QueryClient_impl *corbaClient;

    /// the CORBA client reference transferred to server
    SmartQueryClientPattern_ptr h;

    /// the CORBA server connection
    SmartQueryServerPattern_var corbaServant;
	*/

    /// ACE_Connector implementation for SendClients connector feature
    QueryClientConnector *connector;

    /// ServiceHandler handles all low level communication with server side
    QueryClientServiceHandler *serverProxy;
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

    /// denotes the name of the port if client can be wired from other components
    std::string portname;

    /// indicates the user status with respect to blocking, true/false blocking/non-blocking, default: true
    /// needs to be stored in pattern since every queryRequest generates new condition variable which needs
    /// proper initialization
    bool statusUserBlocking;

    /// stores pointer to wiring slave when managed port
    WiringSlave *wiringslave;

    //<alexej date="2009-08-27">
		/// private static functions to be accessed from the CORBA layer
		//static void hndAnsw(void*, const CORBA::Any &, const CORBA::Long);
		static void hndAnsw(void*, const SmartACE::SmartMessageBlock*, size_t);
    //</alexej>

    /// private static functions to be accessed from the ACE layer
    //<alexej date="2010-03-09">
    static void hndAckConn(void*, int cid, int status);
    static void hndServDisc(void*, int cid);
	 static void hndAckDisc(void*);
    //</alexej>

    /// Default constructor
    ///
    /// throws exception if someone tries to use this constructor
    ///   SMART_ERROR : this constructor is forbidden
    QueryClient();

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
     * @param port       name of the wireable port
     * @param slave      wiring slave of this component
     */
    QueryClient(SmartComponent* component, const std::string& port, WiringSlave* slave);

    /** Constructor (not wired with service provider and not exposed as port).
     *  add()/remove() and connect()/disconnect() can always be used to change
     *  the status of the instance. Instance is not connected to a service provider
     *  and is not exposed as port wireable from outside the component.
     *
     *  (Currently exception not thrown)
     *
     * @param component  management class of the component
     */
    QueryClient(SmartComponent* component);

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
    QueryClient(SmartComponent* component, const std::string& server, const std::string& service);

    /** Destructor.
     *  The destructor calls remove() and disconnect() and therefore properly cleans up
     *  every pending query and removes the instance from the set of wireable ports.
     */
    virtual ~QueryClient();

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
    Smart::StatusCode add(WiringSlave* slave, const std::string& port);

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
    Smart::StatusCode remove();

    /** Connect this service requestor to the denoted service provider. An
     *  already established connection is first disconnected. See disconnect()
     *  for implications on running or pending queries in that case.
     *
     *  It is no problem to change the connection to a service provider at any
     *  point of time irrespective of any calls to query() / queryRequest() / queryReceive() or
     *  queryReceiveWait().
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
    Smart::StatusCode connect(const std::string& server, const std::string& service);

    /** Disconnect the service requestor from the service provider.
     *
     *  All blocking queries are aborted with the SMART_DISCONNECTED status
     *  code. Already received but not yet picked up answers keep valid and
     *  can still be picked up with queryReceive()/queryReceiveWait(). Not
     *  yet received answers at the time of disconnection are lost and
     *  calls to queryReceive()/queryReceiveWait() return with SMART_DISCONNECTED.
     *
     *  It is no problem to change the connection to a service provider at any
     *  point of time irrespective of any calls to query() / queryRequest() / queryReceive()
     *  or queryReceiveWait().
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
    Smart::StatusCode disconnect();

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
    Smart::StatusCode blocking(const bool b);

    /** Blocking Query.
     *
     *  Perform a blocking query and return only when the query answer
     *  is available. Member function is thread safe and thread reentrant.
     *
     *  @param request send this request to the server (Communication Object)
     *  @param answer  returned answer from the server (Communication Object)
     *
     *  @return status code:
     *    - SMART_OK                  : everything is ok and <I>answer</I> contains answer
     *    - SMART_CANCELLED           : blocking is not allowed or is not allowed anymore and therefore
     *                                  pending query is aborted, answer is lost and <I>answer</I>
     *                                  contains no valid answer.
     *    - SMART_DISCONNECTED        : the client is either disconnected and no query
     *                                  can be made or it got disconnected and a pending
     *                                  query is aborted without answer. In both cases,
     *                                  <I>answer</I> is not valid.
     *    - SMART_ERROR_COMMUNICATION : communication problems, <I>answer</I> is not valid.
     *    - SMART_ERROR               : something went wrong, <I>answer</I> is not valid.
     */
    Smart::StatusCode query(const R& request, A& answer);

    /** Asynchronous Query.
     *
     *  Perform a query and receive the answer later, returns immediately.
     *  Member function is thread safe and reentrant.
     *
     *  @param request send this request to the server (Communication Object)
     *  @param id      is set to the identifier which is later used to receive
     *                 the reply to this request
     *
     *  @return status code:
     *    - SMART_OK                  : everything is ok and <I>id</I> contains query identifier
     *                                  used to either fetch or discard the answer.
     *    - SMART_DISCONNECTED        : request is rejected since client is not connected to a server
     *                                  and therefore <I>id</I> is not a valid identifier.
     *    - SMART_ERROR_COMMUNICATION : communication problems, <I>id</I> is not valid.
     *    - SMART_ERROR               : something went wrong, <I>id</I> is not valid.
     */
    Smart::StatusCode queryRequest(const R& request, Smart::QueryIdPtr& id);

    /** Check if answer is available.
     *
     *  Non-blocking call to fetch the answer belonging to the given identifier.
     *  Returns immediately. Member function is thread safe and reentrant.
     *
     *  @warning
     *    It is not allowed to call queryReceive(), queryReceiveWait() or queryDiscard() concurrently
     *    with the <I>same</I> query id (which is not a restriction since it makes no sense !)
     *
     *  @param id      provides the identifier of the query
     *  @param answer  is set to the answer returned from the server if it was available
     *
     *  @return status code:
     *    - SMART_OK           : everything is ok and <I>answer</I> contains the answer
     *    - SMART_WRONGID      : no pending query with this identifier available, therefore no valid
     *                           <I>answer</I> returned.
     *    - SMART_NODATA       : answer not yet available, therefore try again later. The identifier <I>id</I>
     *                           keeps valid, but <I>answer</I> contains no valid answer.
     *    - SMART_DISCONNECTED : the answer belonging to the <I>id</I> can not be received
     *                           anymore since the client got disconnected. <I>id</I> is
     *                           not valid any longer and <I>answer</I> contains no valid answer.
     *    - SMART_ERROR        : something went wrong, <I>answer</I> contains no answer and <I>id</I> is
     *                           not valid any longer.
     *
     */
    Smart::StatusCode queryReceive(const Smart::QueryIdPtr id, A& answer);

    /** Wait for reply.
     *
     *  Blocking call to fetch the answer belonging to the given identifier. Waits until
     *  the answer is received.
     *
     *  @warning
     *    It is not allowed to call queryReceive(), queryReceiveWait() or queryDiscard() concurrently
     *    with the <I>same</I> query id (which is not a restriction since it makes no sense !)
     *
     *  @param id       provides the identifier of the query
     *  @param answer   is set to the answer returned from the server if it was available
     *  @param timeout  the optional maximum waiting time for a query answer (default value max: method blocks indefinitely)
     *
     *  @return status code:
     *    - SMART_OK           : everything is ok and <I>answer</I> contains the answer
     *    - SMART_WRONGID      : no pending query with this identifier available, therefore no
     *                           valid <I>answer</I> returned.
     *    - SMART_CANCELLED    : blocking call is not allowed or is not allowed anymore and therefore
     *                           blocking call is aborted and no valid <I>answer</I> is returned. The
     *                           query identifier <I>id</I> keeps valid and one can either again call
     *                           queryReceive(), queryReceiveWait() or discard the answer by calling
     *                           queryDiscard().
     *    - SMART_DISCONNECTED : blocking call is aborted and the answer belonging to <I>id</I> can not
     *                           be received anymore since client got disconnected. <I>id</I> is not valid
     *                           any longer and <I>answer</I> contains no valid answer.
     *    - SMART_ERROR        : something went wrong, <I>answer</I> contains no answer and <I>id</I> is
     *                           not valid any longer.
     *
     */
    Smart::StatusCode queryReceiveWait(const Smart::QueryIdPtr id, A& answer, const Smart::Duration &timeout=Smart::Duration::max());

    /** Discard the pending answer with the identifier <I>id</I>
     *
     *  Call this member function if you do not want to get the answer of a request anymore which
     *  was invoked by queryRequest(). This member function invalidates the identifier <I>id</I>.
     *
     *  @warning
     *    This member function does NOT abort blocking calls ! This is done by the blocking() member
     *    function. It has to be called if you have not yet received an answer and the identifier is
     *    still valid, for example due to a CANCELLED return value, and you don't want to get the
     *    answer anymore.
     *
     *  @warning
     *    It is not allowed to call queryReceive(), queryReceiveWait() or queryDiscard() concurrently
     *    with the <I>same</I> query id (which is not a restriction since it makes no sense !)
     *
     *  @param id  provides the identifier of the query
     *
     *  @return status code:
     *    - SMART_OK           : everything is ok and query with the identifier <I>id</I> discarded.
     *    - SMART_WRONGID      : no pending query with this identifier.
     *    - SMART_ERROR        : something went wrong, <I>id</I> not valid any longer.
     *
     */
    Smart::StatusCode queryDiscard(const Smart::QueryIdPtr id);
  };
}


/////////////////////////////////////////////////////////////////////////
//
// server part
//
/////////////////////////////////////////////////////////////////////////

namespace SmartACE {
  /** server query template.
   *  Server part of query pattern to perform two-way communication.
   *
   *  Template parameters
   *    - <b>R</b>: request class (Communication Object)
   *    - <b>A</b>: answer (reply) class (Communication Object)
   *
   *  Demonstrated in <a href="/drupal/?q=node/51#first-example">first example</a> and <a href="/drupal/?q=node/51#third-example">third example</a>
   */
  template<class R, class A> class QueryServer : public Smart::IQueryServerPattern<R,A> {
  private:
    //
    // element of list of not yet answered queries
    //
    typedef struct QueryServerList {
      Smart::NumericCorrelationId id;
      Smart::NumericCorrelationId cltId;
      R                r;
      //<alexej date="26.11.2008">
	    //SmartQueryClientPattern_ptr client;
        QueryServerServiceHandler *clientProxy;
      //</alexej>
      Smart::QueryStatus     querystatus;
      QueryServerList *next;
    }QueryServerList;

    //
    // element of list of connected clients
    //
    typedef struct QueryClientList {
      //<alexej date="2010-03-09">
    	QueryServerServiceHandler *clientProxy;
    	int                       connection_id;
      //</alexej>
      QueryClientList *next;
    }QueryClientList;

    /// see comment at client part
    QueryServerList     *queries;
    QueryClientList     *clients;
    SmartRecursiveMutex mutexQueryList;
    SmartRecursiveMutex mutexClientList;
    Smart::NumericCorrelationId queryCnt;

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

    /// management class of the component
    SmartComponent *component;

    /// name of service
    std::string service;

    /// is used for destruction of server with proper handling of
    /// still connected clients. See destructor and incoming handlers
    int serverReady;


    //<alexej date="02.10.2008">
		/*
		/// the CORBA server object
		QueryServer_impl *corbaServant;

		/// private functions
		static void hndRqst(void*, const CORBA::Any &, const SmartQueryClientPattern_ptr, const CORBA::Long);

		/// private functions
		static Smart::StatusCode hndCnct(void*, const SmartQueryClientPattern_ptr);

		/// private functions
		static void hndDisc(void*, const SmartQueryClientPattern_ptr);
		*/

    /// the ACE Acceptor class
    QueryServerAcceptor *acceptor;

	/// private handler function
	static void hndRqst(void*, const SmartMessageBlock*, const QueryServerServiceHandler*, size_t);

    //<alexej date="2010-03-18">
    /// private handler function
    static void hndConnect(void*, const QueryServerServiceHandler *,int,const ACE_Utils::UUID&);

    /// private handler function
    static void hndDiscard(void*, const QueryServerServiceHandler *);

    /// private handler function
    static void hndDisconnect(void*, const QueryServerServiceHandler *);
    //</alexej>

    // method implementing the server-initiated-disconnect (SID) procedure
    virtual void serverInitiatedDisconnect();

    /// Default constructor
    ///
    /// throws exception if someone tries to use this constructor
    ///   SMART_ERROR : this constructor forbidden
    QueryServer();

  public:
	using IQueryServerBase = Smart::IQueryServerPattern<R,A>;
	using typename IQueryServerBase::IQueryServerHandlerPtr;

    /** Constructor.
     *
     *  Note that a handler has to be supplied. Without a handler, the
     *  QueryServer could not accept a query.
     *
     *  (Currently exception not thrown)
     *
     *  @param component management class of the component
     *  @param service   name of the service
     */
    QueryServer(SmartComponent* component, const std::string& service, IQueryServerHandlerPtr query_handler = nullptr);

    /** Destructor.
     *  Properly disconnects all service requestors in case of destruction
     *  such that all pending queries are handled correctly at client side
     *  even when the service provider disappears during pending queries.
     */
    virtual ~QueryServer();

    /** Provide answer to be sent back to the requestor.
     *
     *  Member function is thread safe and thread reentrant.
     *
     *  @param id identifies the request to which the answer belongs
     *  @param answer is the reply itself.
     *
     *  @return status code:
     *    - SMART_OK                  : everything is ok and answer sent to requesting client
     *    - SMART_WRONGID             : no pending query with that <I>id</I> known
     *    - SMART_DISCONNECTED        : answer not needed anymore since client
     *                                  got disconnected meanwhile
     *    - SMART_ERROR_COMMUNICATION : communication problems
     *    - SMART_ERROR               : something went wrong
     */
    Smart::StatusCode answer(const Smart::QueryIdPtr id, const A& answer);

  };
}

////////////////////////////////////////////////////////////////////////
//
// include template code
//
////////////////////////////////////////////////////////////////////////
#include "smartQuery.th"

#endif    // _SMARTQUERY_HH

