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

#include "commExamplePrint.hh"

using namespace SmartACE;

CommExamplePrint::CommExamplePrint()
{
}

CommExamplePrint::~CommExamplePrint()
{
}


void CommExamplePrint::get(int &h,int &m,int &s,std::string& t) const
{
  h = message.hour;
  m = message.minute;
  s = message.second;
  t = message.text;
}

void CommExamplePrint::set(int h,int m,int s,std::string t)
{
  message.hour   = h;
  message.minute = m;
  message.second = s;
  message.text   = t.c_str();
}

void CommExamplePrint::print(std::ostream &os) const
{
  os << "Time in Greenwich is "
     << std::setw (2) << std::setfill ('0') << message.hour << ":"
     << std::setw (2) << std::setfill ('0') << message.minute << ":"
     << std::setw (2) << std::setfill ('0') << message.second << " text:" << message.text << std::endl;
}

