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
#include "commExampleTime.hh"
#include "commExampleValues.hh"
#include "commExampleResult.hh"

#include "aceSerializationExamples.hh"

#ifndef DEBUG
#define DEBUG
#endif

// -------------------------------------------------------------------
//
// global variables
//
// -------------------------------------------------------------------
SmartACE::SmartComponent *component;


//
// handler class for the first query service of this component
//
//
class TimeQueryHandler : public SmartACE::QueryServerHandler<SmartACE::CommExampleTime,SmartACE::CommExampleTime>
{
public:
  void handleQuery(IQueryServer &server, const Smart::QueryIdPtr& id, const SmartACE::CommExampleTime& r)
    {
      SmartACE::CommExampleTime a;
#ifdef DEBUG
      std::cout << "time service " << id << " received time: ";
      r.print();
#endif
      time_t time_now = time(0);
	  struct tm *time_p = ACE_OS::gmtime(&time_now);

      a.set_now();

#ifdef DEBUG
      std::cout << "time service " << id << " sent answer time: ";
      a.print();
#endif
      server.answer(id,a);
    }
};

//
// handler of the second query service of this component
//
// receives a variable list of values which are summed up and the
// result is returned to the caller
//

class SumQueryHandler :  public SmartACE::QueryServerHandler<SmartACE::CommExampleValues,SmartACE::CommExampleResult>
{
public:
  void handleQuery(IQueryServer &server, const Smart::QueryIdPtr& id, const SmartACE::CommExampleValues& r)

    {
      SmartACE::CommExampleResult a;
      std::vector<int>         l;
      int                    result;
#ifdef DEBUG
      std::cout << "calc service " << id << std::endl;
#endif
      r.get(l);
      result = 0;
      for (std::vector<int>::iterator i=l.begin();i!=l.end();++i) {
	      result += *i;
      }
      a.set(result);
#ifdef DEBUG
      std::cout << "calc service " << id << " sent answer " << result << std::endl;
#endif
      server.answer(id,a);
    }
};


// -------------------------------------------------------------------
//
//
//
// -------------------------------------------------------------------
int main (int argc, char *argv[])
{

  try {
     component = new SmartACE::SmartComponent("exampleComponent1",argc,argv);

    // Create time query and its handler
    auto timeHandler = std::make_shared<TimeQueryHandler>();
    SmartACE::QueryServer<SmartACE::CommExampleTime,SmartACE::CommExampleTime> timeServant(component,"time-handler", timeHandler);

    // Create sum servant and its handler
    auto calcHandler = std::make_shared<SumQueryHandler>();
    SmartACE::QueryServer<SmartACE::CommExampleValues,SmartACE::CommExampleResult> calcServant(component,"calc-handler",calcHandler);

    component->run();
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

