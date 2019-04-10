// --------------------------------------------------------------------------
//
//  Copyright (C) 2002/2004/2008 Christian Schlegel
//                2012 Alex Lotz
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

#ifndef _SMARTTIMERTHREAD_HH
#define _SMARTTIMERTHREAD_HH


class ACE_Handler;

#include <ace/Task.h>
#include <ace/Synch.h>
#include <ace/Timer_Heap.h>
#include <ace/Time_Value.h>

#include <smartITimerManager.h>
#include <set>

namespace SmartACE {

/** Interface to receive notification from a TimerThread.
 *
 *  handler methods (hooks) are not abstract -> the user is not forced to
 *  provide handler methods he does not need.
 */
class TimerHandler : public Smart::ITimerHandler {
public:
  TimerHandler();
  virtual ~TimerHandler();

  /** hook called on timer expiration
   */
  virtual void timerExpired(const std::chrono::system_clock::time_point &abs_time, const void * arg) = 0;

  /// This method is called when the timer is cancelled
  virtual void timerCancelled() {  }

  /// This method is called when the timer queue is destroyed and
  /// the timer is still contained in it
  virtual void timerDeleted(const void * arg) override {  }
};

/** Functor for Timer_Queues.
 *
 *  @internal
 *
 * This class implements the functor required by ACE_Timer_Queue
 * to call the Handler method in a TimerHandler Object
 */
template <class ACE_LOCK>
class TimerHandlerHandleUpcall
{
public:
  typedef ACE_Timer_Queue_T<Smart::ITimerHandler *,
                            TimerHandlerHandleUpcall<ACE_LOCK>,
                            ACE_LOCK>
  TIMER_QUEUE;

  // = Initialization and termination methods.
  /// Constructor.
  TimerHandlerHandleUpcall () {};

  /// Destructor.
  ~TimerHandlerHandleUpcall () {};

  /// This method is called when the timer expires
  int timeout (TIMER_QUEUE &timer_queue,
		 Smart::ITimerHandler *handler,
		 const void *arg,
		 int recurring_timer,
		 const ACE_Time_Value &cur_time)
    {
	// Upcall to the <handler>s handle_timeout method.
	handler->timerExpired (std::chrono::system_clock::now(), arg);
	return 0;
    };


  /// This method is called when the timer is cancelled
  int cancel_timer (TIMER_QUEUE &timer_queue,
		  Smart::ITimerHandler *handler,
                    int dont_call_handle_close,
                    int requires_reference_counting)
    {
	handler->timerCancelled ();
	return 0;
    };

  /// This method is called when the timer queue is destroyed and
  /// the timer is still contained in it
  int deletion (TIMER_QUEUE &timer_queue,
		  Smart::ITimerHandler *handler,
		  const void *arg)
    {
	handler->timerDeleted(arg);
	return 0;
    }

  /// This method is called when a time is registered
  int registration (TIMER_QUEUE &timer_queue,
		  Smart::ITimerHandler *handler,
                    const void *arg)
  {
    return 0;
  }

  /// This method is called before the timer expires
  int preinvoke (TIMER_QUEUE &timer_queue,
		  Smart::ITimerHandler *handler,
                 const void *arg,
                 int reccuring_timer,
                 const ACE_Time_Value &cur_time,
                 const void *upcall_act)
  {
    return 0;
  }

  /// This method is called after the timer expires
  int postinvoke (TIMER_QUEUE &timer_queue,
		  Smart::ITimerHandler *handler,
                  const void *arg,
                  int recurring_timer,
                  const ACE_Time_Value &cur_time,
                  const void *upcall_act)
  {
    return 0;
  }

  /// This method is called when a handler is canceled
  int cancel_type (TIMER_QUEUE &timer_queue,
		  Smart::ITimerHandler *handler,
                   int dont_call_handle_close,
                   int &requires_reference_counting)
  {
    return 0;
  }

private:
  // = Don't allow these operations for now.
  ACE_UNIMPLEMENTED_FUNC (TimerHandlerHandleUpcall (const TimerHandlerHandleUpcall<ACE_LOCK> &));
  ACE_UNIMPLEMENTED_FUNC (void operator= (const TimerHandlerHandleUpcall<ACE_LOCK> &));
};


typedef ACE_Timer_Heap_T<Smart::ITimerHandler *,
                         TimerHandlerHandleUpcall<ACE_SYNCH_RECURSIVE_MUTEX>,
                         ACE_SYNCH_RECURSIVE_MUTEX>
TimerHeap;

  /** single/continuous timer.
   *
   *  This object has a thread that waits on the earliest time in
   *  a list of timers and an event. When a timer expires, the
   *  apropriate handler is called in the context of this thread.
   *
   *  It is based on ACE_Time_Heap and friends.
   *
   *  Some code taken from ace/Proactor.cpp
   */
  class TimerManagerThread
  :   public ACE_Task<ACE_NULL_SYNCH>
  ,   public Smart::ITimerManager
  {
  public:
    /// Constructor.
	  TimerManagerThread();

    /// Destructor. Properly shuts down the timer thread
    virtual ~TimerManagerThread ();

    /** Schedule a timer.
     *
     *  @param  handler  The handler that will be called when the timer
     *                   expires.
     *  @param  act      a value that will be passed to the handler.
     *  @param  time     relative time for the first timer expiration
     *  @param  interval Interval for periodic timers. A single shot timer
     *                   is scheduled by default.
     *
     * @return timer_id: -1 on failure. Unique time id else. This id
     *                     can be used to cancel a timer before it
     *                     expires with cancelTimer() and to change
     *                     the the interval of a timer with
     *                     resetTimerInterval().
     */
	virtual TimerId scheduleTimer(
			Smart::ITimerHandler *handler,
			const void *act,
			const std::chrono::steady_clock::duration &first_time,
			const std::chrono::steady_clock::duration &interval=std::chrono::steady_clock::duration::zero()
		);
//    long scheduleTimer(Smart::ITimerHandler *handler,
//		       const void *act,
//		       const ACE_Time_Value &time,
//		       const ACE_Time_Value &interval = ACE_Time_Value::zero);


    /** Cancel a single timer.
     *
     *  @param  timer_id   to cancel
     *  @param  act        pointer to retrive the act that was given on
     *                     scheduleTimer(). Can be used to release ressources
     *                     owned by act. If act == 0, nothing is retrieved.
     *  @param notifyHandler
     *                     true: calls timerCancelled() hook on the associated handler
     *                     false: do not call timerCancelled() hook
     *  @return 0 on success
     *  @return -1 on error
     */
	virtual int cancelTimer(const TimerId& id, const void **act=0);
//    int cancelTimer(long timer_id,
//			   const void **act=0,
//			   bool notifyHandler=true);

    /** Cancel all timers associated with a handler
     *
     *  @param handler     cancel timers associated with this handler
     *  @param notifyHandler
     *                     true: calls timerCancelled() hook on the associated handler
     *                     false: do not call timerCancelled() hook
     *
     *  @return number of timers canceled.
     */
	virtual int cancelTimersOf(Smart::ITimerHandler *handler);
//    int cancelTimers(Smart::ITimerHandler *handler,
//		     bool notifyHandler=true);
	virtual void cancelAllTimers();

    /** Resets the interval of a timer.
     *
     *  @param timer_id     to change
     *  @param interval     new timer interval (relative to the current time)
     *  @return 0 on success
     *  @return -1 on error
     */
	virtual int resetTimerInterval(
			const TimerId& id,
			const std::chrono::steady_clock::duration &interval
		);
//    int resetTimerInterval(long timer_id,
//				  const ACE_Time_Value &interval);

    /** Start execution of the internal timer thread
     * @return >=0 on success
     * @return -1 on error
     */
    int startTimerThread();

    /** Start execution of the internal timer thread
     * @param sched_params	initializes a specific scheduler
     * @param cpuAffinity	assigns the thread to one specific CPU core
     * @return >=0 on success
     * @return -1 on error
     */
    int startTimerThread(const ACE_Sched_Params &sched_params, const int &cpuAffinity=-1);

    /** Initializes timer thread to permanently shutdown
     * @param waitOnCompletion indicates, whether this method should block until timer is completely down
     */
    void shutdownTimer(const bool &waitOnCompletion=true);

  protected:
    /// Run by a daemon thread to handle deferred processing. In other
    /// words, this method will do the waiting on the earliest timer and
    /// event.
    virtual int svc (void);

    int setSchedParams(const ACE_Sched_Params &sched_params);

    int setCpuAffinity(const int &cpuCore);

    TimerHeap timer_queue;
    std::set<Smart::ITimerHandler*> handlers;

    /// Event to wait on.
    ACE_Auto_Event timer_event_;

    /// Flag used to indicate when we are shutting down.
    int shutting_down_;
  };

} // end namespace Smart

#endif // _SMARTTIMERTHREAD_HH
