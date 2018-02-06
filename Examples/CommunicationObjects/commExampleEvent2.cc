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

#include "commExampleEvent2.hh"

using namespace SmartACE;

CommExampleEvent2Parameter::CommExampleEvent2Parameter() : state(0)
{
}

CommExampleEvent2Parameter::~CommExampleEvent2Parameter()
{
}

void CommExampleEvent2Parameter::get(int& s) const
{
  s = state;
}

void CommExampleEvent2Parameter::set(int s)
{
  state = s;
}

void CommExampleEvent2Parameter::set(int l,int u)
{
  parameter.lower = l;
  parameter.upper = u;
}

void CommExampleEvent2Parameter::get(int& l,int& u) const
{
  l = parameter.lower;
  u = parameter.upper;
}

void CommExampleEvent2Parameter::print(std::ostream &os) const
{
  os << "Parameter : (lower, upper) " << parameter.lower << " " << parameter.upper << " state " << state << std::endl;
}




CommExampleEvent2Result::CommExampleEvent2Result()
{
}

CommExampleEvent2Result::~CommExampleEvent2Result()
{
}

void CommExampleEvent2Result::set(std::string s)
{
  result.state = s.c_str();
}

void CommExampleEvent2Result::get(std::string& s) const
{
  s = result.state;
}

void CommExampleEvent2Result::print(std::ostream &os) const
{
  os << "Result : " << result.state << std::endl;
}

