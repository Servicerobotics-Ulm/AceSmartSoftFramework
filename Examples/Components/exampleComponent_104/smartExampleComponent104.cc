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

// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
SmartACE::SmartComponent *component;


// -------------------------------------------------------------
//
// -------------------------------------------------------------
class UserThread : public SmartACE::ManagedTask
{
private:
	  SmartACE::WiringMaster wiringmaster;
	  Smart::StatusCode   status;
public:
  UserThread()
  : SmartACE::ManagedTask(component)
  , wiringmaster(component)
  {
	  status = Smart::SMART_OK;
  };
  ~UserThread() {};
  int on_entry();
  int on_execute();
};

int UserThread::on_entry()
{
	std::cout << "Demonstrate dynamic wiring" << std::endl;
	std::cout << std::endl;
	std::cout << "Does not use the state class and therefore rewiring is" << std::endl;
	std::cout << "done without making sure that the client is in a suitable" << std::endl;
	std::cout << "state for changing the server connections." << std::endl;
	std::cout << std::endl;
	return 0;
}

int UserThread::on_execute()
{



  while (1) {
    std::cout << "slave ports are not wired ... " << std::endl;

    std::cout << std::endl << "sleep 5 seconds ..." << std::endl << std::endl;
	 ACE_OS::sleep(2);

    std::cout << "CONnect <calcPort> of <exampleComponent102> to <exampleComponent101,calc> ..." << std::endl;
    status = wiringmaster.connect("exampleComponent102","calcPort","exampleComponent101","calc");
    std::cout << "... : " << status << std::endl;

    std::cout << std::endl << "sleep 5 seconds ..." << std::endl << std::endl;
    ACE_OS::sleep(5);

    std::cout << "CONnect <timePort> of <exampleComponent102> to <exampleComponent101,time> ..." << std::endl;
    status = wiringmaster.connect("exampleComponent102","timePort","exampleComponent101","time");
    std::cout << "... : " << status << std::endl;

    std::cout << std::endl << "sleep 5 seconds ..." << std::endl << std::endl;
    ACE_OS::sleep(5);

    std::cout << "CONnect <calcPort> of <exampleComponent102> to <exampleComponent103,calc> ..." << std::endl;
    status = wiringmaster.connect("exampleComponent102","calcPort","exampleComponent103","calc");
    std::cout << "... : " << status << std::endl;

    std::cout << std::endl << "sleep 5 seconds ..." << std::endl << std::endl;
    ACE_OS::sleep(5);

    std::cout << "CONnect <timePort> of <exampleComponent102> to <exampleComponent103,time> ..." << std::endl;
    status = wiringmaster.connect("exampleComponent102","timePort","exampleComponent103","time");
    std::cout << "... : " << status << std::endl;

    std::cout << std::endl << "sleep 5 seconds ..." << std::endl << std::endl;
    ACE_OS::sleep(5);

    std::cout << "DISconnect <calcPort> of <exampleComponent102> ..." << std::endl;
    status = wiringmaster.disconnect("exampleComponent102","calcPort");
    std::cout << "... : " << status << std::endl;

    std::cout << std::endl << "sleep 5 seconds ..." << std::endl << std::endl;
    ACE_OS::sleep(5);

    std::cout << "DISconnect <timePort> of <exampleComponent102> ..." << std::endl;
    status = wiringmaster.disconnect("exampleComponent102","timePort");
    std::cout << "... : " << status << std::endl;
  }

  return 0;
};


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

    component  = new SmartACE::SmartComponent("exampleComponent104",argc,argv);

    //
    //
    //
    UserThread user;

    //
    //
    //
    user.start();

    component->run();
  } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
      return 1;
  } catch (...) {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  return 0;
}

