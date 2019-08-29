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

//<alexej date="2010-09-16">
// be aware that this include MUST be outside of ifndef statement (below)
//#include "smartState.hh"
//</alexej>

#ifndef _SMARTCOMPONENT_HH
#define _SMARTCOMPONENT_HH


#include <smartIComponent.h>


#include <string>
#include <stdexcept>

#include "middlewareMapping/smartTimerManagerThread.hh"
//<alexej date="2012-02-13">
  #include "smartNSAdapterACE.hh"
  #include "smartTask.hh"
  #include "smartOSMapping.hh"
  #include "smartReactorTask.hh"

  // helper classes
  #include "smartCondVar.hh"
  #include "smartAdministrativeMonitor.hh"
  #include "smartSIDhandler.hh"
  #include "smartTimerManagerThread.hh"
//</alexej>


namespace SmartACE {
   // forward declarations
   class SmartComponent;
   class StateSlave;
}


namespace SmartACE {
    /** @internal
     *  ShutdownTimer
     *
     *<alexej date="2009-07-30"/>
     * This class implements a watchdog timer, which kills the component (given in the constructor)
     * after a timeout. This helps to stop a component, even if the tasks of this component 
     * do not cooperate to shut-down and thus must be killed.
     */
    class ShutdownTimer : public SmartACE::TimerHandler {
    private:
       // pointer to the component to be controlled by this class
       SmartComponent *component;

       // timer variables
       long timer_id;
       bool timer_started;

       // mutex to prevent concurrent call of stop and timerExpired methods
       SmartMutex mutex;
    public:
       // default constructor with default timeout time of 2 seconds
       ShutdownTimer(SmartComponent *comp);
       virtual ~ShutdownTimer() {  }
       
       // start the timer
       void start(const std::chrono::steady_clock::duration &timeout=std::chrono::seconds(2));

       // stop timer
       void stop();
       
       // function called after a timeout occures
       virtual void timerExpired(const std::chrono::system_clock::time_point &abs_time, const void * arg);
    };
}



/////////////////////////////////////////////////////////////////////////
//
// SmartSoft component class
//
/////////////////////////////////////////////////////////////////////////

namespace SmartACE {
  /** Component management class
   *
   *  Every component needs exactly one component management class which
   *  wraps basic services of the SmartSoft framework. This infrastructure
   *  is for example needed by the communication patterns.
   *
   *  The main thread of a component is used by this class to run the
   *  SmartSoft framework. Therefore all user activities must be started
   *  before in separate threads. Further details can be found in the
   *  examples.
   *
   */
  class SmartComponent : public Smart::IComponent, public ACE_Event_Handler {
     //friend class StateSlave;
  protected:
    // Management Thread for server-initiated-disconnects
    SIDhandler srvInitDiscHandler;

    // Shutdown Timer
    ShutdownTimer shutdownWatchdog;

  private:
    //<alexej date="2009-07-01" descr="Get rid of typedef warnings">
    typedef struct CondListStruct {                  // old ...
      SmartCondClass        *cond;
      struct CondListStruct *next;
    }CondListStruct;
    typedef struct CntCondListStruct {               // old ...
      SmartCntCondClass        *cond;
      struct CntCondListStruct *next;
    }CntCondListStruct;

    typedef struct SmartCVwithMemoryStruct {
      SmartCVwithMemory              *cond;
      struct SmartCVwithMemoryStruct *next;
    }SmartCVwithMemoryStruct;
    typedef struct SmartCVwithoutMemoryStruct {
      SmartCVwithoutMemory              *cond;
      struct SmartCVwithoutMemoryStruct *next;
    }SmartCVwithoutMemoryStruct;

    typedef struct SmartMonitorStruct {
      SmartMonitor              *monitor;
      struct SmartMonitorStruct *next;
    }SmartMonitorStruct;


    /// indicates whether blocking is not allowed in whole component
    bool statusComponentBlocking;

    CondListStruct             *condList;
    CntCondListStruct          *cntCondList;
    SmartCVwithMemoryStruct    *listCVwithMemory;
    SmartCVwithoutMemoryStruct *listCVwithoutMemory;

    SmartMonitorStruct *listMonitor;
    //</alexej>


    bool                       condCancel;
    int                        msgCnt;
    int                        hndCnt;
    std::string                componentName;
    mutable SmartMutex                 mutex;
    SmartConditionMutex runtimeCondVar;

  protected:
    //<alexej date="2011-02-24">
    StateSlave *state;
    bool componentIsRunning;
    bool firstCallOfCleanupMethod;
    void waitOnRuntimeCondVar();
    //</alexej>

    /** Signal handler method for SIGINT.
     *  The Default constructor registers this method to SIGINT.
     *
     *  This method calls exit(0) on SIGINT, resulting in a
     *  uncontrolled shutdown, no destructors will be called etc.
     *
     *  Overriding this method in a subclass is recommended to enable
     *  a clean shutdown after a Ctrl-C.
     *
     *  \warning this code runs in a signal handler
     *    (see http://www.cs.wustl.edu/~schmidt/signal-patterns.html for the approach)
     */
    virtual int handle_signal (int signum, siginfo_t *, ucontext_t *);

    /** @internal
     * variable is used to detect the first call of SIGINT handler
     * and to ignore all further calls
     */
    bool first_call_of_handle_signal;

  protected:
    // the components timer thread
    TimerManagerThread timerThread;

    // components Reactor
    bool ownReactor;
    SmartACE::ReactorTask* reactorTask;

    // signal all tasks (associated to this component) to stop cooperativelly
    // if one of the tasks do not cooperate within the watchdogTime,
    // the internal watchdog task gets active, cleans up as much component's
    // resources as possible and stops process with "exit(0)"
    virtual void signalSmartTasksToStop(const std::chrono::steady_clock::duration &watchdogTime=std::chrono::seconds(2));

    // clean up component's internal resources
    virtual void cleanUpInternalResources();

    // set timed mode for all registered administrative monitors (used for shutdown procedure)
    virtual void setTimedModeForAllSmartMonitors(const std::chrono::steady_clock::duration &timeout);

  public:
    // = Initialization

    /** Initializes the component.
     *
     *   @param serverName   unique name of the whole component, which is used by the clients to
     *                       address this server
     *   @param argc         # of command line arguments
     *   @param argv         array of command line arguments
     *   @param reactor      optionally a customized implementation of the Reactor can be used (otherwise component automatically initializes an own reactor)
     *   @param sched_params optional scheduling parameters used for the related internal reactor thread
     *   @param compileDate  compile date, just for debugging, don't touch
     *   @param compileTime  compile time, just for debugging, don't touch
     *
     */
    SmartComponent( const std::string &serverName,
                    int argc,
                    char ** argv,
                    SmartACE::ReactorTask* reactor = NULL,
                    const ACE_Sched_Params &sched_params = ACE_Sched_Params(ACE_SCHED_OTHER, ACE_THR_PRI_OTHER_DEF),
                    const char *compileDate = __DATE__ ,
                    const char *compileTime = __TIME__  );

    /** Destructor.
     */
    virtual ~SmartComponent();

    /** Runs the SmartSoft framework within a component which includes handling
     *  intercomponent communication etc. This method is called in the main()-routine
     *  of a component after all initializations including activation of user threads
     *  are finished.
     *
     *  @return status code
     *    - SMART_ERROR_UNKNOWN: unknown error (probably a corba problem)
     *    - SMART_OK: gracefully terminated
     */
    virtual Smart::StatusCode run(void);

	/** get timer-manager for registering timer-handlers
	 *
	 *  An instance of an ITimerManager is instantiated by an IComponent.
	 *  An ITimerManager allows activation of ITimerHandler instances
	 *  that are triggered (once or repeatedly) after a given time period.
	 *
	 *  @return a pointer to the ITimerManager
	 */
	inline virtual Smart::ITimerManager* getTimerManager() {
		return &timerThread;
	}

    /** Allow or abort and reject blocking calls in communication patterns of this component.
     *
     *  if blocking is set to false all blocking calls of all communication patterns
     *  of this component return with SMART_CANCELLED. This can be used to abort blocking
     *  calls of ALL communication patterns inside a component.
     *
     *  @param b (blocking)  true/false
     *
     *  @return status code
     *    - SMART_OK    : new mode set
     *    - SMART_ERROR : something went wrong
     */
    Smart::StatusCode blocking(const bool b);

    /** @internal
     * Returns the reference for the StateSlave if one is started otherwise NULL is returned.
     */
    void setStateSlave(StateSlave *state);

    /** @internal
     * Returns the reference for the StateSlave if one is started otherwise NULL is returned.
     */
    StateSlave* getStateSlave() const;

    /** @internal
     * Returns the reactor that is responsible for handling
     * all Events of this component and its communication patterns
     */
    inline SmartACE::ReactorTask* getReactorTask() { return reactorTask; }

    /** @internal
     * Returns the pointer of the shutdown task.
     */
    //inline ShutdownTask* getShutdownTask() { return &shutdown; }

    /** @internal
     *  called by the communication patterns to add their condition variable to the
     *  component. Used to set the cancelled state for all communication patterns of
     *  a component by a single call to the blocking() member function of this class.
     *  This is for example used by the state pattern.
     */
    Smart::StatusCode addCV(SmartCVwithMemory* cond);

    /** @internal
     *  see above
     */
    Smart::StatusCode removeCV(SmartCVwithMemory* cond);

    /** @internal
     *  see above
     */
    Smart::StatusCode addCV(SmartCVwithoutMemory* cond);

    /** @internal
     *  see above
     */
    Smart::StatusCode removeCV(SmartCVwithoutMemory* cond);

    /** @internal
     *  see above
     */
    Smart::StatusCode addSmartMonitor(SmartMonitor* monitor);

    /** @internal
     *  see above
     */
    Smart::StatusCode removeSmartMonitor(SmartMonitor* monitor);

    /** @internal
     *  provide interruptable wait on condition variable.
     */
    Smart::StatusCode waitForCond( SmartCondClass& cond );

    /** @internal
     *  provide interruptable wait on condition variable.
     */
    Smart::StatusCode waitForCond( SmartCntCondClass& cond );


    /** @internal
     * see thesis from Schlegel on page 195!
     */
    void enqueue(void*,
       //<alexej date="2010-03-09">
    		void (*internalServInitDisc)(void *,int),
         int cid);
       //</alexej>

    /** @internal
     * see thesis from Schlegel on page 195!
     */
    void sign_on(void *ptr);

    /** @internal
     * see thesis from Schlegel on page 195!
     */
    void sign_off(void *ptr);
  };
}

#endif

