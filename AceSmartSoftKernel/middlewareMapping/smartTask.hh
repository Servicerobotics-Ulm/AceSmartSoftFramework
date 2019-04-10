// --------------------------------------------------------------------------
//
//  Copyright (C) 2002/2004/2008 Christian Schlegel
//                2012, 2017 Alex Lotz
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

#ifndef _SMARTTASK_HH
#define _SMARTTASK_HH

#include <smartITask.h>
#include "smartOSMapping.hh"

namespace SmartACE {
  /** Base class for active objects.
   */
  class Task : virtual public Smart::ITask, public ACE_Task<ACE_MT_SYNCH>
    // = TITLE
  {
  public:
    // = Initialization

    /// Constructor
    Task (Smart::IComponent *component = 0);

    /// Destructor
    virtual ~Task();

    // = User interface

    /** Creates and starts a new thread (if not yet started)
     *
     */
    virtual int start();

    /// control method to <b>start</b> the internal thread using given scheduling parameters
    virtual int start(const ACE_Sched_Params &sched_params, const int &cpuAffinity=-1);

    /** Closes currently active thread (if it was started before)
     */
    virtual int stop(const bool wait_till_stopped=true);

  protected:
    /** Tests whether the thread has been signaled to stop.
     *
     * This method allows to implement cooperative thread stopping.
     */
    virtual bool test_canceled();

    /** Blocks execution of the calling thread during the span of time specified by rel_time.
     *
     *  Thread-sleeping is sometimes platform-specific. This method encapsulates the
     *  blocking sleep. Calling this method blocks the execution of the calling thread
     *  for a time specified by rel_time.
     *
     *  @param rel_time relative time duration for the thread to sleep
     */
    virtual void sleep_for(const std::chrono::steady_clock::duration &rel_time);

    /// implements ACE_Task<ACE_MT_SYNCH>
    virtual int svc (void);

    int setCpuAffinity(const int &cpuCore);
    int setSchedParams(const ACE_Sched_Params &sched_params);

  private:
		/// @internal variable to consider only the first call of start() function
		bool thread_started;

		/// @internal Mutex
		SmartRecursiveMutex mutex;
  };
  
}


#endif // _SMARTTASK_HH

