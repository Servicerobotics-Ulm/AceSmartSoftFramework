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

#ifndef REMOTENSTASK_H_
#define REMOTENSTASK_H_

#include <smartTask.hh>
#include <smartNSProxy.hh>
#include <smartIniParameter.hh>

#include "NameAcceptor.h"

#include <map>
#include <set>
#include <string>

class RemoteNSTask : public SmartACE::Task {
public:
	RemoteNSTask();
	virtual ~RemoteNSTask();

	void setAcceptor(NameAcceptor* acc);

	int init(const int &argc, char** argv);
	int fini(void);

	void postponeLocalRebind(NameHandler *ptr, const ACE_CString &name, const ACE_CString &value, const ACE_CString &type);
	void propagateRemoteCommand(const int &cmdId, const std::string &name, const std::string &value="", const std::string &type="");

	void insertLocalEntry(const std::string &name);
	void removeLocalEntry(const std::string &name);
	void cleanUpAllLocalEntries();

	int createEndpoint(const std::string &address);

	ACE_TString getRemoteNsType() const { return "remote_ns"; }

	inline bool isInitialized() const { return initialized; }

protected:
	virtual int task_execution();

	bool first_call_of_handle_signal;
	virtual int handle_signal (int signum, siginfo_t *, ucontext_t *);

private:
	bool initialized;
	bool synchronized;
	NameAcceptor* acceptor;

	ACE_Connector<SmartACE::NSProxy, ACE_SOCK_CONNECTOR> connector;
	SmartACE::SmartMutex remote_ns_mutex;
	std::map<std::string, SmartACE::NSProxy*> remote_ns_list;

	std::set<std::string> local_entries;

	SmartACE::SmartIniParameter parameter;

	struct LocalRebind {
		NameHandler *ptr;
		ACE_CString name;
		ACE_CString value;
		ACE_CString type;
	};

	SmartACE::SmartMutex local_rebind_mutex;
	std::list<LocalRebind> postponed_rebinds;

	struct Request {
		int command;
		std::string name;
		std::string value;
		std::string type;
	};

	SmartACE::SmartSemaphore request_sema;
	SmartACE::SmartMutex requsts_mutex;
	std::list<Request> requests;


	int synchronize_with_remote_ns(const std::string &address);

	void finish_postponed_rebinds();
};

typedef ACE_Singleton<RemoteNSTask, ACE_Thread_Mutex> REMOTE_NS;

#endif /* REMOTENSTASK_H_ */
