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

#include "smartCommState.hh"

#include <ace/CDR_Stream.h>
#include <ace/SString.h>

/////////////////////////////////////////////////////////////////////////
//
// Request
//
/////////////////////////////////////////////////////////////////////////

/*
void SmartACE::SmartCommStateRequest::get(SmartMessageBlock *&msg) const
{
  ACE_OutputCDR cdr(ACE_DEFAULT_CDR_BUFSIZE);
  
  // write command id into stream
  cdr << static_cast<ACE_CDR::Long>(command);

  // write state name into cdr stream
  ACE_CString cstate_name(state_name.c_str());
  cdr << cstate_name;


  msg = cdr.begin()->clone();
}

void SmartACE::SmartCommStateRequest::set(const SmartMessageBlock *msg)
{
   ACE_InputCDR cdr(msg);

   // temporary variables
   ACE_CDR::Long cmd;
   ACE_CString cstate_name;

   // read command and cast it to command-enum
   cdr >> cmd;
   command = static_cast<StateCommand>(cmd);

   cdr >> cstate_name;
   state_name = cstate_name.c_str();
}
*/

ACE_CDR::Boolean operator<<(ACE_OutputCDR &cdr, const SmartACE::SmartCommStateRequest &obj)
{
	ACE_CDR::Boolean good_bit = true;

	// write command id into stream
	good_bit = good_bit && cdr << static_cast<ACE_CDR::Long>(obj.data.command);

	// write state name into cdr stream
	ACE_CString cstate_name(obj.data.state_name.c_str());
	good_bit = good_bit && cdr << cstate_name;

	return good_bit;
}

ACE_CDR::Boolean operator>>(ACE_InputCDR &cdr, SmartACE::SmartCommStateRequest &obj)
{
	ACE_CDR::Boolean good_bit = true;

	// temporary variables
	ACE_CString cstate_name;

	// read command and cast it to command-enum
	good_bit = good_bit && cdr.read_long(obj.data.command);

	good_bit = good_bit && cdr >> cstate_name;
	obj.data.state_name = cstate_name.c_str();

	return good_bit;
}

/////////////////////////////////////////////////////////////////////////
//
// Response (Answer)
//
/////////////////////////////////////////////////////////////////////////

/*
void SmartACE::SmartCommStateResponse::get(SmartMessageBlock *&msg) const
{
   ACE_OutputCDR cdr(ACE_DEFAULT_CDR_BUFSIZE);
   ACE_CDR::ULong size = 0;
  
   // write status into stream
   cdr << status;

   // write number of strings in state_list into stream
   size = static_cast<ACE_CDR::ULong>(state_list.size());
   cdr << size;

   // write all strings into stream
   std::list<std::string>::const_iterator it;
   for(it=state_list.begin(); it != state_list.end(); ++it)
   {
      ACE_CString cstate_name(it->c_str());
      cdr << cstate_name;
   }

   msg = cdr.begin()->duplicate();
}

void SmartACE::SmartCommStateResponse::set(const SmartMessageBlock *msg)
{
   ACE_InputCDR cdr(msg);

   // temporary variables
   ACE_CDR::ULong size = 0;

   // read status
   cdr >> status;

   // read number of strings to store in state_list
   cdr >> size;

   state_list.clear();

   for(unsigned int i=0; i<size; ++i)
   {
      ACE_CString cstate_name;
      cdr >> cstate_name;
      state_list.push_back(cstate_name.c_str());
   }

}
*/

ACE_CDR::Boolean operator<<(ACE_OutputCDR &cdr, const SmartACE::SmartCommStateResponse &obj)
{
	ACE_CDR::Boolean good_bit = true;

	// write status into stream
	good_bit = good_bit && cdr << static_cast<ACE_CDR::ULong>(obj.data.status);

	// write number of strings in state_list into stream
	good_bit = good_bit && cdr << static_cast<ACE_CDR::ULong>(obj.data.state_list.size());

	// write all strings into stream
	std::vector<std::string>::const_iterator it;
	for(it=obj.data.state_list.begin(); it != obj.data.state_list.end(); ++it)
	{
	  ACE_CString cstate_name(it->c_str());
	  good_bit = good_bit && cdr << cstate_name;
	}

	return good_bit;
}
ACE_CDR::Boolean operator>>(ACE_InputCDR &cdr, SmartACE::SmartCommStateResponse &obj)
{
	ACE_CDR::Boolean good_bit = true;

	// temporary variables
	ACE_CDR::ULong size = 0;

	// read status
	good_bit = good_bit && cdr.read_long(obj.data.status);

	// read number of strings to store in state_list
	good_bit = good_bit && cdr >> size;

	obj.data.state_list.clear();

	for(unsigned int i=0; i<size; ++i)
	{
	  ACE_CString cstate_name;
	  good_bit = good_bit && cdr >> cstate_name;
	  obj.data.state_list.push_back(cstate_name.c_str());
	}

	return good_bit;
}
