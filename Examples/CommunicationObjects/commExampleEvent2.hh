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

#ifndef _COMM_EXAMPLE_EVENT2_HH
#define _COMM_EXAMPLE_EVENT2_HH

#include <iostream>
#include <string>

#include "exampleEvent2.hh"

namespace SmartACE {

// --------------------------------------------------------------------------
//
// event parameter part
//
// --------------------------------------------------------------------------
class CommExampleEvent2Parameter
{
public:
  ExampleEvent2Parameter parameter;
  int state;  // 0: unknown, 1: in interval, 2: out of interval

  CommExampleEvent2Parameter();
  virtual ~CommExampleEvent2Parameter();

  static inline std::string identifier(void) {
    return "SmartACE::exampleEvent2Parameter";
  };

  void set(int,int);          // set lower/upper bound of interval
  void get(int&,int&) const;  // get lower/upper bound of interval
  void set(int);              // set state
  void get(int&) const;       // get state
  void print(std::ostream &os = std::cout) const ;
};

inline std::ostream &operator<< (std::ostream &os, const CommExampleEvent2Parameter &c) {
  c.print(os);
  return os;
};

// --------------------------------------------------------------------------
//
// event result part
//
// --------------------------------------------------------------------------
class CommExampleEvent2Result
{
public:
  ExampleEvent2Result result;

  CommExampleEvent2Result();
  virtual ~CommExampleEvent2Result();

  static inline std::string identifier(void) {
    return "SmartACE::exampleEvent2Result";
  };

  void set(std::string);
  void get(std::string&) const;
  void print(std::ostream &os = std::cout) const;
};

inline std::ostream &operator<< (std::ostream &os, const CommExampleEvent2Result &c) {
  c.print(os);
  return os;
};

}

#endif

