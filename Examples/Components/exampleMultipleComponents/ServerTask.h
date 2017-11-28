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

#ifndef SERVERTASK_H_
#define SERVERTASK_H_

#include <smartSoft.hh>

#include <commExampleValues.hh>

class ServerTask : public SmartACE::ManagedTask {
protected:
   int x_pos;
   std::list<int> local_list;

   SmartACE::CommExampleValues local_comm_obj;
   SmartACE::PushNewestServer<SmartACE::CommExampleValues> server;
public:
   ServerTask(SmartACE::SmartComponent *comp);
   virtual ~ServerTask();

   inline int on_entry() { x_pos = 0; return 0; }
   int on_execute();
};

#endif /* SERVERTASK_H_ */
