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

#include "smartSoft.hh"

#include "commExampleTime.hh"

// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
SmartACE::SmartComponent *component;

SmartACE::PushClient<SmartACE::CommExampleTime> *timeClient;


// -------------------------------------------------------------
//
// -------------------------------------------------------------
class UserThreadA : public SmartACE::ManagedTask
{
private:
  SmartACE::CommExampleTime a;
  Smart::StatusCode status;

  int counter;

public:
  UserThreadA()
  : SmartACE::ManagedTask(component)
  {
	  status = Smart::SMART_OK;
	  counter = 0;
  };
  ~UserThreadA() {};

  int on_entry();
  int on_execute();
  int on_exit();
};

int UserThreadA::on_entry()
{
    timeClient = new SmartACE::PushClient<SmartACE::CommExampleTime>(component);

    counter = 0; 
    
    return 0;
}

int UserThreadA::on_execute()
{
    if (counter==0) {
      status = timeClient->connect("exampleComponent10","TimeExample");
      std::cout << "connect status    : " << status << std::endl;
    }
    if (counter==10) {
      std::cout << "subscribe()  status : " << timeClient->subscribe() << std::endl;
    }
    if (counter==15) {
      std::cout << "unsubscribe status: " << timeClient->unsubscribe() << std::endl;
    }
    if (counter==20) {
      std::cout << "subscribe(2)  status : " << timeClient->subscribe(2) << std::endl;
    }
    if (counter==25) {
      std::cout << "disconnect status : " << timeClient->disconnect() << std::endl;
    }
    if (counter > 30) {
      counter = -1;
    }

    status = timeClient->getUpdateWait(a);
    if (status != Smart::SMART_OK) {
      std::cout << "blocking wait  status " << status << " not ok => retry ..." << std::endl;
    } else {
      std::cout << "blocking wait  status " << status << " ";a.print();
    }

    counter++;

    return 0;
}

int UserThreadA::on_exit()
{
   delete timeClient;
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
    SmartACE::SmartThreadManager *threadManager = SmartACE::SmartThreadManager::instance();

    component = new SmartACE::SmartComponent("exampleComponent11",argc,argv);


    //
    //
    //
    UserThreadA user;

    //
    //
    //
    user.start();

    component->run();
    threadManager->wait();
  } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
      return 1;
  } catch (...) {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  delete component;

  return 0;
}

