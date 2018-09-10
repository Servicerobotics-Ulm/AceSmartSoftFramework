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


#ifndef _SMARTSEND_HH
#define _SMARTSEND_HH

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <string>

#include <smartISendClientPattern_T.h>
#include <smartISendServerPattern_T.h>

#include "smartComponent.hh"
#include "smartSendServerPattern.hh"
#include "smartSendClientPattern.hh"

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
// client send template
//
namespace SmartACE {

  /** Client part of one-way communication pattern.
   *
   *  Template parameters
   *    - <b>C</b>: command class (Communication Object)
   *
   *  Demonstrated in <a href="/drupal/?q=node/51#sixth-example">sixth example</a>
   */
  template<class C> class SendClient : public Smart::ISendClientPattern<C> {
  private:
    /// mutex to protect "statusConnected"/"statusManagedPort"/"portname"/"corbaServant"
    /// (see further hints in smartQuery.hh)
    SmartRecursiveMutex mutexConnection;

    //<alexej date="2010-03-18">
    /// Administrative monitor for handling connects
    SmartAdministrativeMonitor monitorConnect;

    // Administrative Monitor for handling disconnects
    SmartAdministrativeMonitor monitorDisconnect;
    //</alexej>

    /// management class of the component
    SmartComponent *component;


    //<alexej date="13.11.2008">
    /*
		/// the CORBA client object
		SendClient_impl *corbaClient;

		/// the CORBA client reference transferred to server
		SmartSendClientPattern_ptr h;

		/// the CORBA server connection
		SmartSendServerPattern_var corbaServant;
    */

    /// ACE_Connector implementation for SendClients connector feature
    SendClientConnector *connector;

    /// ServiceHandler handles all low level communication with server side
    SendClientServiceHandler *serverProxy;
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

    /// stores pointer to wiring slave when managed port
    SmartACE::WiringSlave *wiringslave;

    /// private static functions to be accessed from the ACE layer
    //<alexej date="2010-03-09">
    static void hndAckConn(void*, int cid, int status);
    static void hndServDisc(void*, int cid);
    static void hndAckDisc(void*);
    //</alexej>

    /// Default constructor
    ///
    /// throws exception if someone tries to use this constructor
    ///   SMART_ERROR : this constructor forbidden
    SendClient() throw(SmartACE::SmartError);

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
    /** Constructor (port wireable from outside by other components).
     *  add()/remove() and connect()/disconnect() can always be used to
     *  change the status of the client object. Client is not connected
     *  to a server.
     *
     *  Throws exception if port name is already in use.
     *    - SMART_PORTALREADYUSED : port name already in use
     *    - SMART_ERROR           : something went wrong, instance not created
     *
     * @param component  management class of the component
     * @param port       name of the wireable port
     * @param slave      wiring slave of this component
     */
    SendClient(SmartComponent* component, const std::string& port, SmartACE::WiringSlave* slave) throw(SmartACE::SmartError);

    /** Constructor (not wired with service provider and not exposed as port).
     *  add()/remove() and connect()/disconnect() can always be used to change
     *  the status of the instance. Instance is not connected to a service provider
     *  and is not exposed as port wireable from outside the component.
     *
     *  (Currently exception not thrown)
     *
     * @param component  management class of the component
     */
    SendClient(SmartComponent* component) throw(SmartACE::SmartError);

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
    SendClient(SmartComponent* component, const std::string& server, const std::string& service) throw(SmartACE::SmartError);

    /** Destructor.
     *  The destructor calls remove() and disconnect() and therefore properly cleans up.
     */
    virtual ~SendClient() throw();

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
    Smart::StatusCode add(SmartACE::WiringSlave* slave, const std::string& port) throw();

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
     *  already established connection is first disconnected.
     *
     *  It is no problem to change the connection to a service provider at any
     *  point of time irrespective of any calls to send().
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
     *  It is no problem to change the connection to a service provider at any
     *  point of time irrespective of any calls to send().
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

    /** Allow or abort and reject blocking calls.
     *
     *  If blocking is set to false all blocking calls return with SMART_CANCELLED. This can be
     *  used to abort blocking calls. Since this pattern currently does not have any blocking
     *  member functions, currently without effect.
     *
     *  @param b (blocking)  true/false
     *
     *  @return status code
     *   - SMART_OK                  : new mode set
     *   - SMART_ERROR               : something went wrong
     */
    Smart::StatusCode blocking(const bool b) throw();


    /** Perform a one-way communication. Appropriate status codes make
     *  sure that the information has been transferred.
     *
     *  @param c the object to be sent (Communication Object)
     *
     *  @return status code:
     *    - SMART_OK                  : everything is ok and communication object sent to server
     *    - SMART_DISCONNECTED        : the client is disconnected and no send can be made
     *    - SMART_ERROR_COMMUNICATION : communication problems, data not transmitted
     *    - SMART_ERROR               : something went wrong, data not transmitted
     */
    Smart::StatusCode send(const C& c) throw();
  };
};


/////////////////////////////////////////////////////////////////////////
//
// server part
//
/////////////////////////////////////////////////////////////////////////

namespace SmartACE {

  // forward declaration
  template <class C> class SendServer;

  //
  // server send template
  //

  /** Handler Class for SendServer for incoming commands.
   *
   *  Used by the SendServer to handle incoming commands.
   *  The user should provide the handleSend() method by
   *  subclassing and register an instance of this handler
   *  class with the SendServer.
   */

  template <class C>
  class SendServerHandler : public Smart::ISendServerHandler<C> {
  public:
    /** Default constructor
     *
     * This constructor expects a pointer to the related SendServer as the main argument.
     * A SendServer uses this handler to process server-side send-requests.
     * A user needs to implement the handleSend method.
     *
     * @param server a pointer to the related SendServer
     */
    SendServerHandler(SendServer<C> *server)
    : Smart::ISendServerHandler<C>(server)
    { }
    virtual ~SendServerHandler()
    { }

    /** Handler method for an incoming command.
     *
     *  This method is called by the communication pattern every time
     *  a new command is received. It must be provided by the component
     *  developer to handle incoming commands. Since the method is
     *  executed by the communication thread, it must be very fast
     *  and non-blocking.
     *
     *  Usually the command will be inserted into a queue and another
     *  working thread processes the command. The ThreadedSendHandler
     *  decorator provides such a processing pattern.
     *
     *  @param cmd command object (Communication Object)
     */
    virtual void handleSend(const C& cmd) throw() = 0;
  };


  /** Server part of one-way communication pattern.
   *
   *  Template parameters:
   *    - <b>C</b>: command class (Communication Object)
   *
   *  Demonstrated in <a href="/drupal/?q=node/51#sixth-example">sixth example</a>
   */
  template<class C> class SendServer : public Smart::ISendServerPattern<C> {
  private:
    //
    // element of list of connected clients
    //
    typedef struct SendClientList {
      //<alexej date="2010-03-18">
    	SendServerServiceHandler *clientProxy;
    	int                       connection_id;
      //</alexej>
      SendClientList *next;
    }SendClientList;

    SendClientList      *clients;
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

    /// management class of the component
    SmartComponent *component;

    /// name of service
    std::string service;

    /// is used for destruction of server with proper handling of
    /// still connected clients. See destructor and incoming handlers
    int serverReady;


    //<alexej date="2010-03-18">
    /*
		/// the CORBA server object
		SendServer_impl *corbaServant;

		/// private functions
		static void hndCmd(void*, const CORBA::Any &);

		/// private functions
		static Smart::StatusCode hndCnct(void*, const SmartSendClientPattern_ptr);

		/// private functions
		static void hndDisc(void*, const SmartSendClientPattern_ptr);
	*/
    /// the ACE Acceptor class
    SendServerAcceptor *acceptor;

    /// private handler function
    static void hndCmd(void*, const SmartMessageBlock *);

    /// private handler function
    static void hndConnect(void*, const SendServerServiceHandler *,int,const ACE_Utils::UUID&);

    /// private handler function
    static void hndDiscard(void*, const SendServerServiceHandler *);

    /// private handler function
    static void hndDisconnect(void*, const SendServerServiceHandler *);
    //</alexej>

    // method implementing the server-initiated-disconnect (SID) procedure
    virtual void serverInitiatedDisconnect();

    /// Default constructor
    ///
    /// throws exception if someone tries to use this constructor
    ///   SMART_ERROR : this constructor forbidden
    SendServer() throw(SmartACE::SmartError);

  public:
    /** constructor.
     *
     *  Note that a handler has to be supplied. Without a handler, the
     *  SendServer could not accept a command.
     *
     *  (Currently exception not thrown)
     *
     *  @param component management class of the component
     *  @param service   name of the service
     */
    SendServer(SmartComponent* component, const std::string& service) throw(SmartACE::SmartError);

    /** Destructor.
     *  Properly disconnects all service requestors in case of destruction.
     */
    virtual ~SendServer() throw();
  };
};

////////////////////////////////////////////////////////////////////////
//
// include template code
//
////////////////////////////////////////////////////////////////////////
#include "smartSend.th"

#endif    // _SMARTSEND_HH

