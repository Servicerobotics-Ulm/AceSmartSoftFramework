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

#include "aceSerializationExamples.hh"

// -------------------------------------------------------------------
//
// global variables
//
// -------------------------------------------------------------------
SmartACE::SmartComponent *component;


//
// handler class for time queries.
//
class TimeQueryHandler : public SmartACE::QueryServerHandler<SmartACE::CommExampleTime,SmartACE::CommExampleTime>
{
public:
	TimeQueryHandler(SmartACE::QueryServer<SmartACE::CommExampleTime,SmartACE::CommExampleTime> *server)
	: SmartACE::QueryServerHandler<SmartACE::CommExampleTime,SmartACE::CommExampleTime>(server)
	  { }
  void handleQuery(const SmartACE::QueryId &id, const SmartACE::CommExampleTime& r) 
    {
      SmartACE::CommExampleTime a;

      std::cout << "time service " << id << " received time: ";
      r.print();

      //
      // very slow time service to show how a blocking client
      // communication method can be aborted by the state pattern
      //
      std::cout << "now wait for 6 seconds ..." << std::endl;

      ACE_OS::sleep(6);

      a.set_now();

      std::cout << "time service " << id << " sent answer time: ";
      a.print();

      server->answer(id,a);
    };
};


// -------------------------------------------------------------------
//
//
//
// -------------------------------------------------------------------
int main (int argc, char *argv[])
{

  try {
    component = new SmartACE::SmartComponent("exampleComponent22",argc,argv);
    // Create an object
    SmartACE::QueryServer<SmartACE::CommExampleTime,SmartACE::CommExampleTime> timeServant(component,"time");
    TimeQueryHandler timeHandler(&timeServant);

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

