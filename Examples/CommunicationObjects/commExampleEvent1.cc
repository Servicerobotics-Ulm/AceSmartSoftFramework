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

#include "commExampleEvent1.hh"

using namespace SmartACE;

CommExampleEvent1Parameter::CommExampleEvent1Parameter()
{
}

CommExampleEvent1Parameter::~CommExampleEvent1Parameter()
{
}

void CommExampleEvent1Parameter::set(int r)
{
  parameter.value = r;
}

void CommExampleEvent1Parameter::get(int& r) const
{
  r = parameter.value;
}

void CommExampleEvent1Parameter::print(std::ostream &os) const
{
  os << "Parameter : " << parameter.value << std::endl;
}




CommExampleEvent1Result::CommExampleEvent1Result()
{
}

CommExampleEvent1Result::~CommExampleEvent1Result()
{
}

void CommExampleEvent1Result::set(int r)
{
  result.value = r;
}

void CommExampleEvent1Result::get(int& r) const
{
  r = result.value;
}

void CommExampleEvent1Result::print(std::ostream &os) const
{
  os << "Result : " << result.value << std::endl;
}




ExampleEventState::ExampleEventState()
:	state(0)
{
}

ExampleEventState::~ExampleEventState()
{
}

void ExampleEventState::set(int r)
{
  state = r;
}

void ExampleEventState::get(int& r) const
{
  r = state;
}

