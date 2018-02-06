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
// Example of a communication object whose data structure "TimeOfDay" is
// described using IDL.
//
//
//

#ifndef _COMM_EXAMPLE_TIME_HH
#define _COMM_EXAMPLE_TIME_HH

#include <iostream>
#include <iomanip>
#include <string>

#include <chrono>

#include "ace/Message_Block.h"

#include "exampleTime.hh"

namespace SmartACE {

class CommExampleTime
{
public:
  ExampleTime time;
  //
  // constructors, destructors, copy constructors etc. ...
  //
  CommExampleTime();
  virtual ~CommExampleTime();

  static inline std::string identifier(void) {
    return "SmartACE::exampleTime";
  };


  //
  // The following methods provide the user interface. They are provided
  // by the developer of the communication object and provide everything
  // needed to work with that object. Those methods do not necessarily
  // return CORBA types but can e.g. convert CORBA sequences of the
  // IDL data structure into STL vectors ...
  //

//  void get(int&,int&,int&);
//  void set(int,int,int);

  void set_now();
  std::chrono::system_clock::time_point get_timepoint();

  void print(std::ostream &os = std::cout) const;
};

inline std::ostream &operator<< (std::ostream &os,const CommExampleTime &c) {
  c.print(os);
  return os;
};

}

#endif

