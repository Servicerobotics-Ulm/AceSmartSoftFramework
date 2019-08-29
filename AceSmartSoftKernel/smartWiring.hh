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

#ifndef _SMARTWIRING_HH
#define _SMARTWIRING_HH

#include <cstdio>

#ifdef WIN32
	#pragma warning( disable : 4290 )
#endif


#include <cstdlib>
#include <cstring>
#include <cmath>

#include <string>

#include "smartComponent.hh"
#include "smartCommWiring.hh"

#include "smartQueryServerHandler.hh"
#include "smartProcessingPatterns.hh"


/////////////////////////////////////////////////////////////////////////
//
// master part
//
/////////////////////////////////////////////////////////////////////////

namespace SmartACE {
  /** Master part of wiring pattern.
   *
   *  Provides dynamic wiring of components. Client parts of the communication
   *  patterns can be dynamically wired to appropriate servers if they register
   *  themself as ports. Dynamic wiring allows context dependent configuration
   *  of data flow between components as is often needed at the skill level of
   *  robotic architectures. The master part of the wiring pattern is visible
   *  to the user and allows to specify to which service of a server component
   *  a specific port of a component should connect to. Connections can be
   *  changed at any time since all communication patterns provide status
   *  information to cope with modified connections.<br>
   *
   *  Demonstrated in <a href="/drupal/?q=node/51#ninth-example">ninth example</a>
   */
  class WiringMaster {
  private:
    SmartMutex      mutex;

    /// management class of the component
    SmartComponent *component;

    /// Default constructor
    WiringMaster() ;

	//<alexej date="2009-06-17">
	/// internal state variable detecting disabled NamingService
	bool _use_ns;
	//</alexej>

  public:
    /** Constructor.
     *
     *  @param component management class of the component
     *
     *  (Currently exception not thrown)
     *
     */
    WiringMaster(SmartComponent* component);

    /** Destructor.
     */
    virtual ~WiringMaster();

    /** Blocking call to connect port of the slave component to a service of a server component.
     *
     *  Connect the port <I>slaveprt</I> of the slave component <I>slavecmpt</I> to
     *  the service <I>serversvc</I> of the component <I>servercmpt</I>.
     *
     *  @warning
     *    This method must not be called concurrently from several threads nor concurrently
     *    with the disconnect() method.
     *
     *  @param slavecmpt   name of the slave component
     *  @param slaveprt    name of the port to be connected
     *  @param servercmpt  name of the component to connect to
     *  @param serversvc   name of the service in component <I>servercmpt</I> to connect to
     *
     *  @return status code:
     *    - SMART_OK                  : everything is ok and the requested connection has been
     *                                  established successfully. An old connection is first
     *                                  removed before a new connection is established.
     *    - SMART_CANCELLED           : blocking is not allowed or is not allowed anymore and
     *                                  therefore blocking call is aborted. The port of the
     *                                  slave component can now be either unchanged, disconnected
     *                                  or properly connected to the specified service provider.
     *    - SMART_UNKNOWNCOMPONENT    : the addressed slave component is either not known or does
     *                                  not provide a wiring service. The requested connection can
     *                                  not be established.
     *    - SMART_UNKNOWNPORT         : the specified port name is not known at the slave component.
     *                                  The requested connection can not be established.
     *    - SMART_SERVICEUNAVAILABLE  : the slave component can not connect to the specified service
     *                                  of the specified server since the server and/or the service
     *                                  to be connected to from the slave component is not available.
     *    - SMART_INCOMPATIBLESERVICE : The service behind the specified port is not compatible with
     *                                  the service to be connected to. The requested connection can
     *                                  not be established.
     *    - SMART_ERROR_COMMUNICATION : communication problems either while connecting to the slave
     *                                  or at the slave component while it tried to establish the
     *                                  requested connection to the requested service provider. The
     *                                  port of the slave component can now either be unchanged,
     *                                  disconnected or already properly connected to the specified
     *                                  service provider.
     *    - SMART_ERROR               : something went completely wrong, see <I>SMART_ERROR_COMMUNICATION</I>
     */
    Smart::StatusCode connect(const std::string & slavecmpt,const std::string & slaveprt, const std::string & servercmpt,const std::string & serversvc);

    /** Blocking call to disconnect port of a slave component from a service of a server.
     *
     *  Disconnect the port <I>slaveprt</I> of the slave component <I>slavecmpt</I> from
     *  any server connection.
     *
     *  @warning
     *    This method must not be called concurrently from several threads nor concurrently
     *    with the connect() method.
     *
     *  @param slavecmpt   name of the slave component
     *  @param slaveprt    name of the port to be disconnected
     *
     *  @return status code:
     *    - SMART_OK                  : everything is ok and the port is disconnected
     *    - SMART_CANCELLED           : blocking is not allowed or is not allowed anymore and therefore
     *                                  blocking call is aborted. The port of the slave component now can
     *                                  either be unchanged or disconnected.
     *    - SMART_UNKNOWNCOMPONENT    : the addressed slave component is either not known or does
     *                                  not provide a wiring service. The requested disconnect is
     *                                  not performed.
     *    - SMART_UNKNOWNPORT         : the specified port name is not known at the slave component.
     *                                  The requested disconnect is not performed.
     *    - SMART_ERROR_COMMUNICATION : communication problems either while connecting to the slave
     *                                  or at the slave component while it tried to perform the disconnect.
     *                                  The port of the slave component can now either be unchanged or
     *                                  disconnected.
     *    - SMART_ERROR               : something went completely wrong, see <I>SMART_ERROR_COMMUNICATION</I>
     */
    Smart::StatusCode disconnect(const std::string & slavecmpt,const std::string & slaveprt);
  };
}






/////////////////////////////////////////////////////////////////////////
//
// slave part
//
/////////////////////////////////////////////////////////////////////////

namespace SmartACE {

  /// forward declaration
  class WiringSlave;

  // forward declaration
  template<class R, class A> class QueryServer;

  /** @internal
   *  Handler for slave part of wiring pattern.
   *
   *  The wiring handler is called by the internally used query pattern
   *  and connects / disconnects a port with a server.
   */
  class WiringHandler : public QueryServerHandler<SmartCommWiring,SmartCommWiring> {
  private:
    /// used to access the WiringSlave from the handler
    WiringSlave * wiringslave;

    /// default constructor
    WiringHandler() ;

  public:
    /** Constructor.
     *
     * @param slave  <I>WiringSlave</I> needed to access it from the handler
     */
    WiringHandler(WiringSlave* slave);

    /// Destructor
    virtual ~WiringHandler();

    /// handle query method of query handler class
    virtual void handleQuery(const QueryId &id, const SmartCommWiring& request);
  };


  /** Slave part of wiring pattern.
   *
   *  The slave part of the wiring pattern is responsible for changing the
   *  port connections within the slave components. It manages the port list
   *  of a component.
   *
   *  The user simply needs exactly one instance of the wiring slave per
   *  component. The WiringSlave provides no user accessible member functions.
   *
   *  Demonstrated in <a href="/drupal/?q=node/51#ninth-example">ninth example</a>
   */

   //  The interaction of the master / slave parts of the wiring pattern is
   //  based on the query communication pattern using the <I>SmartCommWiring</I>
   //  communication object. The service is named <I>wiring</I>.
   //
   //  The port list contains the port name together with pointers to static
   //  internalConnect() and internalDisconnect() methods of the client objects.
   //  Those pointers are set when a client is added to the set of ports. They
   //  are used to call the connect() / disconnect() methods of the client objects
   //  when wiring changes have to be performed.

  class WiringSlave {
	  friend class WiringHandler;
  private:
    typedef struct PortList {
      std::string portname;
      void*       tPtr;                                                 // voided <this> pointer of client object
      Smart::StatusCode  (*cPtr)(void*,const std::string&,const std::string&); // ptr to internalConnect method
      Smart::StatusCode  (*dPtr)(void*);                                       // ptr to internalDisconnect method
      PortList    *next;
    }PortList;

    PortList   *ports;

    SmartMutex mutex;

    /// management class of the component
    SmartComponent *component;

    ///
    WiringHandler * handler;

	/// Decorator for WiringHandler (important with ACE implementation)
	ThreadQueueQueryHandler<SmartCommWiring,SmartCommWiring> * threadHandler;

	/// query server part
    QueryServer<SmartCommWiring,SmartCommWiring> * wiring;

    ///
    WiringSlave() ;

  public:
    /** constructor.
     *
     *  (Currently exception not thrown)
     */
	  WiringSlave(SmartComponent* component, std::string slaveaddress="");

    /** destructor.
     *
     */
    virtual ~WiringSlave();

    /**
     *
     */
    SmartCommWiring handleWiring(const SmartCommWiring& request);

    /** @internal Add client to the set of ports.
     *
     *  @param port  name of the port for the added client
     *  @param tPtr  this pointer of client object (returned to static method)
     *  @param cPtr  address of the internalConnect method of the client object
     *  @param dPtr  address of the internalDisconnect method of the client object
     *
     *  @return status code:
     *    - SMART_OK               : everything is ok and client is added as new port. It can
     *                               now be wired from outside. The connection of the client
     *                               is not changed when being added to the set of ports.
     *    - SMART_PORTALREADYUSED  : client not added since the portname is already in use.
     *    - SMART_ERROR            : something went wrong
     */
    Smart::StatusCode add(const std::string& port, void* tPtr, Smart::StatusCode (*cPtr)(void*,const std::string&,const std::string&), Smart::StatusCode (*dPtr)(void*));

    /** @internal Remove port.
     *
     *  @param port  name of the port to be removed
     *
     *  @return status code:
     *    - SMART_OK                : everything is ok and the client is not wireable from outside
     *                                anymore. The connection of the client is not changed when
     *                                being removed from the set of ports.
     *    - SMART_UNKNOWNPORT       : no port with the specified name available.
     *    - SMART_ERROR             : something went wrong.
     */
    Smart::StatusCode remove(const std::string& port);
  };

}

#endif    // _SMARTWIRING_HH

