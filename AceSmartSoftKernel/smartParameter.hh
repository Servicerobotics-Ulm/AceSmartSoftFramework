// --------------------------------------------------------------------------
//
//  Copyright (C) 2013 Christian Schlegel, Alex Lotz
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

#ifndef _SMARTPARAMETER_HH
#define _SMARTPARAMETER_HH

#include <cstdio>

#ifdef WIN32
	#pragma warning( disable : 4290 )
#endif


#include <cstdlib>
#include <cstring>
#include <cmath>

#include <string>

#include "smartComponent.hh"
#include "smartCommParameterRequest.hh"
#include "smartCommParameterResponse.hh"
#include "smartQuery.hh"

#include "smartProcessingPatterns.hh"

/////////////////////////////////////////////////////////////////////////
//
// master part
//
/////////////////////////////////////////////////////////////////////////

namespace SmartACE {
  /** Master part of parameter pattern.
   *
   *  Provides a generic port to send configuration sets to components in the system.
   *  Typically the master part is used within the sequencer. For each point in time
   *  there is a point-to-point connection between one Parameter Master and one Parameter Slave.
   *  In total one Parameter Master can send parameter-sets to several Parameter Slaves.
   *  There are three different types of parameters (which are encoded in the CommParameterRequest):
   *  - Simple parameters such as SETPOSE(x,y) CHANGEMAP("MapName"), ...
   *  - Triggers
   *  - COMMIT is a special kind of trigger that tells a component that a sequence of parameter
   *    sets is complete and the component can from now on use the consistent set of new parameters.
   *  <br>
   *
   *  TODO: a demonstration component
   */
  class ParameterMaster {
  private:
    SmartMutex      mutex;

    /// management class of the component
    SmartComponent *component;

    SmartACE::QueryClient<SmartACE::CommParameterRequest,SmartACE::CommParameterResponse> query_client;

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
    ParameterMaster(SmartComponent* component) throw(SmartACE::SmartError);

    /** Destructor.
     */
    virtual ~ParameterMaster() throw();

    /** Blocking call to send a parameter set to a component indicated by the comp_name.
     *
     *  Connect the ParameterMaster to the ParameterSlave indicated by the <I>comp_name</I>
     *  and the optional <I>service_name</I>. Then, send the parameter set stored in the
     *  <I>request</I> communication object. After that, await the answer which is written
     *  into the <I>response</I> communication object.
     *
     *  @param request      contains the parameter-set to transfer to the ParameterSlave
     *  @param response     contains the response from the ParameterSlave
     *  @param comp_name    name of the ParameterSlave component
     *  @param service_name (optional) name of the service in component <I>comp_name</I> to connect to
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
     *                                  not provide a Parameter service. The requested connection can
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
     Smart::StatusCode sendParameterWait(
    		const SmartACE::CommParameterRequest &request,
    		SmartACE::CommParameterResponse &response,
    		const std::string &comp_name, const std::string &service_name="param");


  };
}






/////////////////////////////////////////////////////////////////////////
//
// slave part
//
/////////////////////////////////////////////////////////////////////////

namespace SmartACE {

  /// forward declaration
  class ParameterSlave;

  class ParameterUpdateHandler {
  public:
	  ParameterUpdateHandler() {  }
	  virtual ~ParameterUpdateHandler() {  }
	  virtual SmartACE::CommParameterResponse handleParameter(const SmartACE::CommParameterRequest& request)=0;
  };


  /** Slave part of Parameter pattern.
   *
   *  The slave part of the Parameter pattern is responsible for changing the
   *  port connections within the slave components. It manages the port list
   *  of a component.
   *
   *  The user simply needs exactly one instance of the Parameter slave per
   *  component. The ParameterSlave provides no user accessible member functions.
   *
   *  Demonstrated in <a href="/drupal/?q=node/51#ninth-example">ninth example</a>
   */

   //  The interaction of the master / slave parts of the Parameter pattern is
   //  based on the query communication pattern using the <I>CommParameter</I>
   //  communication object. The service is named <I>Parameter</I>.
   //
   //  The port list contains the port name together with pointers to static
   //  internalConnect() and internalDisconnect() methods of the client objects.
   //  Those pointers are set when a client is added to the set of ports. They
   //  are used to call the connect() / disconnect() methods of the client objects
   //  when Parameter changes have to be performed.

  class ParameterSlave {
  private:
    SmartMutex mutex;

    /// management class of the component
    SmartComponent *component;

    class ParameterQueryHandler : public QueryServerHandler<SmartACE::CommParameterRequest,SmartACE::CommParameterResponse> {
    private:
      /// used to access the ParameterSlave from the handler
  	  ParameterUpdateHandler *param_handler;

    public:
      /** Constructor.
       *
       * @param slave  <I>ParameterSlave</I> needed to access it from the handler
       */
      ParameterQueryHandler(
    		  QueryServer<SmartACE::CommParameterRequest,SmartACE::CommParameterResponse>* server,
    		  ParameterUpdateHandler *param_handler) throw();

      /// Destructor
      virtual ~ParameterQueryHandler() throw();

      /// handle query method of query handler class
      void handleQuery(const QueryId &id, const SmartACE::CommParameterRequest & request) throw();
    };

    ///
    ParameterQueryHandler query_handler;


    /// Decorator for ParameterHandler (important with ACE implementation)
    ThreadQueueQueryHandler<SmartACE::CommParameterRequest,SmartACE::CommParameterResponse> thread_handler;

    /// query server part
    QueryServer<SmartACE::CommParameterRequest,SmartACE::CommParameterResponse> query_server;

  public:
    /** constructor.
     *
     *  (Currently exception not thrown)
     */
	  ParameterSlave(SmartComponent* comp, ParameterUpdateHandler *hnd, std::string slave_address="param") throw(SmartACE::SmartError);

    /** destructor.
     *
     */
    virtual ~ParameterSlave() throw();
  };

}

#endif    // _SMARTPARAMETER_HH

