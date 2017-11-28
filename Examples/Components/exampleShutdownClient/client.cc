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
#include "smartSoft.hh" 
#include "commExampleTypes.hh" 
 
class ClientPatternTask: public SmartACE::ManagedTask 
{ 
private: 
   SmartACE::PushNewestClient<SmartACE::CommExampleTypes> client; 
   int count; 
public: 
   ClientPatternTask(SmartACE::SmartComponent *comp); 
   virtual ~ClientPatternTask(); 
 
   virtual int on_entry() { return 0; }
   virtual int on_execute(); 
   virtual int on_exit(); 
}; 
 
ClientPatternTask::ClientPatternTask(SmartACE::SmartComponent *comp) 
:  client(comp)   // initialize client-side communication-port 
{  } 
 
ClientPatternTask::~ClientPatternTask() 
{  } 
 
int ClientPatternTask::on_execute() 
{ 
   // do not use infinite loop inside this function, otherwise shutdown wont work. 
   SmartACE::StatusCode ret = SmartACE::SMART_OK; 
 
      // 1) perform a clean connect, analysing the return values 
      ret = client.connect("exampleShutdownServer", "Testserver"); 
 
      switch(ret) 
      { 
      case SmartACE::SMART_OK: 
      case SmartACE::SMART_ERROR_COMMUNICATION: 
      case SmartACE::SMART_INCOMPATIBLESERVICE: 
      case SmartACE::SMART_SERVICEUNAVAILABLE: 
      case SmartACE::SMART_ERROR: 
         std::cout << "ClientPatternTask::push_client::connect(): " << SmartACE::StatusCodeConversion(ret) << std::endl;
         break;
      default: 
         // this case is actualy not needed but prevents a warning with GCC-compiler 
         std::cout << "ClientPatternTask::push_client::connect(): other error" << std::endl; 
      }; 
 
      // 2) perform a subscribe (it returns a fail retcode if port is not connected) 
      ret = client.subscribe(); 
 
      if( ret == SmartACE::SMART_OK ) { 
         std::cout << "ClientPatternTask::push_client: sucessfull subscribed!" << std::endl; 
 
         // local comm-object
         SmartACE::CommExampleTypes types;  

         // 3) Blocking wait on incomming messages. Breaks up if strg+c is pressed. 
         ret = client.getUpdateWait(types);

         if( ret == SmartACE::SMART_OK ) { 
            std::cout << "ClientPatternTask: message from server...\n" << std::endl; 
            types.print_data(); 
         }else{ 
            std::cout << "ClientPatternTask: getUpdate failed! " << SmartACE::StatusCodeConversion(ret) << std::endl; 
         } 
      }else{ 
         // if not connected or other failure sleep 50 ms to prevent 100% CPU load 
         ACE_OS::sleep(ACE_Time_Value(0, 500000)); 
      } 
 
   // return != 0 breaks up the svc loop 
   return 0; 
}

int ClientPatternTask::on_exit() 
{ 
   client.disconnect();
   return 0;
}
 
// ------------------------------------------------------------------- 
// 
// 
// 
// ------------------------------------------------------------------- 
int main(int argc, char* argv[]) 
{ 
   try { 
      // Create Component 
      SmartACE::SmartComponent comp("exampleShutdownClient", argc, argv); 
 
      // UserTask with pattern 
      ClientPatternTask *task = new ClientPatternTask(&comp); 
      task->start(); 
 
      // Start component (with reactor) 
      comp.run(); 
      delete task;
   } catch (std::exception &e) { 
      std::cerr << e.what() << std::endl; 
      return 1; 
   }catch(...) { 
      std::cerr << "Uncought exception..." << std::endl; 
   } 
 
   return 0; 
} 
