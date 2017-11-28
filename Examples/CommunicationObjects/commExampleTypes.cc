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

#include "commExampleTypes.hh"
#include "ace/SString.h"

#include <iostream>

namespace SmartACE
{

CommExampleTypes::CommExampleTypes(void)
{

}

CommExampleTypes::~CommExampleTypes(void)
{

}

void CommExampleTypes::get(ACE_Message_Block *&msg) const
{
   ACE_OutputCDR cdr(ACE_DEFAULT_CDR_BUFSIZE);

   // 1) write std::string
   ACE_CString str_temp(text.c_str());
   cdr << str_temp;



   // 2) write primitive data types
   cdr << ACE_OutputCDR::from_boolean( data.cdr_bool );
   cdr << data.cdr_char;
   cdr << data.cdr_long;
   cdr << data.cdr_dbl;
   cdr << data.cdr_float;

   cdr << data.cdr_longlong;
   cdr << data.cdr_ulong;
   cdr << data.cdr_ulonglong;
   cdr << data.cdr_ushort;

/*
   // TODO: figure out how to send unicode strings...
   // 3) write ACE_CDR::WChar
   size = wstring.length();

	cdr << static_cast<ACE_UINT32>(size);
   cdr.write_wchar_array(wstring.c_str(), static_cast<ACE_UINT32>(size) );
*/


    msg = cdr.begin()->clone();
}


void CommExampleTypes::set(const ACE_Message_Block *msg)
{
   ACE_InputCDR cdr(msg);

   // 1) read std::string
   ACE_CString str_temp;
   cdr >> str_temp;
   text = str_temp.c_str();

   // 2) read primitive data types
   cdr >> ACE_InputCDR::to_boolean( data.cdr_bool );
   cdr >> data.cdr_char;
   cdr >> data.cdr_long;
   cdr >> data.cdr_dbl;
   cdr >> data.cdr_float;

   cdr >> data.cdr_longlong;
   cdr >> data.cdr_ulong;
   cdr >> data.cdr_ulonglong;
   cdr >> data.cdr_ushort;


   // 3) read ACE_CDR::WChar
   // TODO: figure out how to send unicode strings...
/*
	cdr >> size;

   ACE_CDR::WChar *wtemp = new ACE_CDR::WChar[size+1];
   wstring.clear();

   cdr.read_wchar_array( wtemp , static_cast<ACE_UINT32>(size) );

   wtemp[size] = ACE_TEXT_WIDE('\0');

   wstring.set( wtemp, size );

	delete[] wtemp;
*/
}

void CommExampleTypes::print_data()
{
   std::cout << "Boolean (cdr): " << data.cdr_bool << std::endl;
   std::cout << "Char (cdr): " << data.cdr_char << std::endl;
   std::cout << "Integer (cdr): " << data.cdr_long << std::endl;
   std::cout << "Double (cdr): " << data.cdr_dbl << std::endl;
   std::cout << "Float (cdr): " << data.cdr_float << std::endl;
   std::cout << "Long Integer (cdr): " << data.cdr_longlong << std::endl;
   std::cout << "Unsigned Integer (cdr): " << data.cdr_ulong << std::endl;
   std::cout << "Unsigned Long Integer (cdr): " << data.cdr_ulonglong << std::endl;
   std::cout << "Unsigned Short (cdr): " << data.cdr_ushort << std::endl;
   std::cout << std::endl;
   std::cout << "std::string: " << text << std::endl;
   std::cout << "#######################################" << std::endl;


   //ACE_DEBUG((LM_INFO, ACE_TEXT("CDR_WChar: %W\n"), wstring));

   std::cout << std::endl;
}

} // end namespace
