// --------------------------------------------------------------------------
//
//  Copyright (C) 2002/2004/2008 Christian Schlegel
//                2009 Alex Lotz
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

#include "aceSmartSoft.hh"

#include "commExamplePrint.hh"
#include "aceSerializationExamples.hh"

#include <iostream>

// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
SmartACE::SmartComponent *component;


// -------------------------------------------------------------
//
// -------------------------------------------------------------
class UserThreadA : public SmartACE::ManagedTask
{
private:
	SmartACE::SendClient<SmartACE::CommExamplePrint> *printClient;

  SmartACE::CommExamplePrint m;
  time_t time_now;
  struct tm *time_p;

  int counter;

public:
  UserThreadA()
  : SmartACE::ManagedTask(component)
  ,	time_now()
  {
	  counter = 0;
	  time_p = 0;
	  printClient = 0;
  };
  ~UserThreadA() {};

  int on_entry();
  int on_execute();
};

int UserThreadA::on_entry(void)
{
   printClient = new SmartACE::SendClient<SmartACE::CommExamplePrint>(component);
   counter = 0;
   return 0;
}

int UserThreadA::on_execute()
{
  // stop thread here if counter becomes bigger than 4
  if(counter++ >4) return -1;

    // send message
    time_now = time(0);
	 time_p   = ACE_OS::gmtime(&time_now);

    m.set(time_p->tm_hour,time_p->tm_min,time_p->tm_sec,"Thread A");


	if( printClient->connect("exampleComponent50","PrintServer") == Smart::SMART_OK ) {
    	printf("ThreadA: connection established\n");
    }
    if( printClient->send(m) == Smart::SMART_OK ) {
    	printf("ThreadA: send(message) performed successfully\n");
    }
    if( printClient->disconnect() == Smart::SMART_OK ) {
    	printf("ThreadA: disconnect() successful\n");
    }

	 ACE_OS::sleep(1);

  return 0;
};




class UserThreadB : public SmartACE::ManagedTask
{
private:
	int counter;
	SmartACE::SendClient<SmartACE::CommExamplePrint> *printClient;

  SmartACE::CommExamplePrint m;

  time_t time_now;
  struct tm *time_p;

  Smart::StatusCode status;

public:
  UserThreadB()
  : SmartACE::ManagedTask(component)
  ,	time_now()
  {
	  counter = 0;
	  time_p = 0;
	  printClient = 0;
	  status = Smart::SMART_OK;
  };
  ~UserThreadB() {};

  int on_entry();
  int on_execute();
  int on_exit();
};

int UserThreadB::on_entry()
{
   printClient = new SmartACE::SendClient<SmartACE::CommExamplePrint>(component,"exampleComponent50","PrintServer");
   return 0;
}

int UserThreadB::on_execute()
{
    if(counter++ > 11) return -1;

    // send message
    time_now = time(0);
	 time_p   = ACE_OS::gmtime(&time_now);

    m.set(time_p->tm_hour,time_p->tm_min,time_p->tm_sec,"Thread B");

    status = printClient->send(m);
    if( status == Smart::SMART_OK ) {
    	printf("ThreadB: send(message) performed successfully\n");
    }

    ACE_OS::sleep(ACE_Time_Value(0, 500000));

  return 0;
};

int UserThreadB::on_exit()
{
   status = printClient->disconnect();
   if( status == Smart::SMART_OK ) {
	   printf("ThreadB: disconnect() successful\n");
   }
   return 0;
}

// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
int main (int argc, char *argv[])
{
  //
  //
  //
  try {
	component = new SmartACE::SmartComponent("exampleComponent51",argc,argv);

    //
    //
    //
    UserThreadA user1;
    UserThreadB user2;

    //
    //
    //
    user1.start();
    user2.start();

    component->run();
    ACE_Thread_Manager::instance()->wait();
  } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
      return 1;
  } catch (...) {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  return 0;
}

