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

#include "smartOSMapping.hh"

namespace SmartACE {

  std::ostream & operator<<(std::ostream & o, const ACE_Time_Value & ts)
  {
    time_t sec = ts.sec();
    struct tm * t = ACE_OS::localtime( &sec );
    char s[40];
    ACE_OS::strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", t);
    return o << s << "." << std::setfill('0') << std::setw(6) << ts.usec();
  }

  std::chrono::steady_clock::duration convertToStdDurFrom(const ACE_Time_Value &time)
  {
	  return std::chrono::seconds(time.sec()) + std::chrono::microseconds(time.usec());
  }
  ACE_Time_Value convertToAceTimeFrom(const std::chrono::steady_clock::duration &dur)
  {
    if(dur == std::chrono::steady_clock::duration::zero()) {
      return ACE_Time_Value::zero;
    }
    std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(dur);
    std::chrono::microseconds us = std::chrono::duration_cast<std::chrono::microseconds>(dur-sec);
    return ACE_Time_Value(sec.count(), us.count());
  }
}

