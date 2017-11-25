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

#include "smartCondVar.hh"

/////////////////////////////////////////////////////////////////////////
//
// additional functions
//
/////////////////////////////////////////////////////////////////////////

//
// smartsoft semaphore like condition variable
//
SmartACE::SmartCntCondClass::SmartCntCondClass()
{
  v = 0;

  cond = new SmartConditionMutex(mutex);
}

SmartACE::SmartCntCondClass::~SmartCntCondClass()
{
  delete cond;
}

SmartACE::SmartCntCondClass::SmartCntCondClass(const SmartACE::SmartCntCondClass& s)
{
  v = s.v;

  cond = new SmartConditionMutex(mutex);
}

SmartACE::SmartCntCondClass& SmartACE::SmartCntCondClass::operator=(const SmartACE::SmartCntCondClass& s)
{
  if (this != &s) {
    delete cond;

    v = s.v;
    cond = new SmartConditionMutex(mutex);
  }
  return *this;
}

void SmartACE::SmartCntCondClass::signal(void)
{
  mutex.acquire();
  v=1;
  cond->signal();
  mutex.release();
}

void SmartACE::SmartCntCondClass::wait(void)
{
  mutex.acquire();
  if (v == 0) {
    cond->wait();
  }
  v=0;
  mutex.release();
}

int SmartACE::SmartCntCondClass::test(void)
{
  int res;

  mutex.acquire();
  res = v;
  mutex.release();

  return res;
}







//
// smartsoft  condition variable
//
SmartACE::SmartCondClass::SmartCondClass()
{
  v = 0;

  cond = new SmartConditionMutex(mutex);
}

SmartACE::SmartCondClass::~SmartCondClass()
{
  delete cond;
}

SmartACE::SmartCondClass::SmartCondClass(const SmartACE::SmartCondClass& s)
{
  v = s.v;

  cond = new SmartConditionMutex(mutex);
}

SmartACE::SmartCondClass& SmartACE::SmartCondClass::operator=(const SmartACE::SmartCondClass& s)
{
  if (this != &s) {
    delete cond;

    v = s.v;
    cond = new SmartConditionMutex(mutex);
  }
  return *this;
}

int SmartACE::SmartCondClass::signalOne(void)
{
  int res;

  mutex.acquire();
  res = v--;
  cond->signal();
  mutex.release();

  return res;
}

int SmartACE::SmartCondClass::signalAll(void)
{
  int res;

  mutex.acquire();
  res = v;
  v   = 0;
  cond->broadcast();
  mutex.release();

  return res;
}

int SmartACE::SmartCondClass::wait(void)
{
  mutex.acquire();
  v++;
  cond->wait();
  mutex.release();

  return 0;
}



/////////////////////////////////////////////////////////////////////////
//
// SmartCVwithoutMemory
//
/////////////////////////////////////////////////////////////////////////
SmartACE::SmartCVwithoutMemory::SmartCVwithoutMemory()
{
  statusBlockingComponent = true;
  statusBlockingPattern   = true;
  statusBlockingUser      = true;

  cond = new SmartConditionMutex(mutex);
}

SmartACE::SmartCVwithoutMemory::~SmartCVwithoutMemory()
{
  delete cond;
}

SmartACE::SmartCVwithoutMemory::SmartCVwithoutMemory(const SmartACE::SmartCVwithoutMemory& s)
{
  // hint: we normally should wrap this block with s.mutex.acquire()/release() but mutex is not public
  statusBlockingComponent = s.statusBlockingComponent;
  statusBlockingPattern   = s.statusBlockingPattern;
  statusBlockingUser      = s.statusBlockingUser;

  cond = new SmartConditionMutex(mutex);
}

SmartACE::SmartCVwithoutMemory& SmartACE::SmartCVwithoutMemory::operator=(const SmartACE::SmartCVwithoutMemory& s)
{
  if (this != &s) {
    delete cond;

    // hint: we normally should wrap this block with s.mutex.acquire()/release() but mutex is not public
    statusBlockingComponent = s.statusBlockingComponent;
    statusBlockingPattern   = s.statusBlockingPattern;
    statusBlockingUser      = s.statusBlockingUser;

    cond = new SmartConditionMutex(mutex);
  }
  return *this;
}

void SmartACE::SmartCVwithoutMemory::blockingComponent(const bool b)
{
  mutex.acquire();
  statusBlockingComponent = b;
  if (statusBlockingComponent == false) {
    cond->broadcast();
  }
  mutex.release();
}

void SmartACE::SmartCVwithoutMemory::blockingPattern(const bool b)
{
  mutex.acquire();
  statusBlockingPattern = b;
  if (statusBlockingPattern == false) {
    cond->broadcast();
  }
  mutex.release();
}

void SmartACE::SmartCVwithoutMemory::blockingUser(const bool b)
{
  mutex.acquire();
  statusBlockingUser = b;
  if (statusBlockingUser == false) {
    cond->broadcast();
  }
  mutex.release();
}

void SmartACE::SmartCVwithoutMemory::signal()
{
  mutex.acquire();
  cond->broadcast();
  mutex.release();
}

Smart::StatusCode SmartACE::SmartCVwithoutMemory::wait()
{
  int flag;
  Smart::StatusCode result = Smart::SMART_OK;

  mutex.acquire();

  flag = 0;
  while (flag == 0) {
    if ((statusBlockingPattern == true) && (statusBlockingUser == true) && (statusBlockingComponent == true)) {
      // wait for the next signal
      cond->wait();
      // signal can be either from change of blocking mode or a true signal
      if ((statusBlockingComponent == true) && (statusBlockingPattern == true) && (statusBlockingUser == true)) {
        // still both blocking therefore true signal
        flag   = 1;
        result = Smart::SMART_OK;
      } else {
        // non blocking mode ...
        flag   = 1;
        result = Smart::SMART_CANCELLED;
      }
    } else {
      // return immediately
      flag   = 1;
      result = Smart::SMART_CANCELLED;
    }
  }

  mutex.release();

  return result;
}


Smart::StatusCode SmartACE::SmartCVwithoutMemory::wait(const SmartTimeValue &timeout)
{
  int flag;
  Smart::StatusCode result = Smart::SMART_OK;

  mutex.acquire();

  flag = 0;
  while (flag == 0) {
    if ((statusBlockingPattern == true) && (statusBlockingUser == true) && (statusBlockingComponent == true)) {
      // wait for the next signal
       SmartTimeValue time = ACE_OS::gettimeofday() + timeout;
       if( cond->wait(&time) == -1) {
         if(errno == ETIME) {
            result = Smart::SMART_TIMEOUT;
         }else{
            result = Smart::SMART_ERROR;
         }
         flag = 1;
       }
      // signal can be either from change of blocking mode or a true signal
      if ((statusBlockingComponent == true) && (statusBlockingPattern == true) && (statusBlockingUser == true)) {
        // still both blocking therefore true signal
        flag   = 1;
        result = Smart::SMART_OK;
      } else {
        // non blocking mode ...
        flag   = 1;
        result = Smart::SMART_CANCELLED;
      }
    } else {
      // return immediately
      flag   = 1;
      result = Smart::SMART_CANCELLED;
    }
  }

  mutex.release();

  return result;
}


/////////////////////////////////////////////////////////////////////////
//
// SmartCVwithMemory
//
/////////////////////////////////////////////////////////////////////////
SmartACE::SmartCVwithMemory::SmartCVwithMemory()
{
  signalled               = false;
  statusBlockingComponent = true;
  statusBlockingPattern   = true;
  statusBlockingUser      = true;

  cond = new SmartConditionMutex(mutex);
}

SmartACE::SmartCVwithMemory::~SmartCVwithMemory()
{
  delete cond;
}

SmartACE::SmartCVwithMemory::SmartCVwithMemory(const SmartACE::SmartCVwithMemory& s)
{
  // hint: we normally should wrap this block with s.mutex.acquire()/release() but mutex is not public
  signalled               = s.signalled;
  statusBlockingComponent = s.statusBlockingComponent;
  statusBlockingPattern   = s.statusBlockingPattern;
  statusBlockingUser      = s.statusBlockingUser;

  cond = new SmartConditionMutex(mutex);
}

SmartACE::SmartCVwithMemory& SmartACE::SmartCVwithMemory::operator=(const SmartACE::SmartCVwithMemory& s)
{
  if (this != &s) {
    delete cond;

    // hint: we normally should wrap this block with s.mutex.acquire()/release() but mutex is not public
    signalled               = s.signalled;
    statusBlockingComponent = s.statusBlockingComponent;
    statusBlockingPattern   = s.statusBlockingPattern;
    statusBlockingUser      = s.statusBlockingUser;

    cond = new SmartConditionMutex(mutex);
  }
  return *this;
}

void SmartACE::SmartCVwithMemory::blockingComponent(const bool b)
{
  mutex.acquire();
  statusBlockingComponent = b;
  if (statusBlockingComponent == false) {
    cond->broadcast();
  }
  mutex.release();
}

void SmartACE::SmartCVwithMemory::blockingPattern(const bool b)
{
  mutex.acquire();
  statusBlockingPattern = b;
  if (statusBlockingPattern == false) {
    cond->broadcast();
  }
  mutex.release();
}

void SmartACE::SmartCVwithMemory::blockingUser(const bool b)
{
  mutex.acquire();
  statusBlockingUser = b;
  if (statusBlockingUser == false) {
    cond->broadcast();
  }
  mutex.release();
}

void SmartACE::SmartCVwithMemory::signal()
{
  mutex.acquire();
  signalled = true;
  cond->broadcast();
  mutex.release();
}

Smart::StatusCode SmartACE::SmartCVwithMemory::wait()
{
  int flag;
  Smart::StatusCode result = Smart::SMART_OK;

  mutex.acquire();

  flag = 0;
  while (flag == 0) {
    if (signalled == true) {
      flag      = 1;
      signalled = false;
      result    = Smart::SMART_OK;
    } else if ((statusBlockingComponent == true) && (statusBlockingPattern == true) && (statusBlockingUser == true)) {
      cond->wait();
    } else {
      flag   = 1;
      result = Smart::SMART_CANCELLED;
    }
  }

  mutex.release();

  return result;
}

Smart::StatusCode SmartACE::SmartCVwithMemory::wait(const SmartTimeValue &timeout)
{
  int flag;
  Smart::StatusCode result = Smart::SMART_OK;

  mutex.acquire();

  flag = 0;
  while (flag == 0) {
    if (signalled == true) {
      flag      = 1;
      signalled = false;
      result    = Smart::SMART_OK;
    } else if ((statusBlockingComponent == true) && (statusBlockingPattern == true) && (statusBlockingUser == true)) {
       SmartTimeValue time = ACE_OS::gettimeofday() + timeout;
       if( cond->wait(&time) == -1) {
         if(errno == ETIME) {
            result = Smart::SMART_TIMEOUT;
         }else{
            result = Smart::SMART_ERROR;
         }
         flag = 1;
       }
    } else {
      flag   = 1;
      result = Smart::SMART_CANCELLED;
    }
  }

  mutex.release();

  return result;
}


void SmartACE::SmartCVwithMemory::reset()
{
  mutex.acquire();
  signalled = false;
  mutex.release();
}




