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

#include "commExampleValues.hh"

#include "ace/CDR_Stream.h"

using namespace SmartACE;

CommExampleValues::CommExampleValues()
{
}

CommExampleValues::~CommExampleValues()
{
}

void CommExampleValues::get(ACE_Message_Block *&msg) const
{
	ACE_OutputCDR cdr(ACE_DEFAULT_CDR_BUFSIZE);

	// Set number of values in list.
	// this helps by reading (see set function)
	cdr << static_cast<ACE_UINT32>(values.size());

	std::list<int>::const_iterator it;
	for(it=values.begin(); it != values.end(); it++) {
		cdr << *it;
	}

	msg = cdr.begin()->clone();
}

void CommExampleValues::set(const ACE_Message_Block *msg)
{
	ACE_InputCDR cdr(msg);
	int temp = 0;
	int nmbrOfValues = 0;

	cdr >> nmbrOfValues;

	values.clear();

	for(int i=0; i<nmbrOfValues; ++i) {
		cdr >> temp;
		values.push_back(temp);
	}

}

void CommExampleValues::set(std::list<int> a)
{
  values = a;
}

void CommExampleValues::get(std::list<int> &l) const
{
  l = values;
}

