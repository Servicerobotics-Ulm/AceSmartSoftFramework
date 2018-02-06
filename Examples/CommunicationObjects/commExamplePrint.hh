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

//
//
//
// Example of a communication object whose data structure "Print" is
// described using IDL.
//
//
//

#ifndef _COMM_EXAMPLE_PRINT_HH
#define _COMM_EXAMPLE_PRINT_HH

#include <iostream>
#include <iomanip>
#include <string>

#include "examplePrint.hh"

namespace SmartACE {

class CommExamplePrint
{
public:
  ExamplePrint message;
	
  //
  // constructors, destructors, copy constructors etc. ...
  //
  CommExamplePrint();
  virtual ~CommExamplePrint();

  //
  // The following methods MUST be available in a communication object.
  // This however is not too bad for implementers of a communication
  // object since you can get cookbook like instructions on how
  // to implement these. They are always the same since they have to
  // set / get the above IDL structure. They are used by the communication
  // patterns and should not be used by users.
  //
  
  //<alexej date="13.11.2008">
  static inline std::string identifier(void) {
    return "SmartACE::examplePrint";
  };
  //</alexej>


  //
  // The following methods provide the user interface. They are provided
  // by the developer of the communication object and provide everything
  // needed to work with that object. Those methods do not necessarily
  // return CORBA types but can e.g. convert CORBA sequences of the
  // IDL data structure into STL vectors ...
  //

  void get(int&,int&,int&,std::string& text) const;
  void set(int,int,int,std::string);
  void print(std::ostream &os = std::cout) const;
};

}

#endif

