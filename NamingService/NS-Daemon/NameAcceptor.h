// --------------------------------------------------------------------------
//
//  Copyright (C) 2014 Alex Lotz
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

#ifndef NAMEACCEPTOR_H_
#define NAMEACCEPTOR_H_

#include <ace/Acceptor.h>

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include <ace/SOCK_Acceptor.h>
#include <ace/svc_export.h>

#include "NameHandler.h"

template <typename T>
class LockingProxy {
private:
	T *object_;
	SmartACE::SmartRecursiveMutex &mutex;
public:
	LockingProxy(T *object, SmartACE::SmartRecursiveMutex &mutex)
	:	object_(object)
	,	mutex(mutex)
	{
		mutex.acquire();
	}
	~LockingProxy()
	{
		mutex.release();
	}

	T* operator->() const
	{
		return object_;
	}
};

/**
 * @class NameAcceptor
 *
 * @brief This class contains the service-specific methods that can't
 * easily be factored into the <ACE_Strategy_Acceptor>.
 */
class NameAcceptor : public ACE_Strategy_Acceptor<NameHandler, ACE_SOCK_ACCEPTOR>
{
public:
  /// Dynamic linking hook.
  virtual int init (int argc, ACE_TCHAR *argv[]);

  /// Parse svc.conf arguments.
  int parse_args (int argc, ACE_TCHAR *argv[]);

  /// Prints help message for command line arguments
  void printHelp(std::ostream &oss=std::cout);

  /// Naming context for acceptor /for the listening port/
  LockingProxy<ACE_Naming_Context> naming_context (void);

private:
  /// The scheduling strategy is designed for Reactive services.
  ACE_Schedule_All_Reactive_Strategy<NameHandler> scheduling_strategy_;

  /// The processor-scheduing policy
  int processor_scheduling_policy_;

  /// The processor-scheduing priority
  int processor_scheduling_priority_;

  /// The Naming Context
  ACE_Naming_Context naming_context_;

  SmartACE::SmartRecursiveMutex mutex_;
};

#endif /* NAMEACCEPTOR_H_ */
