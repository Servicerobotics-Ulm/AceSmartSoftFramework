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

#include "smartTimerManagerThread.hh"

#include "smartOSMapping.hh"

using namespace SmartACE;

//=============================================================================
// SmartACE::SyncReleaseTimerHandler
//=============================================================================

TimerHandler::TimerHandler()
{

};

TimerHandler::~TimerHandler()
{

};


//=============================================================================
// SmartACE::SyncReleaseTimerHandler
//=============================================================================

//=============================================================================
// SmartACE::TimerThread
//=============================================================================


TimerManagerThread::TimerManagerThread()
  : ACE_Task <ACE_NULL_SYNCH>(),
    shutting_down_ (0)
{
}

TimerManagerThread::~TimerManagerThread (void)
{
   this->shutdownTimer();
}


int TimerManagerThread::startTimerThread() {
	return this->activate (THR_NEW_LWP);
}

int TimerManagerThread::startTimerThread(const ACE_Sched_Params &sched_params, const int &cpuAffinity)
{
	// start timer thread with default thread options
	int result = this->startTimerThread();

	// set-up scheduling options and CPU-affinity
	if(result != -1) {
		result = this->setSchedParams(sched_params);

		if(result != -1 && cpuAffinity != -1) {
			// set the CPU affinity to the given CPU core number
			return this->setCpuAffinity(cpuAffinity);
		}
	}
	return result;
}

int TimerManagerThread::setSchedParams(const ACE_Sched_Params &sched_params)
{
	// retrieve the current thread id and set according cpu affinity
	ACE_thread_t threadList[1];
	if(this->thr_mgr()->thread_list(this, threadList, 1) == 1) {
		if(ACE_OS::sched_params(sched_params, threadList[0]) == -1) {
			if(errno == EPERM || errno == ENOTSUP) {
				ACE_DEBUG((LM_DEBUG, ACE_TEXT("\nWarning: user is not superuser, so the default scheduler will be used\n")));
			} else {
				ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"),ACE_TEXT("ACE_OS::sched_params()")),-1);
			}
		}
	}
	return 0;
}

int TimerManagerThread::setCpuAffinity(const int &cpuCore) {
#if defined _GNU_SOURCE && defined ACE_HAS_PTHREADS
	long numCpus = ACE_OS::num_processors();
	if(cpuCore < numCpus) {
		// use the posix interface to create the cpu affinity struct
		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		CPU_SET(cpuCore,&cpuset);

		// retrieve the current thread id and set according cpu affinity
		ACE_thread_t threadList[1];
		if(this->thr_mgr()->thread_list(this, threadList, 1) == 1) {
			// at least one thread is activated
			// the next call requires explicit linking with the pthread library
			return pthread_setaffinity_np(threadList[0], sizeof(cpu_set_t), &cpuset);
			// the generic call is not set-up properly on linux systems (it would require the compiler flag ACE_HAS_PTHREAD_SETAFFINITY_NP)
//			return ACE_OS::thr_set_affinity(threadList[0], sizeof(cpu_set_t),&cpuset);
		}
	}
#endif
	return EINVAL;
}

void TimerManagerThread::shutdownTimer(const bool &waitOnCompletion)
{
  // shutdown timer thread, if we have one running
  if (thr_count() != 0) {
    // Mark for closing down.
    shutting_down_ = 1;

    // Signal timer event.
    timer_event_.signal ();

    if(waitOnCompletion) {
       // Wait for the Timer Handler thread to exit.
       thr_mgr()->wait_grp (this->grp_id ());
    }
  }
}

int TimerManagerThread::svc (void)
{
  ACE_Time_Value absolute_time;
  int result = 0;

  while (shutting_down_ == 0)
    {
      // Check whether the timer queue has any items in it.
      if (timer_queue.is_empty () == 0)
        {
          // Get the earliest absolute time.
          absolute_time = timer_queue.earliest_time ();

	  /* ACE 5.2 has not ACE_Event::wait() for relative time.
	   * Since our Timerqueue uses the standart gettimeofday (and
	   * the user cant change this), we are allowed to use the
	   * absolute time from timer_queue.earliest_time() for
	   * ACE_Event::wait().
	   *
	   */

          // Block till next timer expires.
          result = timer_event_.wait (&absolute_time);
        }
      else
        // The timer queue has no entries, so wait indefinitely.
        result = timer_event_.wait ();

      // Check for timer expiries.
      if (result == -1)
        {
          switch (errno)
            {
            case ETIME:
              // timeout: expire timers
              timer_queue.expire ();
              break;
            default:
              // Error.
              ACE_ERROR_RETURN ((LM_ERROR,
                                 ACE_TEXT ("%N:%l:(%P | %t):%p\n"),
                                 ACE_TEXT ("TimerManagerThread::svc:wait failed")),
                                -1);
              break;
            }
        }
    }
  return 0;
}

//long
//TimerManagerThread::scheduleTimer(Smart::ITimerHandler *handler,
//			   const void *act,
//			   const ACE_Time_Value &time,
//			   const ACE_Time_Value &interval)
Smart::ITimerManager::TimerId TimerManagerThread::scheduleTimer(
			Smart::ITimerHandler *handler,
			const std::chrono::steady_clock::duration &first_time,
			const std::chrono::steady_clock::duration &interval
		)
{
  // absolute time.
  ACE_Time_Value absolute_time = timer_queue.gettimeofday() + convertToAceTimeFrom(first_time);

  // Only one guy goes in here at a time
  ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> guard(timer_queue.mutex());
  if (guard.locked() == 0) {
    return -1;
  }

  // Schedule the timer
  long result = timer_queue.schedule (handler,
				      (void*)0,
				      absolute_time,
					  convertToAceTimeFrom(interval));
  if (result != -1)
  {
    handlers.insert(handler);
    // no failures: check to see if we are the earliest time
    if (timer_queue.earliest_time () == absolute_time)
    {
      // wake up the timer thread
      if (timer_event_.signal () == -1)
      {
        // Cancel timer
        timer_queue.cancel (result);
        result = -1;
      }
    }
  }
  return result;
}

//int
//TimerManagerThread::cancelTimer(long timer_id,
//			   const void **arg,
//			   bool notifyHandler)
int TimerManagerThread::cancelTimer(const TimerId& id)
{
  // No need to singal timer event here. Even if the cancel timer was
  // the earliest, we will have an extra wakeup.
  long timer_id = id;
  int result = timer_queue.cancel (timer_id);
//					   arg,
//					   !notifyHandler ? 0 : 1);
  if (result == 1) {
    return 0;
  } else {
    // timer not found
    return -1;
  }
}

//int
//TimerManagerThread::cancelTimers(Smart::ITimerHandler *handler,
//			  bool notifyHandler)
int TimerManagerThread::cancelTimersOf(Smart::ITimerHandler *handler)
{
  // No need to signal timer event here. Even if the cancel timer was
  // the earliest, we will have an extra wakeup.
  return timer_queue.cancel (handler);
//			     !notifyHandler ? 0 : 1);
}

void TimerManagerThread::cancelAllTimers()
{
  for(std::set<Smart::ITimerHandler*>::iterator it=handlers.begin(); it!=handlers.end(); it++) {
    this->cancelTimersOf(*it);
  }
}

//int TimerManagerThread::resetTimerInterval(long timer_id,
//						const ACE_Time_Value &interval)
int TimerManagerThread::resetTimerInterval(
		const TimerId& id,
		const std::chrono::steady_clock::duration &interval
	)
{
  ACE_Time_Value next_wakeup = timer_queue.earliest_time();

  // only one at a time.
  ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> guard(timer_queue.mutex());
  if (guard.locked() == 0) {
    return -1;
  }

  long timer_id = id;
  int result = timer_queue.reset_interval(timer_id,
		  convertToAceTimeFrom(interval));
  if (result != 0) {
    // couldn't reset timer interval
    return -1;
  };
  // no failures: check to see if the next wakeup time has changed
  if (timer_queue.earliest_time () == next_wakeup) {
    // wake up the timer thread
    if (timer_event_.signal () == -1)
    {
      // Cancel timer if we couldn't signal our thread
      timer_queue.cancel (result);
      return -1;
    }
  }
  return 0;
}
