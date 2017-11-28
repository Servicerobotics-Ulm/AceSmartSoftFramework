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

#ifndef REACTORTASK_H_
#define REACTORTASK_H_

#include "smartTask.hh"

#include <ace/Reactor.h>

namespace SmartACE {

class ReactorTask: public Task
{
public:
   ReactorTask(Smart::IComponent *component);
   virtual ~ReactorTask();

   // this methods allows to activate/deactivate custom event handling mode (idle-time zero means non-blocking)
   virtual void activateCustomEventsMode(const ACE_Time_Value &idleTime = ACE_Time_Value::zero);
   virtual void deactivateCustomEventsMode();

   // methods, that create/destroy internal Reactor (including memory allocation/release)
   void initReactor(size_t max_number_of_handles = ACE::max_handles()); // TODO: check if the max_handles is a reasonable default
   void finiReactor();

   // start/stop internal thread
   int startReactor();
   int startReactor(const ACE_Sched_Params &sched_params, const int &cpuAffinity=-1);
   bool isReactorRunning() const;
   int stopReactor(const bool &waitForCompletion = true);

   inline ACE_Reactor * getImpl()
   {
      return &reactor;
   }
   ACE_thread_t getReactorOwnerThread() const;

protected:
   // mutable allows to call the mutex from within const methods
   mutable ACE_Thread_Mutex mutex;

   // the Reactor instance
   ACE_Reactor reactor;

   // members used for custom events mode
   bool useCustomEventsMode;
   ACE_Time_Value reactorIdleTime;

   // internal thread that runs the reactor-event-loop
   int task_execution();

   // use this call-back in derived classes to handle further events besides those of Reactor
   virtual int customEventsCallback();

	virtual void on_shutdown();

private:
   ACE_thread_t reactorOwnerThread;
   bool reactorIsRunning;
   bool reactorInitialized;

   ACE_Sched_Params sched_params;
   int cpuAffinity;

   // don't copy the reactor (does not make sense)
   ReactorTask(const ReactorTask & reactor);
   ReactorTask& operator=(const ReactorTask & reactor);
};

} /* namespace SmartACE */
#endif /* REACTORTASK_H_ */
