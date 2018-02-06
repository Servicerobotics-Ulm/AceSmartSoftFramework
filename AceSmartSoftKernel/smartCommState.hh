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

#ifndef SMART_COMM_STATE_HH
#define SMART_COMM_STATE_HH

#include "smartOSMapping.hh"

#include "smartCommStateIdl.hh"

#include <ace/CDR_Stream.h>

namespace SmartACE {

/////////////////////////////////////////////////////////////////////////
//
// Request
//
/////////////////////////////////////////////////////////////////////////

   enum StateCommand
   {
      STATE_CMD_ERROR,
      STATE_CMD_SET_STATE,
      STATE_CMD_GET_CURRENT_STATE,
      STATE_CMD_GET_MAIN_STATES,
      STATE_CMD_GET_SUB_STATES
   };

  class SmartCommStateRequest
  {
  public:
     // internal data structure
	 CommStateIDL::SmartCommStateRequest data;
   
    //
    // constructors, destructors, copy constructors etc. ...
    //
     SmartCommStateRequest()
     {
    	 data.command = static_cast<int>(STATE_CMD_ERROR);
     }

     virtual ~SmartCommStateRequest() {  };

    static inline std::string identifier(void) {
      return "SmartACE::smartStateRequest";
    };


    /// Set/Get the command-enum defined above
    inline void setCommand(const StateCommand &command)
    {
    	data.command = static_cast<int>(command);
    }
    inline StateCommand  getCommand(void) const
    {
      return static_cast<StateCommand>(data.command);
    }

    /// Set/Get state-name
    inline void setStateName(const std::string &name)
    {
    	data.state_name = name;
    }
    inline std::string getStateName(void) const
    {
      return data.state_name;
    }
  };

/////////////////////////////////////////////////////////////////////////
//
// Response (Answer)
//
/////////////////////////////////////////////////////////////////////////

  class SmartCommStateResponse
  {
  public:
     // internal data structure
	 CommStateIDL::SmartCommStateResponse data;

    //
    // constructors, destructors, copy constructors etc. ...
    //
    SmartCommStateResponse()
    {
    	data.status = 0;
    	data.state_list.clear();
    }
    virtual ~SmartCommStateResponse() {  }

    static inline std::string identifier(void) {
      return "SmartACE::smartStateResponse";
    };


    /// Get/Set return status
    inline void setStatus(const int &status)
    {
    	data.status = status;
    }
    inline int getStatus(void) const
    {
       return data.status;
    }

    ///
    inline void setStateList(const std::vector<std::string> &states)
    {
    	data.state_list = states;
    }
    inline std::vector<std::string> getStateList(void) const
    {
      return data.state_list;
    }
  };
} // end namespace SMART_COMM_STATE_HH


////////////////////////////////////////////////////////////////////////
//
// serialization operators
//
////////////////////////////////////////////////////////////////////////
ACE_CDR::Boolean operator<<(ACE_OutputCDR &cdr, const SmartACE::SmartCommStateRequest &obj);
ACE_CDR::Boolean operator>>(ACE_InputCDR &cdr, SmartACE::SmartCommStateRequest &obj);

ACE_CDR::Boolean operator<<(ACE_OutputCDR &cdr, const SmartACE::SmartCommStateResponse &obj);
ACE_CDR::Boolean operator>>(ACE_InputCDR &cdr, SmartACE::SmartCommStateResponse &obj);
#endif

