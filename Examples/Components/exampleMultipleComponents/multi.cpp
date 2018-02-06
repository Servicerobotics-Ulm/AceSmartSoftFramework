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

#include "../../../AceSmartSoftKernel/aceSmartSoft.hh"
#include "ShutdownManager.h"

#include "ServerComponent.h"
#include "ClientComponent.h"

int main(int argc, char * argv[])
{
   try {
      ShutdownManager manager;

      ServerComponent server("ServerComponent", argc, argv);
      ClientComponent client("ClientComponent", argc, argv);

      manager.manage_component(server.getComponentRef());
      manager.manage_component(client.getComponentRef());

      // in this case first the server is started and then the client component
      server.start();
      client.start();

      manager.run();
   }catch(std::exception &ex){
      std::cerr << ex.what() << std::endl;
   }catch(...){
      std::cerr << "Uncaught exception..." << std::endl;
   }

	return 0;
}
