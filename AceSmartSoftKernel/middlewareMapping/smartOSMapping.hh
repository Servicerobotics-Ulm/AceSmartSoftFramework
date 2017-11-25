// --------------------------------------------------------------------------
//
//  Copyright (C) 2002/2004/2008 Christian Schlegel
//                2009/2012 Alex Lotz
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

#ifndef _SMARTSYS_HH
#define _SMARTSYS_HH

#include <iostream>
#include <iomanip>
#include <chrono>

#include <ace/ACE.h>
#include "ace/Synch.h"
#if ACE_MAJOR_VERSION < 6
#include <ace/Synch_T.h> // for backwards compatibility only
#endif
#include <ace/Thread.h>
#include <ace/Thread_Manager.h>
#include <ace/Shared_Memory_SV.h>
#include <ace/Token.h>
#include <ace/Task.h>
#include <ace/OS_NS_time.h>
#include <ace/UUID.h>
#include <ace/Condition_Recursive_Thread_Mutex.h>
#include <ace/Log_Msg.h>
#include <ace/Message_Block.h>
#include <ace/SString.h>
#include <ace/Sched_Params.h>

#ifdef __QNXNTO__
   #include "ctype.h"
   #include "stdlib.h"
#else
   #if ACE_MAJOR_VERSION < 6
      #include <ace/OS.h> // for backwards compatibility
   #else
      #include <ace/OS_main.h>
   #endif
#endif

#ifndef WIN32
   #include <unistd.h>
#endif

#ifndef SMARTSOFT_PRINT_PREFIX
   #define SMARTSOFT_PRINT_PREFIX ACE_TEXT("<SmartSoft> ")
#endif

#ifndef SMARTSOFT_PRINT
   #define SMARTSOFT_PRINT(...) ACE_DEBUG((LM_INFO, SMARTSOFT_PRINT_PREFIX __VA_ARGS__))
#endif

namespace SmartACE {
  /// typedef of ACE class
  typedef ACE_Thread_Mutex SmartMutex;
  /// typedef of ACE class
  typedef ACE_RW_Thread_Mutex SmartRWMutex;
  /// typedef of ACE class
  typedef ACE_Recursive_Thread_Mutex SmartRecursiveMutex;
  /// typedef of ACE class
  typedef ACE_Semaphore SmartSemaphore;

  /// typedef of ACE class
  typedef ACE_Guard<ACE_Thread_Mutex> SmartGuard;
  /// typedef of ACE class
  typedef ACE_Read_Guard<ACE_RW_Thread_Mutex> SmartReadGuard;
  /// typedef of ACE class
  typedef ACE_Write_Guard<ACE_RW_Thread_Mutex> SmartWriteGuard;

  //<alexej date="2010-11-29">
  /// typedef of ACE class
  typedef ACE_Guard<ACE_Recursive_Thread_Mutex> SmartRecursiveGuard;
  //</alexej>
  
  /// typedef of ACE class
  typedef ACE_Condition_Thread_Mutex SmartConditionMutex;

  //<alexej date="2010-03-18">
  /// typedef for a recursive condition variable (used in SmartMonitor class)
  typedef ACE_Condition_Recursive_Thread_Mutex SmartConditionRecursiveMutex;
  //</alexej>

  /// define ACE makro to handle SmartMutex acquire/release automaticaly
  #define SMART_GUARD(LOCK) ACE_GUARD(SmartACE::SmartMutex, guard, LOCK);

  /// define ACE makro to handle SmartMutex acquire/release automaticaly, breaking on error with RETURN value
  #define SMART_GUARD_RETURN(LOCK, RETURN) ACE_GUARD_RETURN(SmartACE::SmartMutex, guard, LOCK, RETURN);

  /// define ACE makro to handle SmartMutex acquire/release automaticaly
  #define SMART_REC_GUARD(LOCK) ACE_GUARD(SmartACE::SmartRecursiveMutex, guard, LOCK);

  /// define ACE makro to handle SmartMutex acquire/release automaticaly, breaking on error with RETURN value
  #define SMART_REC_GUARD_RETURN(LOCK, RETURN) ACE_GUARD_RETURN(SmartACE::SmartRecursiveMutex, guard, LOCK, RETURN);

  /// define an interoperable TimeValue based on the ACE implementation
  typedef ACE_Time_Value SmartTimeValue;


  // basic thread management
  /// typedef of ACE class
  typedef ACE_Thread_Manager SmartThreadManager;
  /// typedef of ACE class
  typedef ACE_Thread_Control SmartThreadControl;
  /// typedef of ACE class
  typedef ACE_Thread SmartThread;
  /// typedef of ACE class
  typedef ACE_THR_FUNC SMART_THR_FUNC;

  /// typedef of ACE class
  typedef ACE_thread_t SmartThreadType;
  /// typedef of ACE class
  typedef ACE_hthread_t SmartHThreadType;

  //
  // misc utility functions
  //

  /// easy printing of Time_Values
  std::ostream & operator<<(std::ostream & o, const ACE_Time_Value & t);

  //<alexej date="02.10.2008">
  typedef ACE_Message_Block SmartMessageBlock;
  //</alexej>

  std::chrono::steady_clock::duration convertToStdDurFrom(const ACE_Time_Value &time);
  ACE_Time_Value convertToAceTimeFrom(const std::chrono::steady_clock::duration &dur);
}

#endif // _SMARTSYS_HH

