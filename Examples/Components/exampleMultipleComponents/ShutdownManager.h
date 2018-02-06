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

#ifndef SHUTDOWNMANAGER_H_
#define SHUTDOWNMANAGER_H_

#include <list>
#include "../../../AceSmartSoftKernel/aceSmartSoft.hh"

class ShutdownManager : public ACE_Event_Handler {
private:
   class ShutdownTask : public ACE_Task<ACE_MT_SYNCH> {
   private:
      std::list<SmartACE::SmartComponent*> componnents;
      ACE_Thread_Mutex mutex;
      bool stopped;
   public:
      ShutdownTask();
      int add_component(SmartACE::SmartComponent *comp);
      int open();
      int svc();
      int stop();
   }shutdownTask;

public:
   ShutdownManager();
   virtual ~ShutdownManager();

   virtual int manage_component(SmartACE::SmartComponent *comp);

   virtual int handle_signal(int signum, siginfo_t * = 0, ucontext_t * = 0);
   virtual int handle_timeout(const ACE_Time_Value &current_trime, const void *act = 0);

   virtual int run();
};

#endif /* SHUTDOWNMANAGER_H_ */
