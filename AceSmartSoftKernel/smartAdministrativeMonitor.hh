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

#ifndef SMARTADMINISTRATIVEMONITOR_HH_
#define SMARTADMINISTRATIVEMONITOR_HH_

#include <smartStatusCode.h>
#include "smartOSMapping.hh"


namespace SmartACE {
 /** @internal
  *  This class is used to implement synchronized Request-Response message communication.
  *  The implementation is similar to SmartCVwithMemory class, with the difference of using
  *  SmartRecursiveMutex and SmartConditionRecursiveMutex.
  *  (for more Details see PhD Thesis of Prof. Dr. Christian Schlegel, chapter 5.6.6.5)
  */
   class SmartMonitor
   {
   private:
      int nested_level;
      SmartRecursiveMutex  mutex;
   protected:
      bool                 signalled;

      bool                 statusBlocking;
      bool                 statusTimed;

      ACE_Time_Value timeout_time;

      SmartConditionRecursiveMutex cond;

      //<alexej date="2010-03-17">
      // ACE-BUG: nested level is not supported with cond.wait()
      // hence this function reduces the nested level to 1 before calling wait()
      void clear_nested_level();

      // this function restores the nested-level after calling cond.wait()
      // to former level
      void restore_nested_level();
      //</alexej>

   public:
      /// default constructor
      SmartMonitor();

      /// default destructor
      virtual ~SmartMonitor();

      /// block/unblock
      void blocking(const bool);

      /// enable timed mode
      void enableTimedMode(const ACE_Time_Value &timeout);

      /// disable timed mode
      void disableTimedMode();

      /// aquire internal recursive mutex
      int acquire();

      /// release internal recursive mutex
      int release();

      /// signal to <b>all</b> threads to unblock from wait call
      virtual void broadcast();

      /// reset internal state to "not signalled"
      virtual void reset();

      /// blocking wait on condition variable to unblock
      virtual Smart::StatusCode wait();
   };

   // forward declaration
   class SmartComponent;

  /** @internal
   *  Administrative Monitor is used to implement Monitoring of
   *  B/U interaction from service requestor to sevice provider
   *  e.g. in connect() or disconnect() procedures.
   *  (for more Details see PhD Thesis of Prof. Dr. Christian Schlegel, chapter 5.6.6.10)
   */
  class SmartAdministrativeMonitor: public SmartMonitor
   {
   private:
      int connection_id;
      enum StateAutomaton
      {SAM_AWAIT, SAM_TIMEOUT, SAM_OK, SAM_ERROR, SAM_NONE}state;
      int statusA0;
      SmartComponent *component;
   public:
      /// default constructor
      SmartAdministrativeMonitor(SmartACE::SmartComponent *comp=NULL);

      /// default destructor
      virtual ~SmartAdministrativeMonitor();

      /// prepare the B/U interaction
      void prepare(int cid = 0);

      /// used in callback fktions
      void signal(int cid = 0, int status = (int)Smart::SMART_OK);

      /// getter method for status
      int getStatusA0();

      /// blocking wait on condition wariable to unblock
      Smart::StatusCode wait();
   };
}

#endif /* SMARTADMINISTRATIVEMONITOR_HH_ */
