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
#include "commExampleEvent1.hh"
#include "commExampleEvent2.hh"

#include "aceSerializationExamples.hh"

// -------------------------------------------------------------
//
//
//
//
//
// -------------------------------------------------------------
SmartACE::SmartComponent *component;

SmartACE::EventClient<SmartACE::CommExampleEvent1Parameter,SmartACE::CommExampleEvent1Result> *eventClient1;
SmartACE::EventClient<SmartACE::CommExampleEvent2Parameter,SmartACE::CommExampleEvent2Result> *eventClient2;
SmartACE::EventClient<SmartACE::CommExampleEvent1Parameter,SmartACE::CommExampleEvent1Result> *eventClient3;


// -------------------------------------------------------------
//
// activates continuous "More Than" event
//
// -------------------------------------------------------------
class UserThreadA : public SmartACE::ManagedTask
{
private:
  SmartACE::CommExampleEvent1Parameter parameter;
  SmartACE::CommExampleEvent1Result    result;
  SmartACE::EventId               id;

  Smart::StatusCode status;
  int a;
public:
  UserThreadA()
  : SmartACE::ManagedTask(component)
  {
  	status = Smart::SMART_OK;
  	id = 0;
  	a = 0;
  };
  ~UserThreadA() {};
  int on_entry();
  int on_execute();
  int on_exit();
};

int UserThreadA::on_entry()
{
  // parameter will be set to a:
  a = 20;

  //
  // continuous event
  //
  parameter.set(a);

  status = eventClient1->activate(Smart::continuous, parameter, id);
  std::cout << "activate event1 with parameter: " << a << "; status: " << status << std::endl;

  return 0;
}

int UserThreadA::on_execute()
{
    // result is strored in b:
    int b;

    // wait on event to occure
    status = eventClient1->getEvent(id, result);
    if (status == Smart::SMART_OK) {
      // get result value of fired event
      result.get(b);
      std::cout << "event MORETHAN id: " << id << " event mode: continuous, > " << a << " fired: " << b << "\n";
    }

  return 0;
}

int UserThreadA::on_exit()
{
  status = eventClient1->deactivate(id);
  return 0;
}


// -------------------------------------------------------------
//
// activates / deactivates single "More Than" event
//
// -------------------------------------------------------------
class UserThreadB : public SmartACE::ManagedTask
{
  private:
    SmartACE::CommExampleEvent1Parameter parameter;
    SmartACE::CommExampleEvent1Result    result;
    SmartACE::EventId               id;

    Smart::StatusCode status;
    int a, b;
  public:
    UserThreadB()
    : SmartACE::ManagedTask(component)
    {
    	status = Smart::SMART_OK;
    	id = 0;
    	a = 0; b = 0;
    };
    ~UserThreadB() {};
    int on_entry();
    int on_execute();
    int on_exit();
};

int UserThreadB::on_entry()
{
  a = 30;

  //
  // single event
  //
  parameter.set(a);

  status = eventClient1->activate(Smart::single, parameter, id);

  return 0;
}

int UserThreadB::on_execute()
{
  status = eventClient1->getEvent(id, result);
  if (status == Smart::SMART_OK) {
    result.get(b);
    std::cout << "event MORETHAN id: " << id << " event mode: single, > " << a << " fired: " << b << "\n";
  }

  status = eventClient1->getEvent(id, result);
  std::cout << "event MORETHAN id: " << id << " event mode: single, > " << a << " status getEvent: " << status << "\n";

  status = eventClient1->getNextEvent(id, result);
  std::cout << "event MORETHAN id: " << id << " event mode: single, > " << a << " status getNextEvent: " << status << "\n";

  return -1;
}

int UserThreadB::on_exit()
{
  status = eventClient1->deactivate(id);
  std::cout << "event MORETHAN id: " << id << " deactivated: " << status << std::endl;
  return 0;
}


// -------------------------------------------------------------
//
// activates continuous interval event in [25,40]
//
// -------------------------------------------------------------
class UserThreadC : public SmartACE::ManagedTask
{
  private:
    SmartACE::CommExampleEvent1Parameter parameter1;
    SmartACE::CommExampleEvent2Parameter parameter2;
    SmartACE::CommExampleEvent2Result    result;
    SmartACE::EventId               id1;
    SmartACE::EventId               id2;

    Smart::StatusCode status;
    int l, u;
  public:
    UserThreadC()
    : SmartACE::ManagedTask(component)
    {
    	status = Smart::SMART_OK;
    	id1 = 0; id2 = 0;
    	l = 0; u = 0;
    };
    ~UserThreadC() {};
    int on_entry();
    int on_execute();
    int on_exit();
};


int UserThreadC::on_entry()
{
  l = 25;
  u = 40;

   //
   // demonstrate event handler object
   //
   // activate continuous "more than" event with threshold 40 with event handler
   // event handler is used only for eventClient3 (and not for eventClient1 which is the same event on the server side !)
   //
   int v = 40;

   parameter1.set(v);
   status = eventClient3->activate(Smart::continuous, parameter1, id1);



  //
  // continuous event
  //
  parameter2.set(l,u);

  status = eventClient2->activate(Smart::continuous, parameter2, id2);
  return 0;
}

int UserThreadC::on_execute()
{
    std::string b;
    status = eventClient2->getEvent(id2, result);
    if (status == Smart::SMART_OK) {
      result.get(b);
      std::cout << "event INTERVAL id: " << id2 << " event mode: continuous, [" << l << " " << u << "] fired: " << b << "\n";
    }

  return 0;
};

int UserThreadC::on_exit()
{
  status = eventClient3->deactivate(id1);
  status = eventClient2->deactivate(id2);
  return 0;
}


// -------------------------------------------------------------
//
// demonstrates event handler usage
//
// -------------------------------------------------------------

// -------------------------------------------------------------
// event handler service method
// -------------------------------------------------------------

class MoreThanEventHandler:
  public SmartACE::EventHandler<SmartACE::CommExampleEvent1Result>
{
public:
	MoreThanEventHandler()
	:	SmartACE::EventHandler<SmartACE::CommExampleEvent1Result>(eventClient3)
	{ }
  void handleEvent(const SmartACE::EventId &id, const SmartACE::CommExampleEvent1Result& e) throw()
    {
      int a;

      e.get(a);
      std::cout << "event handler MORE THAN: id " << id << " event value: " << a << std::endl;
    };
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
    component = new SmartACE::SmartComponent("exampleComponent41",argc,argv);


    eventClient1 = new SmartACE::EventClient<SmartACE::CommExampleEvent1Parameter,SmartACE::CommExampleEvent1Result>(component,"exampleComponent40","eventMoreThan");
    eventClient2 = new SmartACE::EventClient<SmartACE::CommExampleEvent2Parameter,SmartACE::CommExampleEvent2Result>(component,"exampleComponent40","eventInterval");
    eventClient3 = new SmartACE::EventClient<SmartACE::CommExampleEvent1Parameter,SmartACE::CommExampleEvent1Result>(component,"exampleComponent40","eventMoreThan");
    MoreThanEventHandler eventHandler;

    //
    //
    //
    UserThreadA user1;
    UserThreadB user2;
    UserThreadC user3;

    //
    //
    //
    user1.start();
    user2.start();
    user3.start();

    component->run();
  } catch  (const std::exception &ex) {
     std::cerr << ex.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  delete eventClient1;
  delete eventClient2;
  delete eventClient3;
  delete component;

  return 0;
}

