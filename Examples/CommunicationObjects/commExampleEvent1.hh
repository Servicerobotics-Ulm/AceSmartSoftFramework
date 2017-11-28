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

#ifndef _COMM__EXAMPLE_EVENT1_HH
#define _COMM__EXAMPLE_EVENT1_HH

#include <iostream>
#include <string>

#include "exampleEvent1.hh"

#include "ace/Message_Block.h"

namespace SmartACE {

// --------------------------------------------------------------------------
//
// event parameter part
//
// --------------------------------------------------------------------------
class CommExampleEvent1Parameter
{
protected:
  ExampleEvent1Parameter parameter;

public:
  CommExampleEvent1Parameter();
  virtual ~CommExampleEvent1Parameter();

  //<alexej date="2009-10-26">
      // Helper functions to pack/unpack comm-obj data
      void get(ACE_Message_Block *&msg) const;
      void set(const ACE_Message_Block *msg);
  //</alexej>
  static inline std::string identifier(void) {
    return "SmartACE::exampleEvent1Parameter";
  };

  void set(int);
  void get(int&) const;
  void print(std::ostream &os = std::cout) const;
};

inline std::ostream &operator<< (std::ostream &os, const CommExampleEvent1Parameter &c) {
  c.print(os);
  return os;
};

// --------------------------------------------------------------------------
//
// event result part
//
// --------------------------------------------------------------------------
class CommExampleEvent1Result
{
protected:
  ExampleEvent1Result result;

public:
  CommExampleEvent1Result();
  virtual ~CommExampleEvent1Result();

  //<alexej date="2009-10-26">
      // Helper functions to pack/unpack comm-obj data
      void get(ACE_Message_Block *&msg) const;
      void set(const ACE_Message_Block *msg);
  //</alexej>
  static inline std::string identifier(void) {
    return "SmartACE::exampleEvent1Result";
  };

  void set(int);
  void get(int&) const;
  void print(std::ostream &os = std::cout) const;
};

inline std::ostream &operator<< (std::ostream &os, const CommExampleEvent1Result &c) {
  c.print(os);
  return os;
};

// --------------------------------------------------------------------------
//
// event state part
//
// --------------------------------------------------------------------------
class ExampleEventState
{
protected:
  int state;

public:
  ExampleEventState();
  virtual ~ExampleEventState();

  void set(int);
  void get(int&) const;
};

}

#endif

