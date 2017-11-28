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

#include "smartComponent.hh"

#include "smartConfig.hh"

#include "smartState.hh"

// used to initialize the naming service as a dynamic service
#include <ace/Service_Config.h>

////////////////////////////////////////////////////////////
//
// functions for component
//
////////////////////////////////////////////////////////////

//
// constructor
//
SmartACE::SmartComponent::SmartComponent( const std::string &name,
                                     int argc,
                                     char **argv,
                                     SmartACE::ReactorTask* reactor,
									 const ACE_Sched_Params &sched_params,
                                     const char *compileDate,
                                     const char *compileTime  )
   :  Smart::IComponent(name)
   ,  srvInitDiscHandler(this)
   ,  shutdownWatchdog(this)
   ,  mutex()
   ,  componentIsRunning(false)
   ,  runtimeCondVar(mutex)
   ,  timerThread()
{
  // condition list handling
  condList    = 0;
  cntCondList = 0;
  condCancel  = false;
  msgCnt = 0;
  hndCnt = 0;

  listCVwithMemory    = 0;
  listCVwithoutMemory = 0;
  listMonitor = 0;
  first_call_of_handle_signal = true;
  state = NULL;
  statusComponentBlocking = true;
  componentName = name;
  
  firstCallOfCleanupMethod = true;

  if(reactor == NULL) {
     ownReactor = true;
     reactorTask = new ReactorTask(this);
  } else {
     ownReactor = false;
     reactorTask = reactor;
  }

  SMARTSOFT_PRINT( ACE_TEXT("Used ACE Version %u.%u.%u\n\n"),
     ACE::major_version(), ACE::minor_version(), ACE::beta_version() 
     );

  SMARTSOFT_PRINT( ACE_TEXT(SMARTSOFT_VERSION_STRING) ACE_TEXT("\n\n") );


	if(SmartACE::NAMING::instance()->init(argc, argv) != 0)
	{
	   //ACE_ERROR((LM_ERROR, ACE_TEXT("naming_service: Service initilization failed!\n") ));
	   throw std::logic_error( "<NamingService> Service initialization failed!\nPossible causes could be:\n-> Erroronous configuration.\n-> naming_service not reachable.\n" );
	} else {
	   SMARTSOFT_PRINT(ACE_TEXT("<NamingService> initialized successfully!\n") );
	}


  // start our timer thread.
  if (timerThread.startTimerThread(sched_params, 0) == -1) {
    throw std::logic_error("<SmartComponent> Could not create timerThread");
  }

  // set this class as default handler for SIGPIPE, SIGTERM and SIGINT signals
  reactorTask->getImpl()->register_handler(SIGPIPE, this);
  reactorTask->getImpl()->register_handler(SIGTERM, this);
  reactorTask->getImpl()->register_handler(SIGINT, this);

  // start internal reactor thread (must be done before component->run call)
  reactorTask->startReactor(sched_params, 0);

  ACE_DEBUG((LM_DEBUG, ACE_TEXT("\n")));
  SMARTSOFT_PRINT(ACE_TEXT("Component %s started (compiled on %s %s)\n\n"), name.c_str(), compileDate, compileTime);
  //std::cout << "\n<SmartComponent> " << name << " started\ncompiled: " << compileDate << " " << compileTime << "\n\n";
}


//
// destructor
//
SmartACE::SmartComponent::~SmartComponent( void )
{
   if(ownReactor) delete reactorTask;
}

//
// default signal handler
//
int SmartACE::SmartComponent::handle_signal (int signum, siginfo_t *, ucontext_t *)
{
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("\n")));
  SMARTSOFT_PRINT(ACE_TEXT("%s Signal %d received.\n"), componentName.c_str(), signum);

  if (signum == SIGINT || signum == SIGTERM) {
    // 1) Make shure the signal is handeled only once
    if(!first_call_of_handle_signal) return 0;

    first_call_of_handle_signal = false;

    if( !componentIsRunning ) {
       // component is not yet ready (prob. due to pending implizit connections)
       SMARTSOFT_PRINT(ACE_TEXT("%s is not yet fully initilized -> clean up resources directly within signal-handler and exit(0)"), componentName.c_str());
       if(getStateSlave() != NULL) {
          // state pattern initialized -> set shutdown state and close pattern immediatelly
          getStateSlave()->setWaitState("Shutdown");
          getStateSlave()->shutdown(); // shutdown the pattern itself
       }
       this->signalSmartTasksToStop();
       this->cleanUpInternalResources();
       exit(0);
    }

    if(getStateSlave() != NULL) {
       // state pattern initialized, thus use shutdown state
       getStateSlave()->setWaitState("Shutdown");
    } else {
       SmartGuard guard(mutex);
       runtimeCondVar.broadcast();
    }
  } else if(signum == SIGPIPE) {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("SmartComponent::handle_signal: SIGPIPE - do nothing, it is handled inside of ServiceHandler implementation.\n")));
  }
  return 0;
};

//
// starts the communication stuff
//
Smart::StatusCode SmartACE::SmartComponent::run( void )
{
   Smart::StatusCode result = Smart::SMART_OK;

   // open thread for managing all server-initiated-disconnects in this component
   srvInitDiscHandler.start();

   // if meanwile strg+c was called -> return immediatelly, without waiting on substate or reactor
   if(!first_call_of_handle_signal) return Smart::SMART_ERROR;
   
   componentIsRunning = true;

   ///////////////////////////////////////////////////////////
   //                                                       //
   //  component is now fully initialized and running.      //
   //  (untill the component is commanded to shutdown...)   //
   //                                                       //
   ///////////////////////////////////////////////////////////
   if(this->state != NULL)
   {
      // block this thread till component is commanded to shut down...
      getStateSlave()->acquire("shutdown");
         // the stateSlave is safe to shut down because there are
         // no further state changes possible out of the "Shutdown" MainState
         getStateSlave()->shutdown();
      getStateSlave()->release("shutdown");
   } else {
     // block this thread until component is commaned to shut down...
     this->waitOnRuntimeCondVar();
   }
   ///////////////////////////////////////////////////////////
   //                                                       //
   //  Component is now in proces of destruction            //
   //  (shutdown and clean up resources...)                 //
   //                                                       //
   ///////////////////////////////////////////////////////////
   this->signalSmartTasksToStop();
   this->cleanUpInternalResources();

   // print the very last message of this component
   ACE_DEBUG((LM_DEBUG, ACE_TEXT("\n")));
   SMARTSOFT_PRINT(ACE_TEXT("Component '%s' FINISHED - BYE!!!\n"), componentName.c_str() );

   return result;
}

void SmartACE::SmartComponent::waitOnRuntimeCondVar() 
{
   SmartGuard guard(mutex);
   runtimeCondVar.wait();
}

void SmartACE::SmartComponent::signalSmartTasksToStop(const std::chrono::steady_clock::duration &watchdogTime) {
   // 1) start the shutdown watchdog
   shutdownWatchdog.start(watchdogTime);

   // 2) disable blocking waits in ports of this component
   this->blocking(false);

//   // 3) signal all component's ManagedTasks to stop (cooperativelly)
//   ACE_Thread_Manager::instance()->cancel_task(&baseTask);
//
//   // 4) wait till all component's ManagedTasks are stopped or a timeout occurs...
//   ACE_Thread_Manager::instance()->wait_task(&baseTask);
//
//   // 5) wait till all other ManagedTasks are stopped
//   ACE_Thread_Manager::instance()->cancel_task(BaseTaskDummy::instance());
//   ACE_Thread_Manager::instance()->wait_task(BaseTaskDummy::instance());

   this->notify_sutdown();

   // 6) all necessary threads are stopped -> no need for the watchdog any more
   shutdownWatchdog.stop();

   // 7) the typical next steps after this method are:
   //    - destroy all communication ports associated to this component
   //    - thereby the destructors call either disconnect or server-initiated disconnect
   //    - in doing so the communication pattern should not infinitelly block (due to some pending connections)
   //    therefore, all the monitors are now configured to time out after one second 
   this->setTimedModeForAllSmartMonitors(std::chrono::seconds(1));
}

void SmartACE::SmartComponent::cleanUpInternalResources()
{
   if(firstCallOfCleanupMethod) 
   {
      // make sure that this method is only called once
      firstCallOfCleanupMethod = false;

      // 1) stop component's internal timer thread
      timerThread.shutdownTimer();

      // 2) stop component's internal SID management thread
      srvInitDiscHandler.stop_mqueue();
      srvInitDiscHandler.stop();

      // 3) finally stop the reactor's event-loop 
      // (no event-handling is possible from now on)
      reactorTask->stopReactor();

      // 4) since reactor is down we can safely clean up naming service entries
      SmartACE::NSKeyType key;
      key.names[SmartACE::NSKeyType::COMP_NAME] = componentName.c_str();
      SmartACE::NAMING::instance()->unbindEntriesForMatchingPattern(key);

      // 5) naming context is now save to be closed
      SmartACE::NAMING::instance()->fini();
   }
}

Smart::StatusCode SmartACE::SmartComponent::blocking(const bool b) throw()
{
  Smart::StatusCode result = Smart::SMART_OK;
  SmartCVwithMemoryStruct    *ptr1;
  SmartCVwithoutMemoryStruct *ptr2;

  mutex.acquire();

  statusComponentBlocking = b;

  for (ptr1 = listCVwithMemory; ptr1 != 0; ptr1 = ptr1->next) {
    ptr1->cond->blockingComponent(statusComponentBlocking);
  }
  for (ptr2 = listCVwithoutMemory; ptr2 != 0; ptr2 = ptr2->next) {
    ptr2->cond->blockingComponent(statusComponentBlocking);
  }

  if (b == false) {
    //
    // startCancelCond()
    //
    CondListStruct            *ptr;
    CntCondListStruct         *pptr;

    // set flag
    condCancel = true;
    // signal all waiting conditions
    ptr = condList;
    while( ptr != 0 ) {
      (ptr->cond)->signalAll();
      ptr = ptr->next;
    }
    pptr = cntCondList;
    while( pptr != 0 ) {
      (pptr->cond)->signal();
      pptr = pptr->next;
    }
  }

  if (b == true) {
    //
    // stopCancelCond()
    //

    // reset flag
    condCancel = false;
  }

  mutex.release();

  return result;
}

void SmartACE::SmartComponent::setTimedModeForAllSmartMonitors(const std::chrono::steady_clock::duration &timeout)
{
   SmartMonitorStruct *ptr;

   mutex.acquire();
      for (ptr = listMonitor; ptr != 0; ptr = ptr->next) 
      {
         ptr->monitor->enableTimedMode(convertToAceTimeFrom(timeout));
      }
   mutex.release();
}


void SmartACE::SmartComponent::setStateSlave(SmartACE::StateSlave *state)
{
   SmartGuard guard(mutex);
   this->state = state;
}

SmartACE::StateSlave* SmartACE::SmartComponent::getStateSlave() const
{
   SmartGuard guard(mutex);
   return this->state;
}


/////////////////////////////////////////////////////////////////////////
// internal functions
/////////////////////////////////////////////////////////////////////////

Smart::StatusCode SmartACE::SmartComponent::waitForCond( SmartCondClass& cond )
{
  Smart::StatusCode  result = Smart::SMART_OK;
  CondListStruct   *ptr   = 0;
  CondListStruct   *hPtr  = 0;

  // test already canceling or insert in list
  mutex.acquire();
  if( condCancel ) {
    result = Smart::SMART_CANCELLED;
  } else {
    ptr = new CondListStruct;
    ptr->cond = &cond;
    ptr->next = condList;
    condList = ptr;
  }
  mutex.release();

  if( result != Smart::SMART_OK ) return result;

  // wait
  cond.wait();

  mutex.acquire();
  // test result
  if( condCancel ) result = Smart::SMART_CANCELLED;

  // delete from list
  if( ptr == condList ) {
    condList = ptr->next;
    delete ptr;
  } else {
    hPtr = condList;
    while( hPtr->next != ptr ) hPtr = hPtr->next;
    hPtr->next = hPtr->next->next;
    delete ptr;
  }
  mutex.release();

  return result;
}

Smart::StatusCode SmartACE::SmartComponent::waitForCond( SmartCntCondClass& cond )
{
  Smart::StatusCode   result = Smart::SMART_OK;
  CntCondListStruct *ptr   = 0;
  CntCondListStruct *hPtr  = 0;

  // test already canceling or insert in list
  mutex.acquire();
  if( condCancel ) {
    result = Smart::SMART_CANCELLED;
  } else {
    ptr = new CntCondListStruct;
    ptr->cond = &cond;
    ptr->next = cntCondList;
    cntCondList = ptr;
  }
  mutex.release();

  if( result != Smart::SMART_OK ) return result;

  // wait
  cond.wait();

  mutex.acquire();
  // test result
  if( condCancel ) result = Smart::SMART_CANCELLED;

  // delete from list
  if( ptr == cntCondList ) {
    cntCondList = ptr->next;
    delete ptr;
  } else {
    hPtr = cntCondList;
    while( hPtr->next != ptr ) hPtr = hPtr->next;
    hPtr->next = hPtr->next->next;
    delete ptr;
  }
  mutex.release();

  return result;
}


Smart::StatusCode SmartACE::SmartComponent::addCV(SmartCVwithMemory* cond) throw()
{
  Smart::StatusCode result = Smart::SMART_OK;
  SmartCVwithMemoryStruct *cptr;

  mutex.acquire();

  cptr             = new SmartCVwithMemoryStruct;
  cptr->cond       = cond;
  cptr->next       = listCVwithMemory;
  listCVwithMemory = cptr;

  cptr->cond->blockingComponent(statusComponentBlocking);

  mutex.release();

  return result;
}

Smart::StatusCode SmartACE::SmartComponent::removeCV(SmartCVwithMemory* cond) throw()
{
  Smart::StatusCode result = Smart::SMART_OK;
  SmartCVwithMemoryStruct *cptr;
  SmartCVwithMemoryStruct *hcptr;

  mutex.acquire();

  // normally a condition variable is at most once in the list but you never know ...

  //
  // first remove entries from head as long as they match the condition variable
  //
  while ((listCVwithMemory != 0) && (listCVwithMemory->cond == cond)) {
    cptr = listCVwithMemory;
    listCVwithMemory = listCVwithMemory->next;
    delete cptr;
  }

  //
  // now search through the rest of the list which might be empty already. If the
  // length is greater than zero, than the first element never matches and never
  // gets deleted
  //
  cptr = listCVwithMemory;
  if (cptr == 0) {
    // list contains no element anymore => ready
  } else {
    // list can contain one or more elements with the first element matching never
    while (cptr->next != 0) {
      if (cptr->next->cond == cond) {
        hcptr      = cptr->next;
        cptr->next = cptr->next->next;
        delete hcptr;
      } else {
        cptr = cptr->next;
      }
    }
  }

  mutex.release();

  return result;
}

Smart::StatusCode SmartACE::SmartComponent::addCV(SmartCVwithoutMemory* cond) throw()
{
  Smart::StatusCode result = Smart::SMART_OK;
  SmartCVwithoutMemoryStruct *cptr;

  mutex.acquire();

  cptr                = new SmartCVwithoutMemoryStruct;
  cptr->cond          = cond;
  cptr->next          = listCVwithoutMemory;
  listCVwithoutMemory = cptr;

  cptr->cond->blockingComponent(statusComponentBlocking);

  mutex.release();

  return result;
}

Smart::StatusCode SmartACE::SmartComponent::removeCV(SmartCVwithoutMemory* cond) throw()
{
  Smart::StatusCode result = Smart::SMART_OK;
  SmartCVwithoutMemoryStruct *cptr;
  SmartCVwithoutMemoryStruct *hcptr;

  mutex.acquire();

  // normally a condition variable is at most once in the list but you never know ...

  //
  // first remove entries from head as long as they match the condition variable
  //
  while ((listCVwithoutMemory != 0) && (listCVwithoutMemory->cond == cond)) {
    cptr = listCVwithoutMemory;
    listCVwithoutMemory = listCVwithoutMemory->next;
    delete cptr;
  }

  //
  // now search through the rest of the list which might be empty already. If the
  // length is greater than zero, than the first element never matches and never
  // gets deleted
  //
  cptr = listCVwithoutMemory;
  if (cptr == 0) {
    // list contains no element anymore => ready
  } else {
    // list can contain one or more elements with the first element matching never
    while (cptr->next != 0) {
      if (cptr->next->cond == cond) {
        hcptr      = cptr->next;
        cptr->next = cptr->next->next;
        delete hcptr;
      } else {
        cptr = cptr->next;
      }
    }
  }

  mutex.release();

  return result;
}

Smart::StatusCode SmartACE::SmartComponent::addSmartMonitor(SmartMonitor* monitor) throw()
{
  Smart::StatusCode result = Smart::SMART_OK;
  SmartMonitorStruct *cptr;

  mutex.acquire();

  cptr          = new SmartMonitorStruct;
  cptr->monitor = monitor;
  cptr->next    = listMonitor;
  listMonitor   = cptr;

  cptr->monitor->blocking(statusComponentBlocking);

  mutex.release();

  return result;
}

Smart::StatusCode SmartACE::SmartComponent::removeSmartMonitor(SmartMonitor* monitor) throw()
{
  Smart::StatusCode result = Smart::SMART_OK;
  SmartMonitorStruct *cptr;
  SmartMonitorStruct *hcptr;

  mutex.acquire();

  // normally a condition variable is at most once in the list but you never know ...

  //
  // first remove entries from head as long as they match the condition variable
  //
  while ((listMonitor != 0) && (listMonitor->monitor == monitor)) {
    cptr = listMonitor;
    listMonitor = listMonitor->next;
    delete cptr;
  }

  //
  // now search through the rest of the list which might be empty already. If the
  // length is greater than zero, than the first element never matches and never
  // gets deleted
  //
  cptr = listMonitor;
  if (cptr == 0) {
    // list contains no element anymore => ready
  } else {
    // list can contain one or more elements with the first element matching never
    while (cptr->next != 0) {
      if (cptr->next->monitor == monitor) {
        hcptr      = cptr->next;
        cptr->next = cptr->next->next;
        delete hcptr;
      } else {
        cptr = cptr->next;
      }
    }
  }

  mutex.release();

  return result;
}

void SmartACE::SmartComponent::enqueue(void *ptr,
   //<alexej date="2010-03-09">
      void (*internalServInitDisc)(void *,int),
      int cid)
   //</alexej>
{
   srvInitDiscHandler.enqueue(ptr, internalServInitDisc, cid);
}

void SmartACE::SmartComponent::sign_on(void *handle)
{
   srvInitDiscHandler.sign_on(handle);
}

void SmartACE::SmartComponent::sign_off(void *handle)
{
   srvInitDiscHandler.sign_off(handle);
}


/////////////////////////////////////////////////////////////////////////
//
// Shutdown Timer
//
/////////////////////////////////////////////////////////////////////////
SmartACE::ShutdownTimer::ShutdownTimer(SmartACE::SmartComponent *comp)
:  component(comp)
,  timer_id(0)
,  timer_started(false)
,  mutex()
{  
   //lthis = NULL;
}

void SmartACE::ShutdownTimer::start(const std::chrono::steady_clock::duration &timeout)
{
   SmartGuard guard(mutex);
   timer_id = component->getTimerManager()->scheduleTimer(this, timeout);
   timer_started = true;
}

void SmartACE::ShutdownTimer::timerExpired(const std::chrono::system_clock::time_point &abs_time)
{
   {  // begin guard scope 
      SmartGuard guard(mutex);

      if(timer_started)
      {
         SMARTSOFT_PRINT( ACE_TEXT("ShutdownTimer: Timeout occurred, all tasks and the component will be killed immediately...\n") );

         // 1) clean up name entries in naming service (if used)
         SmartACE::NSKeyType key;
         key.names[SmartACE::NSKeyType::COMP_NAME] = component->getName().c_str();
         SmartACE::NAMING::instance()->unbindEntriesForMatchingPattern(key);
         SmartACE:: NAMING::instance()->fini();

         // 2) end reactor loop
         component->getReactorTask()->stopReactor();
               
      }
   } // exit guard scope (release mutex)

   // 3) exit(0) is now safe to use
   exit(0);
}

void SmartACE::ShutdownTimer::stop()
{
   SmartGuard guard(mutex);
   if(timer_started)
   {
      timer_started = false;
      component->getTimerManager()->cancelTimer(timer_id);
   }
}
