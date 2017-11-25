// --------------------------------------------------------------------------
//
//  Copyright (C) 2012 Alex Lotz
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

#include "smartReactorTask.hh"

namespace SmartACE {

ReactorTask::ReactorTask(Smart::IComponent *component)
:	Smart::ITask(component)
,	Task(component)
,  mutex()
,  reactor()
,  useCustomEventsMode(false)
,  reactorIdleTime()
,  reactorOwnerThread(0)
,  reactorIsRunning(false)
,  reactorInitialized(false)
,  sched_params(ACE_SCHED_OTHER, ACE_THR_PRI_OTHER_DEF)
,  cpuAffinity(-1)
{
   ACE_TRACE("ReactorTask::ReactorTask");
   initReactor();
}

ReactorTask::~ReactorTask()
{
   ACE_TRACE("ReactorTask::~ReactorTask");
   finiReactor();
}

void ReactorTask::initReactor(size_t max_number_of_handles)
{
   ACE_TRACE("ReactorTask::initReactor");
   ACE_Guard<ACE_Thread_Mutex> g(mutex);
   if (!reactorInitialized) {
      // open reinitializes thread ownership
      reactor.open(max_number_of_handles);
      reactor.owner(&reactorOwnerThread); // get the id of the current thread owning the reactor
      reactorInitialized = true;
   }
}

void ReactorTask::finiReactor()
{
   ACE_TRACE("ReactorTask::finiReactor");
   ACE_Guard<ACE_Thread_Mutex> g(mutex);
   if (reactorInitialized) {
      reactor.end_reactor_event_loop();
      reactor.close();
      reactorInitialized = false;
   }
}

void ReactorTask::activateCustomEventsMode(const ACE_Time_Value &idleTime)
{
   ACE_TRACE("ReactorTask::activateCustomEventsMode");
   ACE_Guard<ACE_Thread_Mutex> g(mutex);
   useCustomEventsMode = true;
   reactorIdleTime = idleTime;
   if (reactorIsRunning) {
      g.release();
      // restart reactor to activate new mode
      stopReactor();
      startReactor();
   }
}

void ReactorTask::deactivateCustomEventsMode()
{
   ACE_TRACE("ReactorTask::deactivateCustomEventsMode");
   ACE_Guard<ACE_Thread_Mutex> g(mutex);
   useCustomEventsMode = false;
   if (reactorIsRunning) {
      g.release();
      // restart reactor to activate default mode
      stopReactor();
      startReactor();
   }
}

int ReactorTask::svc(void)
{
   ACE_TRACE("ReactorTask::svc");
   bool useCustomEvents = false;

   { // guard scope
      ACE_Guard<ACE_Thread_Mutex> g(mutex);

      if(sched_params.policy() != ACE_SCHED_OTHER || sched_params.priority() != ACE_THR_PRI_OTHER_DEF) {
    	  this->setSchedParams(sched_params);
      }
      if(cpuAffinity != -1) {
    	  this->setCpuAffinity(cpuAffinity);
      }

      // change the thread-ownership of the reactor to this thread (so the events can be handled from within this thread)
      reactorOwnerThread = ACE_Thread_Manager::instance()->thr_self();
      reactor.owner(reactorOwnerThread); // set the current thread as the new owner of the reactor
      // reset is necessary if reactor was stopped before with the call "end_reactor_event_loop"
      reactor.reset_reactor_event_loop();
      reactorIsRunning = true;
      useCustomEvents = useCustomEventsMode;
   }

   if (useCustomEvents) {
      bool done = false;
      while (!done) {
         // check if reactor is signaled to shutdown -> break up the while loop
         if (reactor.reactor_event_loop_done())
            break;

         // check if new events are available to be handled.
         // Idle time specifies the maximum time the method
         // work_pending will wait till new events arise.
         // If reactorIdleTime is set to zero, then this method does
         // not block at all, but makes a brief check only.
         if (reactor.work_pending(reactorIdleTime)) {
            reactor.handle_events();
         }

         // now call the call-back to handle further user events
         if (this->customEventsCallback() != 0)
            done = true;
      } // end while
   } else {
      // if no custom events are to be handled, use directly the rector event loop
      reactor.run_reactor_event_loop();
   }
   // reactor stopped
   reactorIsRunning = false;
   return 0;
}

int ReactorTask::customEventsCallback()
{
   ACE_TRACE("ReactorTask::customEventsCallback");
   // do nothing in default implementation
   return 0;
}

int ReactorTask::startReactor(const ACE_Sched_Params &sched_params, const int &cpuAffinity)
{
	this->sched_params = sched_params;
	this->cpuAffinity = cpuAffinity;
	return startReactor();
}
int ReactorTask::startReactor()
{
   ACE_TRACE("ReactorTask::startReactor");
   if (!reactorInitialized)
      return -1;

   // start internal thread
   return this->start();
}
bool ReactorTask::isReactorRunning() const
{
   ACE_TRACE("ReactorTask::isReactorRunning");
   ACE_Guard<ACE_Thread_Mutex> g(mutex);
   return reactorIsRunning;
}
int ReactorTask::stopReactor(const bool &waitForCompletion)
{
   ACE_TRACE("ReactorTask::stopReactor");

   if (!reactorInitialized)
      return -1;

   // end reactor's event loop (should cause the thread to close its internal loop)
   reactor.end_reactor_event_loop();

   if (waitForCompletion) {
      // wait on internal thread to close
      ACE_Thread_Manager::instance()->wait_task(this);
   }
   return 0;
}

ACE_thread_t ReactorTask::getReactorOwnerThread() const
{
   ACE_TRACE("ReactorTask::getReactorOwnerThread");
   ACE_Guard<ACE_Thread_Mutex> g(mutex);
   return reactorOwnerThread;
}

} /* namespace CHS */
