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

#ifndef SERVERCOMPONENT_H_
#define SERVERCOMPONENT_H_

#include <smartSoft.hh>

#include "ServerTask.h"

class ServerComponent : public SmartACE::ManagedTask {
protected:
	SmartACE::SmartComponent comp;

	ServerTask server;
public:
	ServerComponent(const std::string &comp_name, int argc, char * argv[]);
	virtual ~ServerComponent();

	int on_entry() { return server.start(); }
	int on_execute();

	inline SmartACE::SmartComponent *getComponentRef() { return &comp; }
};

#endif /* SERVERCOMPONENT_H_ */
