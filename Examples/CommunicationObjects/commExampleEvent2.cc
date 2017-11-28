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

#include "ace/CDR_Stream.h"
#include "ace/SString.h"

#include "commExampleEvent2.hh"

using namespace SmartACE;

CommExampleEvent2Parameter::CommExampleEvent2Parameter() : state(0)
{
}

CommExampleEvent2Parameter::~CommExampleEvent2Parameter()
{
}

void CommExampleEvent2Parameter::get(ACE_Message_Block *&msg) const
{
   ACE_OutputCDR cdr(ACE_DEFAULT_CDR_BUFSIZE);

   cdr << parameter.upper;
   cdr << parameter.lower;

   msg = cdr.begin()->clone();
}

void CommExampleEvent2Parameter::set(const ACE_Message_Block *msg)
{
   ACE_InputCDR cdr(msg);

   cdr >> parameter.upper;
   cdr >> parameter.lower;
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

void CommExampleEvent2Result::get(ACE_Message_Block *&msg) const
{
   ACE_OutputCDR cdr(ACE_DEFAULT_CDR_BUFSIZE);

   ACE_CString str_temp(result.state.c_str());
   cdr << str_temp;

   msg = cdr.begin()->clone();
}

void CommExampleEvent2Result::set(const ACE_Message_Block *msg)
{
   ACE_InputCDR cdr(msg);

   ACE_CString str_temp;
   cdr >> str_temp;
   result.state = str_temp.c_str();
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

