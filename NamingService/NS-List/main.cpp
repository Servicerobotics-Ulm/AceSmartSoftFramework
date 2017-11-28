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
#include <map>
#include <list>

//---------------------------------------------------------
//
//
//---------------------------------------------------------
int
ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
	if(SmartACE::NAMING::instance()->init(argc, argv) == 0) {
/*
		std::cout << "Naming service successfully initialized!" << std::endl;
		ACE_INET_Addr addr = SmartACE::NAMING::instance()->getLocalAddress();

		std::cout << "Local Hostname: " << addr.get_host_name()
				<< "; Hostaddr: " << addr.get_host_addr()
				<< "; portnr: " << addr.get_port_number()
				<< std::endl;
*/
		// get all entries
		SmartACE::NSKeyType searchPattern;
		ACE_Unbounded_Queue<SmartACE::NSKeyType> ns_entries = SmartACE::NAMING::instance()->getEntriesForMatchingPattern(searchPattern);
		std::map<std::string, std::list<SmartACE::NSKeyType> > components;
		std::map<std::string, std::list<SmartACE::NSKeyType> >::iterator it;

		std::cout << "Naming Service:" << std::endl;
		std::cout << "---------------" << std::endl;

		for (ACE_Unbounded_Queue_Iterator<SmartACE::NSKeyType> iter (ns_entries);
		   !iter.done ();
		   iter.advance ())
		{
			SmartACE::NSKeyType *comp = 0;
			iter.next (comp);
			std::string comp_name = comp->names[SmartACE::NSKeyType::COMP_NAME].c_str();
			components[comp_name].push_back(*comp);
		}

		for (it=components.begin(); it!=components.end(); ++it)	
		{
			std::cout << "\n########################" << std::endl;
			std::cout << "component: " << it->first << std::endl;
			for(std::list<SmartACE::NSKeyType>::iterator it2=it->second.begin(); it2!=it->second.end(); it2++) 
			{
				std::cout << "------------------------" << std::endl;
				std::cout << "Port-Type: " << it2->names[SmartACE::NSKeyType::PATTERN_NAME] << std::endl;
				std::cout << "Port-Name: " << it2->names[SmartACE::NSKeyType::SERVICE_NAME] << std::endl;
				std::cout << "COMM1: " << it2->names[SmartACE::NSKeyType::COMMOBJ1_NAME] << std::endl;
				if(it2->names[SmartACE::NSKeyType::COMMOBJ2_NAME] != "*")
					std::cout << "COMM2: " << it2->names[SmartACE::NSKeyType::COMMOBJ2_NAME] << std::endl;
				
				SmartACE::NSValueType value;
				SmartACE::NAMING::instance()->resolve(*it2, value);
				ACE_TCHAR adress[MAXNAMELEN];
				value.getInetAddr().addr_to_string(adress, MAXNAMELEN);
				std::cout << "Port-Address: " << adress << std::endl;
			}
		}

//		std::cout << "NS->fini()..." << std::endl;
		SmartACE::NAMING::instance()->fini();
	} else {
		std::cout << "Naming service initialization ERROR!" << std::endl;
		return -1;
	}
  return 0;
}


