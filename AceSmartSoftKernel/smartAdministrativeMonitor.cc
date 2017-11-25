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

#include "smartAdministrativeMonitor.hh"

#include "smartComponent.hh"

/////////////////////////////////////////////////////////////////////////
//
// SmartMonitor
//
/////////////////////////////////////////////////////////////////////////
SmartACE::SmartMonitor::SmartMonitor()
:  nested_level(0)
,  cond(mutex)
{
  signalled      = false;
  statusBlocking = true;
  statusTimed    = false;
}

SmartACE::SmartMonitor::~SmartMonitor()
{  }

int SmartACE::SmartMonitor::acquire()
{
   mutex.acquire();
   nested_level++;
   return 0;
}

int SmartACE::SmartMonitor::release()
{
   nested_level--;
   mutex.release();
   return 0;
}

void SmartACE::SmartMonitor::clear_nested_level()
{
   for(int i=nested_level; i>1; --i)
      mutex.release();
}

void SmartACE::SmartMonitor::restore_nested_level()
{
   for(int i=nested_level; i>1; --i)
      mutex.acquire();
}

void SmartACE::SmartMonitor::blocking(const bool b)
{
  acquire();
  statusBlocking = b;
  if (statusBlocking == false) {
    cond.broadcast();
  }
  release();
}

void SmartACE::SmartMonitor::enableTimedMode(const ACE_Time_Value &timeout)
{
  acquire();
  statusTimed = true;
  timeout_time = timeout;
  cond.broadcast();
  release();
}

void SmartACE::SmartMonitor::disableTimedMode()
{
  acquire();
  statusTimed = false;
  cond.broadcast();
  release();
}

void SmartACE::SmartMonitor::broadcast()
{
  acquire();
  signalled = true;
  cond.broadcast();
  release();
}

void SmartACE::SmartMonitor::reset()
{
  acquire();
  signalled = false;
  release();
}

Smart::StatusCode SmartACE::SmartMonitor::wait()
{
  int flag;
  Smart::StatusCode result = Smart::SMART_OK;

  acquire();

  flag = 0;
  while (flag == 0) {
    if (signalled == true) {
      flag      = 1;
      signalled = false;
      result    = Smart::SMART_OK;
    } else if ( (statusBlocking == true) && (statusTimed == false) ) {
      clear_nested_level();
      cond.wait();
      restore_nested_level();
    }else if ( (statusBlocking == true) && (statusTimed == true) ) {
      clear_nested_level();
      ACE_Time_Value timeout = ACE_OS::gettimeofday();
      timeout += timeout_time;
      if( cond.wait(&timeout) == -1) {
         if(errno == ETIME) {
            result = Smart::SMART_TIMEOUT;
         } else {
            result = Smart::SMART_ERROR;
         }
         flag = 1;
      }
      restore_nested_level();
    } else {
      flag   = 1;
      result = Smart::SMART_CANCELLED;
    }
  }

  release();

  return result;
}


/////////////////////////////////////////////////////////////////////////
//
// SmartAdministrativeMonitor
//
/////////////////////////////////////////////////////////////////////////
SmartACE::SmartAdministrativeMonitor::SmartAdministrativeMonitor(SmartACE::SmartComponent *comp)
:  SmartMonitor()
,  component(comp)
,  statusA0(0)
,  connection_id(0)
{
   state = SAM_NONE;
}

SmartACE::SmartAdministrativeMonitor::~SmartAdministrativeMonitor()
{  }

void SmartACE::SmartAdministrativeMonitor::prepare(int cid)
{
   acquire();
      state = SAM_AWAIT;
      connection_id = cid;
      reset();
   release();
}

void SmartACE::SmartAdministrativeMonitor::signal(int cid, int status)
{
   acquire();
      if(cid == connection_id)
      {
         state = SAM_OK;
         statusA0 = status;
         broadcast();
      }
   release();
}

int SmartACE::SmartAdministrativeMonitor::getStatusA0()
{
   int retval = -1;

   acquire();
      retval = statusA0;
   release();

   return retval;
}

Smart::StatusCode SmartACE::SmartAdministrativeMonitor::wait()
{
   Smart::StatusCode result = Smart::SMART_ERROR;

   acquire();

   bool use_blocking_mode = true;

   if(component != NULL)
   {
      // check whether this wait method is called out of the reactor thread
      if( ACE_Thread_Manager::instance()->thr_self() == component->getReactorTask()->getReactorOwnerThread() )
      {
         // wait is called out of the reactor thread -> do not block, but handle reactor events directly...

         use_blocking_mode = false;

         do {
            // handle one event iteration
            if(component->getReactorTask()->getImpl()->handle_events() == -1)
            {
               if(errno == ETIME || errno == EWOULDBLOCK) {
                  // remote acceptor is not available or ping times are too large
                  result = Smart::SMART_TIMEOUT;
                  state = SAM_TIMEOUT;
               }else{
                  result = Smart::SMART_ERROR;
                  state = SAM_ERROR;
               }

               // break up on handling error
               break;
            }
         } while ( state == SAM_AWAIT );

         if(state == SAM_OK) result = Smart::SMART_OK;
      }
   }

   // wait is not called out of the reactor thread -> thus use blocking wait
   if(use_blocking_mode)
   {
      result = SmartACE::SmartMonitor::wait();
      if(result == Smart::SMART_OK)
         state = SAM_OK;
      else
         state = SAM_ERROR;
   }

   release();

   return result;
}


