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

#include "ServerTask.h"

ServerTask::ServerTask(SmartACE::SmartComponent *comp)
:  SmartACE::ManagedTask(comp)
,  server(comp, "MyTestServer")
{

}

ServerTask::~ServerTask()
{

}

int ServerTask::on_execute()
{
   local_list.clear();
   local_list.push_back(x_pos);
   local_comm_obj.set(local_list);

   if(server.put(local_comm_obj) == SmartACE::SMART_OK)
   {
      std::cout << "<ServerTask> send x_pos: " << x_pos << "; sleep(1)..."<< std::endl;
   }

   x_pos++;

   ACE_OS::sleep(1);

   return 0;
}
