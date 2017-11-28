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

/////////////////////////////////////////////////////////////////////////
//
// Response (Answer)
//
/////////////////////////////////////////////////////////////////////////

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
