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
	TimeQueryHandler(SmartACE::QueryServer<SmartACE::CommExampleTime,SmartACE::CommExampleTime>* server)
	: SmartACE::QueryServerHandler<SmartACE::CommExampleTime,SmartACE::CommExampleTime>(server)
	{ }
  void handleQuery(const SmartACE::QueryId &id, const SmartACE::CommExampleTime& r) 
    {
      SmartACE::CommExampleTime a;

      std::cout << "time service " << id << " received time: ";
      r.print();

      a.set_now();

      std::cout << "time service " << id << " sent answer time: ";
      a.print();

      server->answer(id,a);
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
	SumQueryHandler(SmartACE::QueryServer<SmartACE::CommExampleValues,SmartACE::CommExampleResult>* server)
	: SmartACE::QueryServerHandler<SmartACE::CommExampleValues,SmartACE::CommExampleResult>(server)
	{ }
  void handleQuery(const SmartACE::QueryId &id, const SmartACE::CommExampleValues& r) 

    {
      SmartACE::CommExampleResult a;
      std::vector<int>    l;
      int               result;

      std::cout << "calc service " << id << std::endl;

      r.get(l);
      result = 0;
      for (std::vector<int>::iterator i=l.begin();i!=l.end();++i) {
		result += *i;
      }
      a.set(result);

      std::cout << "calc service " << id << " sent answer " << result << std::endl;

      server->answer(id,a);
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
    component = new SmartACE::SmartComponent("exampleComponent101",argc,argv);

    // Create time query and its handler
    SmartACE::QueryServer<SmartACE::CommExampleTime,SmartACE::CommExampleTime> timeServant(component,"time");
    TimeQueryHandler timeHandler(&timeServant);

    // Create sum servant and its handler
    SmartACE::QueryServer<SmartACE::CommExampleValues,SmartACE::CommExampleResult> calcServant(component,"calc");
    SumQueryHandler calcHandler(&calcServant);
    // register the threaded calc handler

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

