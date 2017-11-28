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


// -------------------------------------------------------------------
//
// derive the state class and provide handler functions
//
// -------------------------------------------------------------------
class PrintStateChangeHandler : public SmartACE::StateChangeHandler
{
public:
  void handleEnterState(const std::string & s) throw()
    {
      std::cout << "    enterHandler  activate   " << s << std::endl;
    };
  void handleQuitState(const std::string & s) throw()
    {
      std::cout << "    quitHandler   deactivate " << s << std::endl;
    };
};


// -------------------------------------------------------------------
//
// global variables
//
// -------------------------------------------------------------------
SmartACE::SmartComponent *component;
SmartACE::QueryClient<SmartACE::CommExampleTime,SmartACE::CommExampleTime> *timeClient;

PrintStateChangeHandler *stateHandler;
SmartACE::StateSlave *state;


// -------------------------------------------------------------------
//
// thread A requires substate "odd" to run ...
//
// -------------------------------------------------------------------
class UserThreadA : public SmartACE::ManagedTask
{
private:
  Smart::StatusCode status;
  int v;
public:
  UserThreadA(SmartACE::SmartComponent *comp)
  :  SmartACE::ManagedTask(comp)
  {
	  status = Smart::SMART_OK;
	  v = 0;
  };

  ~UserThreadA() {};

  int on_entry();
  int on_execute(void);
};

int UserThreadA::on_entry()
{
  v=1;
  return 0;
}

int UserThreadA::on_execute(void)
{
  status = state->acquire("odd");
  if (status ==  Smart::SMART_OK) {
    v += 2;
    std::cout << "odd " << v << std::endl;
  }
  status = state->release("odd");

  this->sleep_for(std::chrono::milliseconds(100));

  return 0;
}


// -------------------------------------------------------------------
//
// thread B requires substate "even" to run ...
//
// -------------------------------------------------------------------
class UserThreadB : public SmartACE::ManagedTask
{
private:
  Smart::StatusCode status;
  int v;
public:
  UserThreadB(SmartACE::SmartComponent *comp)
  :  SmartACE::ManagedTask(comp)
  {
	  status = Smart::SMART_OK;
	  v=0;
  };

  ~UserThreadB() {};

  int on_entry();
  int on_execute(void);
};

int UserThreadB::on_entry()
{
  v=0;
  return 0;
}

int UserThreadB::on_execute(void)
{
  status = state->acquire("even");
  if (status ==  Smart::SMART_OK) {
    v += 2;
    std::cout << "even " << v << std::endl;
  }
  status = state->release("even");

  this->sleep_for(std::chrono::milliseconds(100));

  return 0;
}


// -------------------------------------------------------------------
//
// thread C runs as long as we are not in the neutral state
//
// since this thread uses a communication pattern, it demonstrates
// how blocking method calls are cancelled if the neutral state is
// enforced and how the state management waits until all substates
// are released if the state management requests a new mainstate.
//
// -------------------------------------------------------------------
class UserThreadC : public SmartACE::ManagedTask
{
private:
  SmartACE::CommExampleTime r,a;
  Smart::StatusCode status;

public:
  UserThreadC(SmartACE::SmartComponent *comp)
  :  SmartACE::ManagedTask(comp)
  {
	  status = Smart::SMART_OK;
  };

  ~UserThreadC() {};

  int on_entry();
  int on_execute(void);
};

int UserThreadC::on_entry()
{
  // arbitrary test-time
  r.set(1,2,3);
  return 0;
}

int UserThreadC::on_execute(void)
{
    status = state->acquire("nonneutral");
    if (status ==  Smart::SMART_OK) {
      //
      // we use two blocking queries within the protected section
      // to demonstrate that both blocking queries are cancelled
      // if the neutral state is enforced. In that case the user
      // activity rushes as fast as possible over all blocking
      // communication methods to reach the substate release in
      // an ordered manner as fast as possible
      //

      //
      // blocking query
      //
      std::cout << "time : 1st call : ask for current time ..." << std::endl;

      status = timeClient->query(r,a);

      if (status ==  Smart::SMART_OK) {
        std::cout << "time : received answer : "; a.print();
      } else {
        std::cout << "time : no answer, query status : " << status << std::endl;
      }

      //
      // blocking query
      //
      std::cout << "time : 2nd call : ask for current time ..." << std::endl;

      status = timeClient->query(r,a);

      if (status ==  Smart::SMART_OK) {
        std::cout << "time : received answer : "; a.print();
      } else {
        std::cout << "time : no answer, query status : " << status << std::endl;
      }
    
    }
    status = state->release("nonneutral");

    this->sleep_for(std::chrono::milliseconds(100));

  return 0;
}


// -------------------------------------------------------------------
//
//
//
// -------------------------------------------------------------------
int main (int argc, char *argv[])
{

  try {
    component = new SmartACE::SmartComponent("exampleComponent20",argc,argv);
    
    // Create an object
    timeClient = new SmartACE::QueryClient<SmartACE::CommExampleTime,SmartACE::CommExampleTime>(component,"exampleComponent22","time");

    stateHandler = new PrintStateChangeHandler();
    state = new SmartACE::StateSlave(component,stateHandler);

    if (state->defineStates("Both","odd")  !=  Smart::SMART_OK) std::cerr << "ERROR: define state" << std::endl;
    if (state->defineStates("Both","even") !=  Smart::SMART_OK) std::cerr << "ERROR: define state" << std::endl;
    if (state->defineStates("Odd","odd")   !=  Smart::SMART_OK) std::cerr << "ERROR: define state" << std::endl;
    if (state->defineStates("Even","even") !=  Smart::SMART_OK) std::cerr << "ERROR: define state" << std::endl;

    if (state->activate() !=  Smart::SMART_OK) std::cerr << "ERROR: activate state" << std::endl;

    //
    //
    //
    UserThreadA user1(component);
    UserThreadB user2(component);
    UserThreadC user3(component);

    //
    //
    //
    user1.start();
    user2.start();
    user3.start();

    if (state->setWaitState("Alive") !=  Smart::SMART_OK) std::cerr << "ERROR: set state to Alive" << std::endl;
    component->run();
  } catch (const std::exception &ex) {
    std::cerr << ex.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  delete state;
  delete stateHandler;
  delete component;

  return 0;
}

