// --------------------------------------------------------------------------
//
//  Copyright (C) 2002/2004/2008 Christian Schlegel
//                2009/2010 Alex Lotz
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

#ifndef _SMARTSTATE_HH
#define _SMARTSTATE_HH

#include <cstdio>

#include <cstdlib>
#include <cstring>
#include <cmath>

#include <string>
#include <vector>

#include "smartComponent.hh"
#include "smartCommState.hh"
#include "smartWiring.hh"
#include "smartQuery.hh"
#include "smartManagedTask.hh"


/////////////////////////////////////////////////////////////////////////
//
// client part
//
/////////////////////////////////////////////////////////////////////////

//
// client state class
//
namespace SmartACE {

  /** Master part of state pattern.
   *
   *  The configuration class provides priorized mutexes for protecting
   *  configurations. Normally, a complex robotic system requires a lot of
   *  different components which can not all run at the same time. Depending
   *  on the current task to be executed and depending on the available
   *  ressources, one has to be able to activate and deactivate components
   *  and activities within components. One the one hand deactivation should
   *  be done in such a way that the component can perform cleanup tasks.
   *  On the other hand a component must still be interruptable even if it
   *  is pending in a blocking call because the corresponding server has
   *  already been deactivated.
   *
   *  The configuration class therefore provides mechanisms for graceful
   *  activation and deactivation of sets of activities within components.
   *
   *  As is explained in the specification of the state pattern, one can
   *  activate one mainstate at a time. A mainstate can however comprise
   *  several substates which are all active at the same time. A master
   *  can only set mainstates whereas the slave can only acquire substates.
   *  This allows the simple management of sets of concurrent activities.
   *
   *  For a complete description see the printed documentation and further
   *  details in <a href="/drupal/?q=node/51#third-example">third example</a>.
   *
   */
  class StateMaster {
  private:
    /// mutexConnection protects critical sections from being executed in
    ///                 parallel. These are all related to changing connections
    ///                 while client/server interactions are active. The mutex
    ///                 avoids racing conditions when for example a subscribe
    ///                 and a disconnect are called in parallel.
    SmartRecursiveMutex mutexConnection;

    // management class of the component
    SmartComponent *component;

    //<alexej date="2009-10-27">
    /// Main Query port - handles all communication of state pattern
    QueryClient<SmartCommStateRequest, SmartCommStateResponse> state_proxy;
    //</alexej>

    /// 0/1 to indicate whether client is not connected / is connected to server
    int statusConnected;

    /// denotes the name of the port if client can be wired from other components
    std::string portname;

    /// Default constructor
    ///
    /// throws exception if someone tries to use this constructor
    ///   SMART_ERROR : this constructor is forbidden
    StateMaster() throw(SmartACE::SmartError);

  public:
    // Initialization
    /** Constructor (not wired with service provider and not exposed as port).
     *  add()/remove() and connect()/disconnect() can always be used to change
     *  the status of the instance. Instance is not connected to a service provider
     *  and is not exposed as port wireable from outside the component.
     *
     *  (Currently exception not thrown)
     *
     * @param component  management class of the component
     */
    StateMaster(SmartComponent* component) throw(SmartACE::SmartError);

    /// Destructor
    virtual ~StateMaster() throw();

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

    /** Blocking call to change the main state.
     *
     *  @param state
     *     - If "Deactivated" is specified as main state, every blocking
     *       query etc. is aborted with SMART_CANCELLED to reach the
     *       "neutral" state as fast as possible.
     *     - If "Neutral" is specified the neutral state will be
     *       reached without aborting pending communication
     *       calls.
     * @param server allows to define the slave for the state-change 
     * @param service is an optional parameter in case the slave has a different service name than "state" (mostly for backwards compatibility)
     *
     * @return status code
     *   - SMART_OK                  : everything is ok
     *   - SMART_UNKNOWNSTATE        : requested mainstate is unknown and
     *                                 therefore no state change has been
     *                                 performed
     *   - SMART_NOTACTIVATED        : state object of server component not yet
     *                                 activated
     *   - SMART_CANCELLED           : component has been requested to reach
     *                                 neutral state, therefore pending
     *                                 setWaitState has been aborted
     *   - SMART_ERROR_COMMUNICATION : communication problems
     *   - SMART_ERROR               : something went wrong
     */
    Smart::StatusCode setWaitState(const std::string& state, const std::string& server, const std::string& service="state") throw();

    /** Blocking call which returns the currently active main state.
     *
     *  Is only for convenience and is not necessary since the
     *  master always knows what has been configured.
     *
     * @param MainState is set to the current main state as returned by the server (slave)
     * @param server allows to define the slave from which the currently active mainstate is returned
     * @param service is an optional parameter in case the slave has a different service name than "state" (mostly for backwards compatibility)
     *
     * @return status code
     *   - SMART_OK                  : everything is ok
     *   - SMART_NOTACTIVATED        : state object of server component not
     *                                 yet activated
     *   - SMART_ERROR_COMMUNICATION : communication problems
     *   - SMART_ERROR               : something went wrong
     *   - (hint for experts:
     *         SMART_CANCELLED can not be returned since a component can
     *         always return its current state even if it is disabled.
     *         Therefore there is no need to abort this method call when
     *         a request to reach the neutral state is pending)
     */
    Smart::StatusCode getCurrentMainState(std::string& MainState, const std::string& server, const std::string& service="state") throw();

    /** Blocking call which returns a list of valid main states.
     *
     *  @param MainStates contain all valid main states of the server (slave)
     * @param server allows to define the slave from which all the mainstates are returned
     * @param service is an optional parameter in case the slave has a different service name than "state" (mostly for backwards compatibility)
     *
     *  @return status code
     *   - SMART_OK                  : everything is ok
     *   - SMART_NOTACTIVATED        : state object of server component not
     *                                 yet activated
     *   - SMART_ERROR_COMMUNICATION : communication problems
     *   - SMART_ERROR               : something went wrong
     *   - (hint for experts:
     *         SMART_CANCELLED can not be returned since a component can
     *         always return its list of mainstates even if it is disabled.
     *         Therefore there is no need to abort this method call when
     *         a request to reach the neutral state is pending)
     */
    Smart::StatusCode getAllMainStates(std::list<std::string>& MainStates, const std::string& server, const std::string& service="state") throw();

    /** Blocking call which returns a list of substates assigned to the
     *  specified main state.
     *
     *  @param MainState is the mainstate to which we want to get the list
     *                   of substates
     *  @param SubStates    is set to the returned list of substates
     * @param server allows to define the slave from which all the substates (for the given mainstate) are returned
     * @param service is an optional parameter in case the slave has a different service name than "state" (mostly for backwards compatibility)
     *
     *  @return status code
     *   - SMART_OK                  : everything is ok
     *   - SMART_NOTACTIVATED        : state object of server component not
     *                                 yet activated
     *   - SMART_UNKNOWNSTATE        : the main state asked for is unknown
     *   - SMART_ERROR_COMMUNICATION : communication problems
     *   - SMART_ERROR               : something went wrong
     *   - (hint for experts:
     *         SMART_CANCELLED can not be returned since a component can
     *         always return its list of mainstates even if it is disabled.
     *         Therefore there is no need to abort this method call when
     *         a request to reach the neutral state is pending)
     */
    Smart::StatusCode getSubStates(const std::string& MainState, std::list<std::string>& SubStates, const std::string& server, const std::string& service="state") throw();
  };
}


/////////////////////////////////////////////////////////////////////////
//
// server part
//
/////////////////////////////////////////////////////////////////////////

//
// forward declaration
//
namespace SmartACE {
  class StateSlave;
}

//
// separate thread needed by server to avoid blocking when state changes have to
// be delayed due to user locks
//
namespace SmartACE {
  class StateUpdateThread : public SmartACE::ManagedTask
  {
  private:
    StateSlave *stateServer;
    SmartSemaphore   *stateQueueSemaphore;

    bool is_running;
  public:
    StateUpdateThread(SmartACE::SmartComponent *component)
    :  SmartACE::ManagedTask(component)
    ,  stateServer(NULL)
    ,  stateQueueSemaphore(NULL)
    ,  is_running(false)
    {};
    ~StateUpdateThread() {};
    void init(StateSlave*, SmartSemaphore*);

    virtual int on_execute();

    void halt();
  };
}

//
// server state class
//
namespace SmartACE {

  /** Handler class for state changes at the slave
   *
   *  Used by StateSlave to notify the user about
   *  state changes.
   */
  class StateChangeHandler
  {
  public:
    virtual ~StateChangeHandler() {  };

    // Handler methods
    /** Called when a substate is entered
     *
     *  @param SubState name of newly entered substate
     */
    virtual void handleEnterState( const std::string& SubState ) throw() = 0;

    /** called when a substate is left
     *
     *  @param SubState name of deactivated substate
     */
    virtual void handleQuitState( const std::string& SubState ) throw() = 0;
  };

  /** @internal
   *  Handler for slave part of wiring pattern.
   *
   *  The wiring handler is called by the internally used query pattern
   *  and connects / disconnects a port with a server.
   */
  class StateSlaveHandler : public QueryServerHandler<SmartCommStateRequest, SmartCommStateResponse>
  {
  private:
    /// used to access the StateSlave from the handler
    StateSlave *stateSlave;

  public:
    /** Constructor.
     *
     * @param slave  <I>StateSlave</I> needed to access it from the handler
     */
    StateSlaveHandler(StateSlave* state) throw();

    /// Destructor
    virtual ~StateSlaveHandler() throw();

      virtual void handleQuery(const QueryId &id, const SmartCommStateRequest& request) throw();
  };

  /** Slave part of state pattern.
   *
   *  Demonstrated in <a href="/drupal/?q=node/51#third-example">third example</a>
   */
  class StateSlave {
     friend class SmartComponent;
     friend class StateSlaveHandler;

  private:

    typedef enum StateSlaveAction {
      SSA_UNDEFINED,
      SSA_CHANGE_STATE
    }StateSlaveAction;

    typedef struct SmartStateEntry {
      StateSlaveAction      action;
      std::string                 state;
      //<alexej date="2010-09-22">
      QueryServer<SmartCommStateRequest,SmartCommStateResponse> *server_proxy;
      QueryId                     qid;
      SmartConditionRecursiveMutex *cond;
      //</alexej>
    }SmartStateEntry;

    //
    // indicates the next action to be performed on a substate
    //
    typedef enum SmartStateAction {
      STATE_ACTION_ACTIVATE,
      STATE_ACTION_DEACTIVATE,
      STATE_ACTION_NONE
    }SmartStateAction;

    //
    // indicates the current mode of a substate
    //
    typedef enum SmartStateMode {
      STATE_ACTIVATED,
      STATE_DEACTIVATED
    }SmartStateMode;

    //
    // used to manage the states of a component
    //
    typedef struct SmartSubStateEntry {
      std::vector<std::string>  mainstates;
      std::string             name;
      //<alexej date="2010-09-08">
      //SmartCondClass        cond;
      SmartConditionRecursiveMutex *cond;
      //</alexej>
      int                     cnt;
      SmartStateAction        action;
      SmartStateMode          state;
    }SmartSubStateEntry;


    StateUpdateThread                    stateUpdateThread;
    SmartSemaphore*                      stateQueueSemaphore;
    ACE_Unbounded_Queue<SmartStateEntry> stateQueue;

    SmartStateEntry  desiredState;

    //<alexej date="2010-09-17">
    // current MainState
    std::string      currentState;

    // initial MainState
    std::string      initialState;
    //</alexej>

    mutable SmartRecursiveMutex  mutex;

    ///
    std::list<SmartSubStateEntry> stateList;

    // set-up initial state-configuration
    void setUpInitialStateList();

    /// if flag is true => no more states can be defined
    bool running;

    // management class of the service
    SmartComponent *component;

    /// service name
    std::string service;

    /// handler for state changes
    StateChangeHandler * changeHandler;

    /// QueryHandler handles incomming state-requests from StateClient
    StateSlaveHandler *query_handler;

    /// QueryServer as main port of StateServer
    QueryServer<SmartCommStateRequest, SmartCommStateResponse> *query_server;

    /// @internal stop internal activity (used from SmartComponent during shutdown)
    void shutdown();

    /// private handler functions
    static void hndSetMainState(void*, QueryServer<SmartCommStateRequest,SmartCommStateResponse> *server, const QueryId &qid, const std::string&);
    static Smart::StatusCode hndGetCurrentState(void*, std::string &);
    static Smart::StatusCode hndGetMainStates(void*, std::vector<std::string>&);
    static Smart::StatusCode hndGetSubStates(void*, const std::string&, std::vector<std::string>&);

    /// Default constructor
    StateSlave() throw(SmartError);

  public:
    // internal use only
    void updateState(void);
    void updateStateFromThread(void);

    // Initialization

    /** Constructor.
     *  @param component management class of the component
     *  @param hnd       notify this handle object when state changes occur
     */
    //StateSlave(SmartComponent* component, StateChangeHandler & hnd) throw(SmartError);

    /** Constructor.
     *  @param component management class of the component
     *  @param hnd       optional callback interface which is called on each state-change
     *  @param serviceName the name for the internal communication-port (as will be visible on component hull)
     */
    StateSlave(SmartComponent* component, StateChangeHandler * hnd = NULL, const std::string& serviceName = "state") throw(SmartError);

    /// Destructor
    virtual ~StateSlave() throw();

    // User interface

    /** Blocking call to change the current component MainState.
     *
     *  @param MainState
     *     - StateSlave is only allowed to change the components lifecycle
     *       MainState (Alive, Shutdown and FatalError). Other MainStates are
     *       only allowed to be changed from the remote StateMaster. 
     *
     * @return status code
     *   - SMART_OK                  : everything is ok
     *   - SMART_UNKNOWNSTATE        : requested mainstate is unknown and
     *                                 therefore no state change has been
     *                                 performed
     *   - SMART_NOTACTIVATED        : state object of server component not yet
     *                                 activated
     *   - SMART_CANCELLED           : component has been requested to reach
     *                                 neutral state, therefore pending
     *                                 setWaitState has been aborted
     *   - SMART_ERROR               : something went wrong
     */
    Smart::StatusCode setWaitState(const std::string & MainState);


    /** Add the SubState to the MainState.
     *
     *  As long as the state object within a component is not
     *  activated, one can configure the name of the main states
     *  and substates which are afterwards visible externally.
     *
     *  If one or both names are used the first time the
     *  corresponding states are created automatically.
     *
     *  @param MainState is the MainState (parent)
     *  @param SubState is the SubState to be assigned to the MainState
     *
     *  @return status code
     *   - SMART_OK                  : everything is ok
     *   - SMART_ACTIVATED           : configuration can not be changed
     *                                 anymore once state object
     *                                 has been activated
     *   - SMART_NOTALLOWED          : tried to assign substates to the
     *                                 neutral state or something similar
     *                                 which is not allowed
     *   - SMART_ERROR_COMMUNICATION : communication problems
     *   - SMART_ERROR               : something went wrong
     */
    Smart::StatusCode defineStates(const std::string& MainState, const std::string& SubState) throw();


    /** Set the first MainState which is automatically entered after the pattern switched into Alive state. 
     *
     *  Per default the MainState Neutral is entered after StateSlave entered Alive state. If this behavior is
     *  not wanted this method can be used to change the initial MainState (which must be one of the previously
     *  defined MainStates).
     *
     *  @param MainState is the initial MainState after reaching Alive state
     *
     *  @return status code
     *   - SMART_OK                  : everything OK
     *   - SMART_NOTALLOWED          : tried to assign substates to the
     *                                 neutral state or something similar
     *                                 which is not allowed
     *   - SMART_ERROR               : something went wrong
     */
    Smart::StatusCode setUpInitialState(const std::string & MainState="Neutral");


    /** Returns the currently active MainState. 
     *
     *  If the StateSlave is in progress of a state-change, the MainState to be entered is
     *  returned instead of the currently active MainState. This is more convenient for Monitoring
     *  state changes!
     *
     *  @return MainState string
     */
    std::string getCurrentMainState() const;

    /** Activation is necessary since otherwise no states can be set
     *  or acquired. No more state definitions are possible after
     *  activation.
     *
     *  @return status code
     *    - SMART_OK  : configuration has been activated and no more
     *                  state definitions possible
     */
    Smart::StatusCode activate() throw();

    /** Wait until specified substate is available and acquire it.
     *
     *  (hint: acquire(nonneutral) blocks until some main state is set.)
     *
     *  @param SubState is the name of the substate to be acquired.
     *
     *  @return status code
     *   - SMART_OK                  : everything is ok
     *   - SMART_UNKNOWNSTATE        : returns immediately if the requested
     *                                 state is unknown
     *   - SMART_NOTACTIVATED        : state object of component not yet
     *                                 activated
     *   - SMART_ERROR_COMMUNICATION : communication problems
     *   - SMART_ERROR               : something went wrong
     */
    Smart::StatusCode acquire(const std::string& SubState) throw();

    /** Acquire specified substate if available, otherwise return immediately.
     *
     * (hint: tryAcquire(nonneutral) can be used e.g. in a query
     *        handler to decide whether to process the query or to
     *        immediately return an appropriate status code indicating the
     *        components inactivity.)
     *
     *  @param SubState is the name of the substate to be acquired.
     *
     *  @return status code
     *   - SMART_OK                  : everything is ok
     *   - SMART_UNKNOWNSTATE        : returns immediately if the requested
     *                                 state is unknown
     *   - SMART_NOTACTIVATED        : state object of component not yet
     *                                 activated
     *   - SMART_ERROR_COMMUNICATION : communication problems
     *   - SMART_ERROR : something went wrong
     */
    Smart::StatusCode tryAcquire(const std::string& SubState) throw();

    /** Release specified substate.
     *
     *  @param SubState is the name of the substate to be released.
     *
     *  @return status code
     *   - SMART_OK                  : everything is ok
     *   - SMART_UNKNOWNSTATE        : returns immediately if the requested
     *                                 state is unknown
     *   - SMART_NOTACTIVATED        : state object of component not yet
     *                                 activated
     *   - SMART_ERROR_COMMUNICATION : communication problems
     *   - SMART_ERROR               : something went wrong
     */
    Smart::StatusCode release(const std::string& SubState) throw();
  };
}

#endif    // _SMARTSTATE_HH

