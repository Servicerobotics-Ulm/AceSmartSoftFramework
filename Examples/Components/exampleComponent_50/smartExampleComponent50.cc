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
#include "commExamplePrint.hh"

#include "aceSerializationExamples.hh"

//
// handler of the send service of this component
//
//
class PrintHandler : public SmartACE::SendServerHandler<SmartACE::CommExamplePrint>
{
public:
	PrintHandler(SmartACE::SendServer<SmartACE::CommExamplePrint> *server)
	: SmartACE::SendServerHandler<SmartACE::CommExamplePrint>(server)
	  { }
  void handleSend(const SmartACE::CommExamplePrint& r) 
    {
      SmartACE::CommExamplePrint a;
      std::cout << "print service received time and message: ";
      r.print();
    }
};

// -------------------------------------------------------------------
//
// global variables
//
// -------------------------------------------------------------------


SmartACE::SmartComponent *component;

SmartACE::SendServer<SmartACE::CommExamplePrint> *printServant;
PrintHandler *sendHandler;



// -------------------------------------------------------------------
//
//
//
// -------------------------------------------------------------------
int main (int argc, char *argv[])
{
	  try {
	   component = new SmartACE::SmartComponent("exampleComponent50",argc,argv);


      printServant = new SmartACE::SendServer<SmartACE::CommExamplePrint>(component,"PrintServer");
      sendHandler = new PrintHandler(printServant);


		//<alexej description="Test2: Timeout (comment out Test1 and uncomment following lines to test the delete fkt."/>
		//Test *test = new Test();
		//test->open();

	    component->run();

	    delete sendHandler;
	    delete printServant;

     } catch (std::exception &e) {
       std::cerr << e.what() << std::endl;
       return 1;
	  } catch (...) {
	    std::cerr << "Uncaught exception..." << std::endl;
	    return 1;
	  }

	delete component;

  return 0;
}

