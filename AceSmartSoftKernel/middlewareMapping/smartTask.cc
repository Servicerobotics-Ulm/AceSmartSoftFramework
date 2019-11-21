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

#include "smartTask.hh"

#include "smartOSMapping.hh"

// using ACE_OS::sleep(...)
#include <ace/OS_NS_unistd.h>


SmartACE::Task::Task(Smart::IComponent *component)
:	thread_started(false)
,	Smart::ITask(component)
{
	// check if a proper pointer has been used, otherwise use the task without a component
	if(component != 0) {
		this->attach_self_to(component);
	}
}

SmartACE::Task::~Task()
{  }

int SmartACE::Task::svc(void) {
	return this->task_execution();
}

int SmartACE::Task::start() {
	ACE_GUARD_RETURN(SmartRecursiveMutex, guard, mutex, -1);

	if(!thread_started)
	{
		int retval = this->activate
				(
					THR_NEW_LWP  // initialize a kernel-level thread with FIFO realtime scheduler
					,1			 // initialize exactly one thread
					,0			 // do not force to activate if already activated earlier
					,ACE_DEFAULT_THREAD_PRIORITY // use the minimal FIFO priority (usually 0)
				);
		if(retval != -1) {
			thread_started = true;
			return 0;
		}
	} else {
		// thread already started => return 0
		return 0;
	}

	return -1;
}

int SmartACE::Task::start(const ACE_Sched_Params &sched_params, const int &cpuAffinity) {
	ACE_GUARD_RETURN(SmartRecursiveMutex, guard, mutex, -1);

	// create new default thread
	int retval = this->start();
	// if successful, set-up scheduler
	if(retval != -1) {
		retval = this->setSchedParams(sched_params);
	}
	// if again successful, set-up CPU-affinity
	if(retval != -1 && cpuAffinity != -1) {
		return this->setCpuAffinity(cpuAffinity);
	}
	return retval;
}

int SmartACE::Task::stop(const bool wait_till_stopped)
{
	ACE_GUARD_RETURN(SmartRecursiveMutex, guard, mutex, -1);
	int retval = 0;
	if(thread_started == true) {
		retval = ACE_Thread_Manager::instance()->cancel_task(this);
		if(wait_till_stopped == true) {
			retval = ACE_Thread_Manager::instance()->wait_task(this);
		}
		thread_started = false;
	}
	return retval;
}

bool SmartACE::Task::test_canceled()
{
	return ACE_Thread_Manager::instance()->testcancel(ACE_Thread::self());
}

void SmartACE::Task::sleep_for(const Smart::Duration &rel_time)
{
	ACE_OS::sleep(convertToAceTimeFrom(rel_time));
}

int SmartACE::Task::setSchedParams(const ACE_Sched_Params &sched_params)
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

int SmartACE::Task::setCpuAffinity(const int &cpuCore) {
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
