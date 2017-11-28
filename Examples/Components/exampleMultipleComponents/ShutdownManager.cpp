// --------------------------------------------------------------------------
//
//  Copyright (C) 2010 Alex Lotz
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

#include "ShutdownManager.h"

///////////////////////////////////////////////////////////////////////////////////
// ShutdownManager
///////////////////////////////////////////////////////////////////////////////////
ShutdownManager::ShutdownManager()
:  shutdownTask()
{
   ACE_Reactor::instance()->register_handler(SIGINT, this);
}

ShutdownManager::~ShutdownManager()
{
   // nothing to do here
}

int ShutdownManager::manage_component(SmartACE::SmartComponent *comp)
{
   // Delegate management to the internal shutdownTask
   std::cout << "<ShutdownManager> got component " << comp->getComponentName() << " to manage." << std::endl;

   comp->getReactorTask()->getImpl()->register_handler(SIGINT, this);

   return shutdownTask.add_component(comp);
}

int ShutdownManager::handle_signal(int signum, siginfo_t *info, ucontext_t *cont)
{
   std::cout << "\n<ShutdownManager> Signal " << signum << " received" << std::endl;
   ACE_Reactor::instance()->schedule_timer(this, (void*)0, ACE_Time_Value(2));
   return shutdownTask.open();
}

int ShutdownManager::handle_timeout(const ACE_Time_Value &current_trime, const void *act)
{
   std::cout << "<ShutdownManager> TIMEOUT, end main reactor, BYE!" << std::endl;
   shutdownTask.stop();
   Smart::NAMING::instance()->fini();
   ACE_Reactor::instance()->end_reactor_event_loop();
   return 0;
}

int ShutdownManager::run()
{
   ACE_Reactor::instance()->run_reactor_event_loop();
   return 0;
}

///////////////////////////////////////////////////////////////////////////////////
// ShutdownTask
///////////////////////////////////////////////////////////////////////////////////
ShutdownManager::ShutdownTask::ShutdownTask()
{
   stopped = false;
}

int ShutdownManager::ShutdownTask::add_component(SmartACE::SmartComponent *comp)
{
   ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, mutex, -1);
   componnents.push_back(comp);
   return 0;
}

int ShutdownManager::ShutdownTask::open()
{
   return this->activate (THR_NEW_LWP);
}

int ShutdownManager::ShutdownTask::svc()
{
   std::list<SmartACE::SmartComponent*>::const_iterator it;
   SmartACE::SmartComponent *comp = NULL;

   mutex.acquire();
      for(it = componnents.begin(); it != componnents.end(); it++)
      {
         comp = *it;
         std::cout << "<ShutdownManager> Signal user-tasks to close for component " << comp->getComponentName() << std::endl;
         comp->blocking(false);
         ACE_Thread_Manager::instance()->cancel_task(comp->getBaseTaskDummy());
      }
  mutex.release();

  for(it = componnents.begin(); it != componnents.end(); it++)
  {
     comp = *it;
     std::cout << "<ShutdownManager> Wait on user-tasks from component " << comp->getComponentName() << " to be closed." << std::endl;
     ACE_Thread_Manager::instance()->wait_task(comp->getBaseTaskDummy());
  }


  mutex.acquire();
     if(!stopped)
     {
        for(it = componnents.begin(); it != componnents.end(); it++)
        {
           comp = *it;
           comp->getReactorTask()->getImpl()->end_reactor_event_loop();
        }
     }
  mutex.release();

  std::cout << "<ShutdownManager> Wait on the ManagedTask(s) used by components to be closed..." << std::endl;
  ACE_Thread_Manager::instance()->wait_task(SmartACE::BaseTaskDummy::instance());

  mutex.acquire();
     if(!stopped)
     {
        Smart::NAMING::instance()->fini();
        // exit is now safe to use
        std::cout << "<ShutdownManager> Shutdown successful, BYE" << std::endl;
        exit(0);
     }
  mutex.release();
  return 0;
}

int ShutdownManager::ShutdownTask::stop()
{
   ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, mutex, -1);
   stopped = true;
   return 0;
}

