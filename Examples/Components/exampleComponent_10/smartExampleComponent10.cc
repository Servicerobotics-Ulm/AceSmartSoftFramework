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
//
//
// -------------------------------------------------------------------
SmartACE::SmartComponent *component;
SmartACE::PushServer<SmartACE::CommExampleTime> *timeUpdate;


class UserThreadA : public SmartACE::ManagedTask
{
private:
	  Smart::StatusCode status;
	  int i;

	  SmartACE::CommExampleTime a;
public:
  UserThreadA()
  :	SmartACE::ManagedTask(component)
  {
	  i=0;
	  status = Smart::SMART_OK;
  };
  virtual ~UserThreadA() {};
  virtual int on_execute();
};


int UserThreadA::on_execute()
{
	a.set_now();
	status = timeUpdate->put(a);

	std::cout << "thread A <push new data> " << i++ << " status: " << status << " ";a.print();

	this->sleep_for(std::chrono::seconds(1));
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
    component = new SmartACE::SmartComponent("exampleComponent10",argc,argv);

    // Create an object
    timeUpdate = new SmartACE::PushServer<SmartACE::CommExampleTime>(component,"TimeExample");

    //
    //
    //
    UserThreadA user1;

    //
    //
    //
    user1.start();

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

