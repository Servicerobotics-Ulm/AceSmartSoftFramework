// --------------------------------------------------------------------------
//
//  Copyright (C) 2009/2010 Alex Lotz
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


#ifndef SMARTMANAGEDTASK_HH_
#define SMARTMANAGEDTASK_HH_

#include <smartIManagedTask.h>
#include <smartInputTaskTrigger.h>
#include "smartTask.hh"

namespace SmartACE {

  /** Wrapper for active objects.
   */
   class ManagedTask : public Smart::IManagedTask, public Task
      // = TITLE
   {
   public:
		/// default constructor
		ManagedTask(Smart::IComponent *component, Smart::TaskTriggerSubject *trigger=0);

		/// default destructor
		virtual ~ManagedTask();

		/// user hook that is called once at the <b>beginning</b> of the internal thread
		virtual int on_entry();

		/// user hook that is called periodically in the thread (must be implemented in derived classes)
		virtual int on_execute() = 0;

		/// user hook that is called once at the <b>end</b> of the thread
		virtual int on_exit();
   };

} // end namespace SmartACE

#endif /* SMARTMANAGEDTASK_HH_ */
