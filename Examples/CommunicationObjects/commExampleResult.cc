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

#include "commExampleResult.hh"

#include "ace/CDR_Stream.h"

using namespace SmartACE;

CommExampleResult::CommExampleResult()
{
}

CommExampleResult::~CommExampleResult()
{
}

void CommExampleResult::get(ACE_Message_Block *&msg) const
{
	ACE_OutputCDR cdr(ACE_DEFAULT_CDR_BUFSIZE);

	cdr << result.value;

	msg = cdr.begin()->clone();
}

void CommExampleResult::set(const ACE_Message_Block *msg)
{
	ACE_InputCDR cdr(msg);

	cdr >> result.value;
}

void CommExampleResult::set(int r)
{
  result.value = r;
}

void CommExampleResult::print(std::ostream &os) const
{
  os << "Result : " << result.value << std::endl;
}

