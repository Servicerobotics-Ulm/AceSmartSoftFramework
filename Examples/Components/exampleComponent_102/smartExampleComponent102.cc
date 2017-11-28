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

#include "commExampleValues.hh"
#include "commExampleResult.hh"
#include "commExampleTime.hh"

// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
SmartACE::SmartComponent *component;

SmartACE::WiringSlave *wiring;

SmartACE::QueryClient<SmartACE::CommExampleTime,SmartACE::CommExampleTime>     *timeClient;
SmartACE::QueryClient<SmartACE::CommExampleValues,SmartACE::CommExampleResult> *calcClient;


// -------------------------------------------------------------
//
// -------------------------------------------------------------
class UserThreadA : public SmartACE::Task
{
public:
  UserThreadA()
  : SmartACE::Task(component)
  {};
  ~UserThreadA() {};
  int task_execution();
};

int UserThreadA::task_execution()
{
  SmartACE::QueryId  id1, id2;
  SmartACE::CommExampleTime q1,q2;
  SmartACE::CommExampleTime a1,a2;

  Smart::StatusCode status1, status2;

  time_t time_now;
  struct tm *time_p;

  while(1) {
    //
    // interleaved queries to ask for time
    //
    time_now = time(0);
    time_p   = ACE_OS::gmtime(&time_now);

    q1.set(time_p->tm_hour,time_p->tm_min,time_p->tm_sec);
    status1 = timeClient->queryRequest(q1,id1);

    ACE_OS::sleep(1);

    time_now = time(0);
    time_p   = ACE_OS::gmtime(&time_now);

    q2.set(time_p->tm_hour,time_p->tm_min,time_p->tm_sec);
    status2 = timeClient->queryRequest(q2,id2);

    std::cout << "thread A REQUEST (status1, id1): " << status1 << " " << id1 << std::endl;
    std::cout << "thread A REQUEST (status2, id2): " << status2 << " " << id2 << std::endl;

    //
    // try to get answer only if queryRequest has been successful
    //
    if (status1 == Smart::SMART_OK) status1 = timeClient->queryReceiveWait(id1,a1);
    if (status2 == Smart::SMART_OK) status2 = timeClient->queryReceiveWait(id2,a2);

    if (status1 == Smart::SMART_OK) {
      std::cout << "thread A ANSWER  (status1, id1): " << status1 << " " << id1 << " "; a1.print();
    } else {
      std::cout << "thread A ANSWER  (status1, id1): " << status1 << std::endl;
      ACE_OS::sleep(ACE_Time_Value(0,10000));
    }

    if (status1 == Smart::SMART_CANCELLED) {
      timeClient->queryDiscard(id1);
    }

    if (status2 == Smart::SMART_OK) {
      std::cout << "thread A ANSWER  (status2, id2): " << status2 << " " << id2 << " "; a2.print();
    } else {
      std::cout << "thread A ANSWER  (status2, id2): " << status2 << std::endl;
      ACE_OS::sleep(ACE_Time_Value(0,10000));
    }

    if (status2 == Smart::SMART_CANCELLED) {
      timeClient->queryDiscard(id2);
    }
  }

  return 0;
};


// -------------------------------------------------------------
//
// -------------------------------------------------------------
class UserThreadB : public SmartACE::Task
{
public:
  UserThreadB()
  : SmartACE::Task(component)
  {};
  ~UserThreadB() {};
  int task_execution();
};

int UserThreadB::task_execution()
{
  int i=0;
  std::list<int> l;

  SmartACE::CommExampleValues q;
  SmartACE::CommExampleResult r;

  Smart::StatusCode status;

  while(1) {
    l.clear();
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);

    q.set(l);

    std::cout << "thread B " << i++ << std::endl;

    status = calcClient->query(q,r);

    if (status == Smart::SMART_OK) {
      std::cout << "thread B (status): " << status << " result ";r.print();
    } else {
      std::cout << "thread B (status): " << status << std::endl;
      ACE_OS::sleep(ACE_Time_Value(0,10000));
    }
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
    SmartACE::SmartThreadManager *threadManager = SmartACE::SmartThreadManager::instance();

    component  = new SmartACE::SmartComponent("exampleComponent102",argc,argv);

    //
    // this component can export its connections to required servers as ports
    // which can then be wired from outside
    //
    wiring     = new SmartACE::WiringSlave(component);

    timeClient = new SmartACE::QueryClient<SmartACE::CommExampleTime,SmartACE::CommExampleTime>(component);
    calcClient = new SmartACE::QueryClient<SmartACE::CommExampleValues,SmartACE::CommExampleResult>(component);

    //
    // add both server connections to the list of externally wireable ports
    //
    timeClient->add(wiring, "timePort");
    calcClient->add(wiring, "calcPort");

    //
    //
    //
    UserThreadA user1;
    UserThreadB user2;

    //
    //
    //
    user1.open();
    user2.open();

    component->run();
    threadManager->wait();

  } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
      return 1;
  } catch (...) {
    std::cerr << "Uncaught exception" << std::endl;
    return 1;
  }

  return 0;
}

