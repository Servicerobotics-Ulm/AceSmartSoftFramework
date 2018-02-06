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
#include <iostream>

namespace SmartACE
{

CommExampleTypes::CommExampleTypes(void)
{

}

CommExampleTypes::~CommExampleTypes(void)
{

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
