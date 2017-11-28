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

#include <string>
#include <list>

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
  protected:
     // internal data structure
     StateCommand command;
     std::string state_name;
   
  public:
    //
    // constructors, destructors, copy constructors etc. ...
    //
     SmartCommStateRequest()
        :   command(STATE_CMD_ERROR)
     {  }

     virtual ~SmartCommStateRequest() {  };

    //
    // The following methods MUST be available in a communication object.
    // This however is not too bad for implementers of a communication
    // object since you can get cookbook like instructions on how
    // to implement these. They are always the same since they have to
    // set / get the above IDL structure. They are used by the communication
    // patterns and should not be used by users.
    //
    //<alexej date="26.11.2008">
    void get(SmartMessageBlock *&msg) const;
    void set(const SmartMessageBlock *msg);
    //</alexej>
    static inline std::string identifier(void) {
      return "SmartACE::smartStateRequest";
    };


    /// Set/Get the command-enum defined above
    inline void setCommand(const StateCommand &command)
    {
       this->command = command;
    }
    inline StateCommand  getCommand(void) const
    {
      return command;
    }

    /// Set/Get state-name
    inline void setStateName(const std::string &name)
    {
      state_name = name;
    }
    inline std::string getStateName(void) const
    {
      return state_name;
    }
  };


/////////////////////////////////////////////////////////////////////////
//
// Response (Answer)
//
/////////////////////////////////////////////////////////////////////////

  class SmartCommStateResponse
  {
  protected:
     // internal data structure
     std::list<std::string> state_list;
     int status;
   
  public:
    //
    // constructors, destructors, copy constructors etc. ...
    //
    SmartCommStateResponse()
       : status(0)
    {
       state_list.clear();
    }
    virtual ~SmartCommStateResponse() {  }

    //
    // The following methods MUST be available in a communication object.
    // This however is not too bad for implementers of a communication
    // object since you can get cookbook like instructions on how
    // to implement these. They are always the same since they have to
    // set / get the above IDL structure. They are used by the communication
    // patterns and should not be used by users.
    //
    //<alexej date="26.11.2008">
    void get(SmartMessageBlock *&msg) const;
    void set(const SmartMessageBlock *msg);
    //</alexej>
    static inline std::string identifier(void) {
      return "SmartACE::smartStateResponse";
    };


    /// Get/Set return status
    inline void setStatus(const int &status)
    {
       this->status = status;
    }
    inline int getStatus(void) const
    {
       return status;
    }

    ///
    inline void setStateList(const std::list<std::string> &states)
    {
      this->state_list = states;
    }
    inline std::list<std::string> getStateList(void) const
    {
      return state_list;
    }
  };


} // end namespace SMART_COMM_STATE_HH
#endif

