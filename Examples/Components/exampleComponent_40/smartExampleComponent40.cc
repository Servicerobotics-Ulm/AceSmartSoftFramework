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

// -------------------------------------------------------------------
//
// global variables
//
// -------------------------------------------------------------------
SmartACE::SmartComponent *component;

SmartACE::EventServer<SmartACE::CommExampleEvent1Parameter,SmartACE::CommExampleEvent1Result,SmartACE::ExampleEventState> *exampleEvent1;
SmartACE::EventServer<SmartACE::CommExampleEvent2Parameter,SmartACE::CommExampleEvent2Result,SmartACE::ExampleEventState> *exampleEvent2;


//
// defining test event function
//
class MoreThanTestHandler :
  public SmartACE::EventTestHandler<SmartACE::CommExampleEvent1Parameter, SmartACE::CommExampleEvent1Result, SmartACE::ExampleEventState>
{
public:
  bool testEvent(SmartACE::CommExampleEvent1Parameter& p, SmartACE::CommExampleEvent1Result& e, const SmartACE::ExampleEventState& s) throw()
  {
    //
    // fire if current counter value is greater or equal to the event parameter
    // when fired return sum of current counter value and event threshold
    //
    int pa, st;

    p.get(pa);
    s.get(st);

    if ( st > pa) {
      e.set ( st );
      return true;
    }
    return false;
  }
};

class IntervalTestHandler :
  public SmartACE::EventTestHandler<SmartACE::CommExampleEvent2Parameter, SmartACE::CommExampleEvent2Result, SmartACE::ExampleEventState>
{
public:
  bool testEvent(SmartACE::CommExampleEvent2Parameter& p, SmartACE::CommExampleEvent2Result& e, const SmartACE::ExampleEventState& s) throw()
    {
      //
      // report when current counter enters / leaves specified interval
      //
      int pl, pu, ps;
      int st;

      bool result;

      std::string inside("inside");
      std::string outside("outside");

      p.get(pl,pu);
      p.get(ps);
      s.get(st);

      switch (ps) {
      case 0:
	// old state: unknown
	if ((st >= pl) && (st <= pu)) {
	  // now inside => fire inside, new state: inside
	  p.set(1);
	  e.set(inside);
	  result = true;
	} else {
	  // now outside => fire outside, new state: outside
	  p.set(2);
	  e.set(outside);
	  result = true;
	}
	break;
      case 1:
	// old state: inside, fire only if now outside
	if ((st < pl) || (st > pu)) {
	  // now outside
	  p.set(2);
	  e.set(outside);
	  result = true;
	} else {
	  // same state as before
	  result = false;
	}
	break;
      case 2:
	// old state: outside, fire only if now inside
	if ((st >= pl) && (st <= pu)) {
	  // now inside
	  e.set(inside);
	  p.set(1);
	  result = true;
	} else {
	  // same state as before
	  result = false;
	}
	break;
      default:
	// wrong state
	result = false;
	break;
      }

      return result;
    }
};

//
// thread which performs computations which feed the event
//
class EventThread : public SmartACE::ManagedTask
{
private:
  SmartACE::ExampleEventState st;
  int cnt;

public:
  EventThread(SmartACE::SmartComponent *component)
  : SmartACE::ManagedTask(component)
  {
	  cnt = 0;
  };
  ~EventThread() {};
  int on_entry();
  int on_execute();
  int on_exit();
};

int EventThread::on_entry()
{
   cnt = 0;
   return 0;
}

int EventThread::on_execute(void)
{
    // compute new state
    cnt++;
    cnt %= 50;

    // put current state into event server object
    std::cout << "internal counter value : " << cnt << std::endl;

    st.set( cnt );
    exampleEvent1->put(st);
    exampleEvent2->put(st);

    ACE_OS::sleep(1);
  return 0;
}

int EventThread::on_exit()
{
   delete exampleEvent1;
   delete exampleEvent2;

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
    // Create an object
    component = new SmartACE::SmartComponent("exampleComponent40",argc,argv);

    EventThread  userThread(component);
    
    MoreThanTestHandler moreThanTester;
    exampleEvent1 = new SmartACE::EventServer<SmartACE::CommExampleEvent1Parameter,SmartACE::CommExampleEvent1Result,SmartACE::ExampleEventState>(component,"eventMoreThan",&moreThanTester);

    IntervalTestHandler intervallTester;
    exampleEvent2 = new SmartACE::EventServer<SmartACE::CommExampleEvent2Parameter,SmartACE::CommExampleEvent2Result,SmartACE::ExampleEventState>(component,"eventInterval",&intervallTester);

    userThread.start();

    component->run();
  } catch (const std::exception &ex) {
    std::cerr << ex.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  delete component;

  return 0;
}

