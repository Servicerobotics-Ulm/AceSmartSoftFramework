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

#ifndef SMARTCONDVAR_HH_
#define SMARTCONDVAR_HH_

#include <smartStatusCode.h>
#include "smartOSMapping.hh"

/////////////////////////////////////////////////////////////////////////
//
// additional functions / classes
//
/////////////////////////////////////////////////////////////////////////

namespace SmartACE {

  /** @internal
   *  Condition variable with memory / more or less a semaphore which does
   *  not count.
   *
   *  The condition variable is used to avoid active waiting. It uses an
   *  internal state to indicate whether the condition variable has been
   *  signaled already (active state) and then returns immediately
   *  resetting the internal state (passive state). Can not be used to
   *  broadcast a signal since multiple signals are summarized in the
   *  active state which is consumed by the first call to the wait-method.
   *
   *  Within the communication patterns there is always one producer
   *  and one consumer of a signal. Used to handle methods like queryReceive
   *  where one has to know whether the expected answer has already been
   *  received before invoking the queryReceive method (if not yet received
   *  block until reception of answer).
   */
  class SmartCntCondClass {
  private:
    int                  v;
    SmartMutex           mutex;
    SmartConditionMutex* cond;
  public:
    /// constructor
    SmartCntCondClass();

    /// destructor
    ~SmartCntCondClass();

    /// copy constructor
    SmartCntCondClass(const SmartCntCondClass&);

    /// copy assignment
    SmartCntCondClass& operator=(const SmartCntCondClass&);

    ///
    void signal(void);

    ///
    void wait(void);

    ///
    int test(void);
  };
}

namespace SmartACE {

  /** @internal condition like variable class.
   *
   *  The condition variable is used to avoid active waiting. In
   *  contrast to the SmartCntCondClass version, we really wait until
   *  the next signal is received and we do not store an already
   *  received signal. "signalAll" e.g. is used within clients of the
   *  push service to release every waiting thread as soon as an
   *  update is received.
   */
  class SmartCondClass {
  private:
    int                  v;
    SmartMutex           mutex;
    SmartConditionMutex* cond;
  public:
    /// constructor
    SmartCondClass();

    /// destructor
    ~SmartCondClass();

    /// copy constructor
    SmartCondClass(const SmartCondClass&);

    /// copy assignment
    SmartCondClass& operator=(const SmartCondClass&);

    ///
    int signalOne(void);

    ///
    int signalAll(void);

    ///
    int wait(void);
  };
}





namespace SmartACE {

  /** @internal
   *  Condition variable without memory
   *
   *  The condition variable is used to avoid active waiting. The condition
   *  variable has no memory and does not know whether it has been signalled
   *  before or not. It is therefore used for example within the pushNewestClient
   *  to wait for the next arriving update.
   *
   *  Condition variable discriminates between blockingPattern and blockingUser.
   *  Both can enforce non-blocking. The pattern uses the non-blocking mode to
   *  abort blocking waits when getting disconnected for example. This is used
   *  to avoid blocking calls for new updates if for example an unsubscribe is
   *  executed in parallel to a blocking call to "getUpdateWait" in the
   *  pushNewestClient. Please look there for further details. The user can
   *  enforce non-blocking mode to abort blocking waits for example to continue
   *  with processing without awaiting the answer.
   *
   *  One could also implement a counting blocking indicator with acquire/release
   *  member functions. It is however less error prone from the user view not
   *  to be forced to have a matching number of acquire/release calls and we
   *  therefore implemented separate flags for the pattern internal use and
   *  the user.
   *
   *  The wait() member function first checks the blocking flags. It returns
   *  immediately with SMART_CANCELLED if at least one flag enforces non-blocking
   *  mode. Otherwise it waits for either a signal or for being cancelled. If
   *  the condition variable is signalled, it returns with SMART_OK.
   */
  class SmartCVwithoutMemory {
  private:
    bool                 statusBlockingComponent;
    bool                 statusBlockingPattern;
    bool                 statusBlockingUser;
    SmartMutex           mutex;
    SmartConditionMutex* cond;
  public:
    /// constructor
    SmartCVwithoutMemory();

    /// destructor
    ~SmartCVwithoutMemory();

    /// copy constructor
    SmartCVwithoutMemory(const SmartCVwithoutMemory&);

    /// copy assignment
    SmartCVwithoutMemory& operator=(const SmartCVwithoutMemory&);

    ///
    void blockingUser(const bool);

    ///
    void blockingPattern(const bool);

    ///
    void blockingComponent(const bool);

    ///
    void signal();

    ///
    Smart::StatusCode wait(const SmartTimeValue &timeout=SmartTimeValue::zero);
  };
}

namespace SmartACE {

  /** @internal
   *  Condition variable with memory
   *
   *  The condition variable is used to avoid active waiting. It memorizes
   *  whether the condition variable has been signalled since the last
   *  wait. It is used within the queryPattern to memorize whether an
   *  answer has been received for the pending request.
   *
   *  Using this condition variable makes sure that no signal and therefore
   *  no answer is missed. One otherwise would eventually block infinitely
   *  on a condition variable waiting for the next signal which will never
   *  be given since each request has exactly one response.
   *
   *  Condition variable discriminates between blockingPattern and blockingUser.
   *  Both can enforce non-blocking. The pattern uses the non-blocking mode to
   *  abort blocking waits when getting disconnected for example. The user can
   *  enforce non-blocking mode to abort blocking waits to for example continue
   *  with processing without awaiting the answer.
   *
   *  One could also implement a counting blocking indicator with acquire/release
   *  member functions. It is however less error prone from the user view not
   *  to be forced to have a matching number of acquire/release calls and we
   *  therefore implemented separate flags for the pattern internal use and
   *  the user.
   *
   *  Signalling this condition variable sets the signalled flag. The wait()
   *  member function returns immediately with SMART_OK in case the signalled
   *  flag is set and consumes it. If it has not been signalled so far, it
   *  checks the blocking flags. It returns immediately with SMART_CANCELLED
   *  in case of at least one flag enforces non-blocking mode. Otherwise, it
   *  waits for either a signal or for being cancelled.
   *
   */
  class SmartCVwithMemory {
  private:
    bool                 signalled;
    bool                 statusBlockingComponent;
    bool                 statusBlockingPattern;
    bool                 statusBlockingUser;
    SmartMutex           mutex;
    SmartConditionMutex* cond;
  public:
    /// constructor
    SmartCVwithMemory();

    /// destructor
    ~SmartCVwithMemory();

    /// copy constructor
    SmartCVwithMemory(const SmartCVwithMemory&);

    /// copy assignment
    SmartCVwithMemory& operator=(const SmartCVwithMemory&);

    ///
    void blockingUser(const bool);

    ///
    void blockingPattern(const bool);

    ///
    void blockingComponent(const bool);

    ///
    void signal();

    ///
    Smart::StatusCode wait(const SmartTimeValue &timeout=SmartTimeValue::zero);

    ///
    void reset();
  };
}

#endif /* SMARTCONDVAR_HH_ */
