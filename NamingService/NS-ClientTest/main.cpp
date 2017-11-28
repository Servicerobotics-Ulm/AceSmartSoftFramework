// --------------------------------------------------------------------------
//
//  Copyright (C) 2009 Alex Lotz
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


// Test the client-side of the ACE Name Server...
#include "smartNSAdapterACE.hh"

#include <iostream>

//---------------------------------------------------------
//
//
//---------------------------------------------------------
int
ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
	ACE_Utils::UUID_Generator uuid_genrator;
	ACE_Utils::UUID *service_identifier;
	uuid_genrator.init();
	service_identifier = uuid_genrator.generate_UUID();

	if(SmartACE::NAMING::instance()->init(argc, argv) == 0) {
		std::cout << "Naming service successfully initialized!" << std::endl;

		ACE_INET_Addr addr((u_short)12345, "127.0.0.1");

		std::cout << "Local Hostname: " << addr.get_host_name()
				<< "; Hostaddr: " << addr.get_host_addr()
				<< "; portnr: " << addr.get_port_number()
				<< std::endl;

		SmartACE::NSKeyType key;
		key.names[SmartACE::NSKeyType::COMP_NAME] = "MyComponent";
		key.names[SmartACE::NSKeyType::PATTERN_NAME] = "PushNewestServer";
		key.names[SmartACE::NSKeyType::SERVICE_NAME] = "ImageServer";
		key.names[SmartACE::NSKeyType::COMMOBJ1_NAME] = "CommImage";
		SmartACE::NSValueType value(addr, *service_identifier);
		SmartACE::NSValueType retVal;

		if(SmartACE::NAMING::instance()->rebind(key, value) == 0) {
			std::cout << "REBIND MyComponent was successful" << std::endl;
		} else {
			std::cout << "REBIND MyComponent ERROR" << std::endl;
		}
		std::cout << "sleep(1)..." << std::endl;
		ACE_OS::sleep(1);

		key.names[SmartACE::NSKeyType::COMMOBJ1_NAME] = "COMMTEST";
		if(SmartACE::NAMING::instance()->rebind(key, value) == 0) {
			std::cout << "REBIND MyComponent TEST2 was successful" << std::endl;
		} else {
			std::cout << "REBIND MyComponent TEST2 ERROR" << std::endl;
		}
		std::cout << "sleep(1)..." << std::endl;
		ACE_OS::sleep(1);

		SmartACE::NSKeyType searchPattern;
		ACE_Unbounded_Queue<SmartACE::NSKeyType> entries =
				SmartACE::NAMING::instance()->getEntriesForMatchingPattern(searchPattern);

		while(!entries.is_empty()) {
			SmartACE::NSKeyType entry;
			entries.dequeue_head(entry);
			std::cout << "received entry: " << entry << std::endl;
		}

		std::cout << "sleep(1)..." << std::endl;
		ACE_OS::sleep(1);

		if(SmartACE::NAMING::instance()->resolve(key, retVal) == 0) 
		{
			std::cout << "Resolved value: " << retVal << std::endl;
		} else {
			std::cout << "Resolve FAILED!" << std::endl;
		}

		delete service_identifier;

		ACE_OS::sleep(1);
		std::cout << "NS->fini()..." << std::endl;
		SmartACE::NAMING::instance()->fini();
	} else {
		std::cout << "Naming service initialization ERROR!" << std::endl;
	}
  return 0;
}


