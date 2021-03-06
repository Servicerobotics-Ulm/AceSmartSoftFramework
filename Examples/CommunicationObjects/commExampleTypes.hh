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

//
//
//
// Example of a communication object whose data structure "Print" is
// described using IDL.
//
//
//

#ifndef _COMM_EXAMPLE_TYPES_HH
#define _COMM_EXAMPLE_TYPES_HH

#include <string>

#include "exampleTypes.hh"

namespace SmartACE
{
   class CommExampleTypes
   {
   public:
      CommExampleTypes(void);
      virtual ~CommExampleTypes(void);

      // internal DATA storage of commobject
      ExampleTypes data;
      std::string text;
      //ACE_WString wstring;

      // Comm-Obj identifier
      static inline std::string identifier(void) {
        return "SmartACE::exampleTypes";
      };

      // print data to std-out
      void print_data();
   };

}

#endif
