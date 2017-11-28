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

SmartACE::StateMaster *stateMaster;


// -------------------------------------------------------------
//
// -------------------------------------------------------------
class UserThread : public SmartACE::ManagedTask
{
private:
  std::list<std::string> mainstates;
  std::list<std::string> substates;

  SmartACE::StatusCode status;

public:
  UserThread(SmartACE::SmartComponent *comp) 
  :  SmartACE::ManagedTask(comp)
  {};
  
~UserThread() {};

  int on_execute(void);
};

int UserThread::on_execute(void)
{
    //
    // first get list of main states and check whether component is
    // already activated
    //
    status = stateMaster->getAllMainStates(mainstates, "exampleComponent20");
    while (status != SmartACE::SMART_OK) {
      std::cout << "status still " << SmartACE::StatusCodeConversion(status) << " ... wait another second" << std::endl;
      ACE_OS::sleep(1);
      status = stateMaster->getAllMainStates(mainstates, "exampleComponent20");
    }

    std::cout << std::endl;
    std::cout << "list of states of the slave component : " << std::endl;

    for (std::list<std::string>::iterator iterator1=mainstates.begin();iterator1 != mainstates.end(); ++iterator1) {
      std::cout << "   mainstate : " << *iterator1 << std::endl;
      status = stateMaster->getSubStates(*iterator1,substates, "exampleComponent20");

      for (std::list<std::string>::iterator iterator2=substates.begin();iterator2 != substates.end(); ++iterator2) {
        std::cout << "      substate : " << *iterator2 << std::endl;
      }
    }

    std::cout << std::endl;
    std::cout << "Now wait 5 seconds ..." << std::endl;

    ACE_OS::sleep(5);


    //
    // now set main state
    //
    std::cout << "set mainstate <Both> ..."; std::cout.flush();
    status = stateMaster->setWaitState("Both", "exampleComponent20");
    if (status == SmartACE::SMART_OK) {
      std::cout << "... mainstate <Both> set" << std::endl;
    } else {
      std::cout << "... error set mainstate <Both> : " << SmartACE::StatusCodeConversion(status) << " " << status << std::endl;
    }

    std::cout << "now wait 20 secs ..." << std::endl;
    ACE_OS::sleep(20);

    std::cout << "set mainstate <Odd> ..."; std::cout.flush();
    status = stateMaster->setWaitState("Odd", "exampleComponent20");
    if (status == SmartACE::SMART_OK) {
      std::cout << "... mainstate <Odd> set" << std::endl;
    } else {
      std::cout << "... error set mainstate <Odd> : " << SmartACE::StatusCodeConversion(status) << " " << status << std::endl;
    }

    std::cout << "now wait 20 secs ..." << std::endl;
    ACE_OS::sleep(20);

    std::cout << "set mainstate <Even> ..."; std::cout.flush();
    status = stateMaster->setWaitState("Even", "exampleComponent20");
    if (status == SmartACE::SMART_OK) {
      std::cout << "... mainstate <Even> set" << std::endl;
    } else {
      std::cout << "... error set mainstate <Even> : " << SmartACE::StatusCodeConversion(status) << " " << status << std::endl;
    }

    std::cout << "now wait 20 secs ..." << std::endl;
    ACE_OS::sleep(20);

    std::cout << "ordered component deactivation ..."; std::cout.flush();
    status = stateMaster->setWaitState("Neutral", "exampleComponent20");
    if (status == SmartACE::SMART_OK) {
      std::cout << "... component deactivated (Neutral state)" << std::endl;
    } else {
      std::cout << "... error deactivate component : " << SmartACE::StatusCodeConversion(status) << " " << status << std::endl;
    }

    std::cout << "now wait 20 secs ..." << std::endl;
    ACE_OS::sleep(20);

    std::cout << "set mainstate <Both> ..."; std::cout.flush();
    status = stateMaster->setWaitState("Both", "exampleComponent20");
    if (status == SmartACE::SMART_OK) {
      std::cout << "... mainstate <Both> set" << std::endl;
    } else {
      std::cout << "... error set mainstate <Both> : " << SmartACE::StatusCodeConversion(status) << " " << status << std::endl;
    }

    std::cout << "now wait 20 secs ..." << std::endl;
    ACE_OS::sleep(20);

    std::cout << "forced component deactivation ..."; std::cout.flush();
    status = stateMaster->setWaitState("deactivated", "exampleComponent20");
    if (status == SmartACE::SMART_OK) {
      std::cout << "... forced deactivation done (Neutral)" << std::endl;
    } else {
      std::cout << "... error forced deactivation : " << SmartACE::StatusCodeConversion(status) << " " << status << std::endl;
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

    component = new SmartACE::SmartComponent("exampleComponent21",argc,argv);

    stateMaster = new SmartACE::StateMaster(component);

    //
    //
    //
    UserThread user(component);

    //
    //
    //
    user.start();

    component->run();
    threadManager->wait();
  } catch  (const std::exception &ex) {
     std::cerr << ex.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  return 0;
}

