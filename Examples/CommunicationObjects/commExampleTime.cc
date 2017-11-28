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

#include "commExampleTime.hh"

#include "ace/CDR_Stream.h"

using namespace SmartACE;

CommExampleTime::CommExampleTime()
{
}

CommExampleTime::~CommExampleTime()
{
}

void CommExampleTime::get(ACE_Message_Block *&msg) const
{
	ACE_OutputCDR cdr(ACE_DEFAULT_CDR_BUFSIZE);

	cdr << time.hour;
	cdr << time.minute;
	cdr << time.second;

	msg = cdr.begin()->clone();
}

void CommExampleTime::set(const ACE_Message_Block *msg)
{
	ACE_InputCDR cdr(msg);

	cdr >> time.hour;
	cdr >> time.minute;
	cdr >> time.second;
}

void CommExampleTime::get(int &h,int &m,int &s)
{
  h = time.hour;
  m = time.minute;
  s = time.second;
}

void CommExampleTime::set(int h,int m,int s)
{
  time.hour   = h;
  time.minute = m;
  time.second = s;
}

void CommExampleTime::print(std::ostream &os) const
{
  os << "Time in Greenwich is "
     << std::setw (2) << std::setfill ('0') << time.hour << ":"
     << std::setw (2) << std::setfill ('0') << time.minute << ":"
     << std::setw (2) << std::setfill ('0') << time.second << std::endl;
}

