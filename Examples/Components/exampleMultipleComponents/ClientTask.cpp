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

#include "ClientTask.h"

ClientTask::ClientTask(SmartACE::SmartComponent *comp)
:  SmartACE::ManagedTask(comp)
,  client(comp)
{

}

ClientTask::~ClientTask()
{

}

int ClientTask::on_entry()
{
   while(client.connect("ServerComponent", "MyTestServer") != SmartACE::SMART_OK)
   {
      std::cout << "<ClientTask> Try to reconnect to server <ServerComponent:MyTestServer> after sleep(1)..." << std::endl;
      ACE_OS::sleep(1);
   }

   if(client.subscribe() == SmartACE::SMART_OK) return 0;

   return -1;
}

int ClientTask::on_execute()
{
   status = client.getUpdateWait(local_comm_obj);
   if(status == SmartACE::SMART_OK)
   {
      local_list.clear();
      local_comm_obj.get(local_list);

      // sleep is not necessary at all here, however leads to more readable console outputs in this example
      ACE_OS::sleep(ACE_Time_Value(0,1000));
      std::cout << "<ClientTask> received value: " << local_list.front() << std::endl;
   }else{
      std::cout << "<ClientTask> getUpdateWait() failed with " << SmartACE::StatusCodeConversion(status) << std::endl;
      ACE_OS::sleep(ACE_Time_Value(0, 50000));
   }

   return 0;
}

int ClientTask::on_exit()
{
   status = client.disconnect();
   if(status == SmartACE::SMART_OK)
   {
      std::cout << "<ClientTask> disconnect successful" << std::endl;
   }else{
      std::cout << "<ClientTask> disconnect failed with " << SmartACE::StatusCodeConversion(status) << std::endl;
   }
   return 0;
}
