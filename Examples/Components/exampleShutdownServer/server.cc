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
 
// SmartSoft
#include "aceSmartSoft.hh"
#include <commExampleTypes.hh> 
#include "aceSerializationExamples.hh"

class ServerTask: public SmartACE::ManagedTask 
{ 
private:
   SmartACE::SmartComponent *component;
   SmartACE::PushServer<SmartACE::CommExampleTypes> *server;
   int count;

   SmartACE::CommExampleTypes types;
 
public: 
   ServerTask(SmartACE::SmartComponent *comp) 
   : SmartACE::ManagedTask(comp)
   , component(comp)
   , server(0)
   , count(0)
   { 
      //server = new SmartACE::PushNewestServer<SmartACE::CommExampleTypes>(comp, "Testserver"); 
   } 
 
   ~ServerTask() 
   { 
      //delete server;
   } 
 
   int on_entry()
   {
      server = new SmartACE::PushServer<SmartACE::CommExampleTypes>(component, "Testserver");

      types.text = "ServerTask"; 
 
      types.data.cdr_bool = true; 
      types.data.cdr_char = 'A'; 
      types.data.cdr_float = 1.5; 
      types.data.cdr_dbl = 100.12345; 
      types.data.cdr_long = -123; 
      types.data.cdr_longlong = 456; 
      types.data.cdr_ulong = 789; 
      types.data.cdr_ulonglong = 159; 
      types.data.cdr_ushort = 5; 

      return 0;
   }

   int on_execute() 
   { 
         if( server->put(types) == Smart::SMART_OK) {
            ACE_OS::printf("PushMessage:%d\n\n", count);
            types.print_data(); 
         } 
 
         ACE_OS::sleep(ACE_Time_Value(0, 500000)); 
         //ACE_OS::sleep(3); 
      return 0; 
   } 
 
   int on_exit()
   { 
      delete server;
      return 0; 
   } 
}; 
 
int main(int argc, char* argv[]) 
{ 
   try { 
      SmartACE::SmartComponent comp("exampleShutdownServer", argc, argv); 
 
      ServerTask server(&comp); 
      server.start(); 
 
      // start component management 
      comp.run(); 
   } catch (std::exception &e) { 
      std::cerr << e.what() << std::endl; 
      return 1; 
   }catch(...) { 
      std::cerr << "Uncought exception..." << std::endl; 
   } 
 
 
   return 0; 
} 
