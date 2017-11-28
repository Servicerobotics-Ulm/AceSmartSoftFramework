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
#include "commExampleValues.hh"
#include "commExampleResult.hh"

// -------------------------------------------------------------
//
//
//
// -------------------------------------------------------------
SmartACE::SmartComponent *component;

SmartACE::QueryClient<SmartACE::CommExampleTime,SmartACE::CommExampleTime>     *timeClient;
SmartACE::QueryClient<SmartACE::CommExampleValues,SmartACE::CommExampleResult> *calcClient;

#ifndef DEBUG
#define DEBUG
#endif

// -------------------------------------------------------------
//
// -------------------------------------------------------------
class UserThreadA : public SmartACE::ManagedTask
{
private:
  SmartACE::QueryId    id1, id2;
  SmartACE::CommExampleTime q1,q2;
  SmartACE::CommExampleTime a1,a2;

  Smart::StatusCode status1, status2;

  time_t time_now;
  struct tm *time_p;
public:
  UserThreadA(SmartACE::SmartComponent *component)
  : SmartACE::ManagedTask(component)
  ,	time_now(0)
  {
	  status1 = Smart::SMART_OK;
	  status2 = Smart::SMART_OK;
	  id1 = 0;
	  id2 = 0;
	  time_p = 0;
  };
  ~UserThreadA() {};

  int on_execute();
  int on_exit();
};

int UserThreadA::on_execute()
{
    //
    // interleaved queries
    //
    time_now = time(0);
	 time_p   = ACE_OS::gmtime(&time_now);

    q1.set(time_p->tm_hour,time_p->tm_min,time_p->tm_sec);
    status1 = timeClient->queryRequest(q1,id1);

    time_now = time(0);
	 time_p   = ACE_OS::gmtime(&time_now);

    q2.set(time_p->tm_hour,time_p->tm_min,time_p->tm_sec);
    status2 = timeClient->queryRequest(q2,id2);

#ifdef DEBUG
    std::cout << "thread A (status1, id1): " << status1 << " " << id1 << std::endl;
    std::cout << "thread A (status2, id2): " << status2 << " " << id2 << std::endl;
#endif

    status1 = timeClient->queryReceiveWait(id1,a1);
    status2 = timeClient->queryReceiveWait(id2,a2);

    if (status1 == Smart::SMART_CANCELLED) timeClient->queryDiscard(id1);
    if (status2 == Smart::SMART_CANCELLED) timeClient->queryDiscard(id2);

#ifdef DEBUG
    std::cout << "thread A (status1, id1): " << status1 << " " << id1 << " "; a1.print();
    std::cout << "thread A (status2, id2): " << status2 << " " << id2 << " "; a2.print();
#endif

    ACE_OS::sleep(ACE_Time_Value(0, 1000));

  return 0;
};

int UserThreadA::on_exit()
{
   delete timeClient;
   return 0;
}


// -------------------------------------------------------------
//
// -------------------------------------------------------------
class UserThreadB : public SmartACE::ManagedTask
{
private:
  int i;
  std::list<int> l;

  SmartACE::CommExampleValues q;
  SmartACE::CommExampleResult r;

  SmartACE::QueryId    id;
  Smart::StatusCode status;

public:
  UserThreadB(SmartACE::SmartComponent *component)
  : SmartACE::ManagedTask(component)
  , i(0)
  ,	id(0)
  ,	status(Smart::SMART_OK)
  {};
  ~UserThreadB() {};

  int on_entry() { i=0; return 0; };
  int on_execute();
  int on_exit() {  delete calcClient; return 0; }
};

int UserThreadB::on_execute()
{
    //
    // first query
    //
    l.clear();
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);

    q.set(l);
#ifdef DEBUG
    std::cout << "thread B first query " << i++ << std::endl;
#endif
    status = calcClient->query(q,r);
#ifdef DEBUG
    std::cout << "thread B first query (status): " << status << " result ";r.print();
#endif
    //
    // second query
    //
    l.clear();
    l.push_back(10);
    l.push_back(11);
    l.push_back(12);

    q.set(l);

#ifdef DEBUG
    std::cout << "thread B second query " << i++ << std::endl;
#endif

    status = calcClient->queryRequest(q,id);

#ifdef DEBUG
    std::cout << "thread B second query request (status): " << status << std::endl;
    std::cout << "thread B second query : ... request done ... " << std::endl;
#endif

    status = calcClient->queryReceiveWait(id,r);

    if (status == Smart::SMART_CANCELLED) calcClient->queryDiscard(id);

#ifdef DEBUG
    std::cout << "thread B second query result (status): " << status << " result ";r.print();
#endif

    ACE_OS::sleep(ACE_Time_Value(0, 1000));

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
    component = new SmartACE::SmartComponent("exampleComponent2",argc,argv);

    //
    //
    //
    timeClient = new SmartACE::QueryClient<SmartACE::CommExampleTime,SmartACE::CommExampleTime>(component,"exampleComponent1","time-handler");

    //
    //
    //
    calcClient = new SmartACE::QueryClient<SmartACE::CommExampleValues,SmartACE::CommExampleResult>(component,"exampleComponent1","calc-handler");

    //
    //
    //
    UserThreadA user1(component);
    UserThreadB user2(component);

    //
    //
    //
    user1.start();
    user2.start();

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

