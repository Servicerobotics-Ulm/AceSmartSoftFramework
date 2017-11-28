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
// This is another example of a communication class where one uses the IDL
// data structure only for communication purposes. Therefore the IDL data
// structure is filled within the mandatory get / set methods.
//
#ifndef _COMM_EXAMPLE_VALUES_HH
#define _COMM_EXAMPLE_VALUES_HH

#include <string>

#include "ace/Message_Block.h"

#include "exampleValues.hh"

namespace SmartACE {

class CommExampleValues
{
protected:
  ExampleValues values;

public:
  CommExampleValues();
  virtual ~CommExampleValues();

  //<alexej date="26.11.2008">
  void get(ACE_Message_Block *&msg) const;
  void set(const ACE_Message_Block *msg);
  //</alexej>

  static inline std::string identifier(void) {
    return "SmartACE::exampleValues";
  };


  //
  // user interface
  //
  void set(std::list<int>);
  void get(std::list<int>&) const;
};

}

#endif

