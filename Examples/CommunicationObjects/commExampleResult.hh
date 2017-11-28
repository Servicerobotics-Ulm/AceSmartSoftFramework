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

#ifndef _COMM_EXAMPLE_RESULT_HH
#define _COMM_EXAMPLE_RESULT_HH

#include <iostream>
#include <string>

#include "ace/Message_Block.h"

#include "exampleResult.hh"

namespace SmartACE {

class CommExampleResult
{
protected:
  ExampleResult result;

public:
  CommExampleResult();
  virtual ~CommExampleResult();

  //<alexej date="26.11.2008">
  void get(ACE_Message_Block *&msg) const;
  void set(const ACE_Message_Block *msg);
  //</alexej>

  static inline std::string identifier(void) {
    return "SmartACE::exampleResult";
  };


  void set(int);
  void print(std::ostream &os = std::cout) const;
};

inline std::ostream &operator<<(std::ostream &os, const CommExampleResult &c) {
  c.print(os);
  return os;
};

}

#endif

