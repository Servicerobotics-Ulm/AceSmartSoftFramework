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

#include "ace/Message_Block.h"

#include "exampleTime.hh"

namespace SmartACE {

class CommExampleTime
{
protected:
  //
  // This is the data structure which is described in the IDL. Therefore
  // CORBA can marshal / unmarshal this type for communication purposes.
  //
  ExampleTime time;

  //
  // We can add further data structures here but only those data structures
  // described in IDL and being accessible by the get/set-methods are
  // transmitted. Currently I assume that everything to be transmitted
  // is wrapped by a single structure. This is no restriction since
  // IDL interfaces can easily composed of several structures.
  //
public:
  //
  // constructors, destructors, copy constructors etc. ...
  //
  CommExampleTime();
  virtual ~CommExampleTime();

  //
  // The following methods MUST be available in a communication object.
  // This however is not too bad for implementers of a communication
  // object since you can get cookbook like instructions on how
  // to implement these. They are always the same since they have to
  // set / get the above IDL structure. They are used by the communication
  // patterns and should not be used by users.
  //

  //<alexej date="26.11.2008">
  void get(ACE_Message_Block *&msg) const;
  void set(const ACE_Message_Block *msg);
  //</alexej>

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

  void get(int&,int&,int&);
  void set(int,int,int);

  void print(std::ostream &os = std::cout) const;
};

inline std::ostream &operator<< (std::ostream &os,const CommExampleTime &c) {
  c.print(os);
  return os;
};

}

#endif

