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

#ifndef _SMARTERRNO_HH
#define _SMARTERRNO_HH

#include <string>
#include <stdexcept>
#include <iostream>

#include <smartStatusCode.h>


/////////////////////////////////////////////////////////////////////////
//
// some standard error codes
//
/////////////////////////////////////////////////////////////////////////

namespace SmartACE {

  //<alexej date="2009-06-16">
  // constant to disable naming service
  const std::string SMART_NONS = "NoNamingService";

  //
  // exceptions
  //

  /** allows printing exceptions based on std::exception */
  std::ostream & operator<<(std::ostream & o, const std::exception & e);


  /** Exception class to describe errors in constructors of communication patterns.
   */
  class SmartError : public std::invalid_argument{
    public:
      Smart::StatusCode status;

      SmartError(const Smart::StatusCode s,const std::string & reason) : std::invalid_argument(reason),status(s) {};
  };

} // end namespace SmartACE

#endif    // ifndef _SMARTERRNO_HH

