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

#include "smartCommWiring.hh"

#include <ace/CDR_Stream.h>

SmartACE::SmartCommWiring::SmartCommWiring()
{
}

SmartACE::SmartCommWiring::~SmartCommWiring()
{
}

void SmartACE::SmartCommWiring::get(SmartACE::SmartMessageBlock *&msg) const
{
  ACE_OutputCDR cdr(ACE_DEFAULT_CDR_BUFSIZE);
  
  // wiring command
  cdr << wiring.command;

  // wiring slaveport
  cdr << wiring.slaveport;

  // wiring servercomponent
  cdr << wiring.servercomponent;

  // wiring serverservice
  cdr << wiring.serverservice;

  // wiring status
  cdr << wiring.status;

  msg = cdr.begin()->duplicate();
}

void SmartACE::SmartCommWiring::set(const SmartACE::SmartMessageBlock *msg)
{
  ACE_InputCDR cdr(msg);

  // wiring command
  cdr >> wiring.command;

  // wiring slaveport
  cdr >> wiring.slaveport;

  // wiring servercomponent
  cdr >> wiring.servercomponent;

  // wiring serverservice
  cdr >> wiring.serverservice;

  // wiring status
  cdr >> wiring.status;
}
//</alexej>

void SmartACE::SmartCommWiring::setCommand(const std::string cmd,const std::string slaveprt,const std::string servercmpt,const std::string serversvc)
{
  wiring.command         = cmd.c_str();
  wiring.slaveport       = slaveprt.c_str();
  wiring.servercomponent = servercmpt.c_str();
  wiring.serverservice   = serversvc.c_str();
}

void SmartACE::SmartCommWiring::getCommand(std::string& cmd,std::string& slaveprt,std::string& servercmpt,std::string& serversvc) const
{
  cmd        = wiring.command.c_str();
  slaveprt   = wiring.slaveport.c_str();
  servercmpt = wiring.servercomponent.c_str();
  serversvc  = wiring.serverservice.c_str();
}

void SmartACE::SmartCommWiring::setStatus(const Smart::StatusCode status)
{
  wiring.status = status;
}

void SmartACE::SmartCommWiring::getStatus(Smart::StatusCode& status) const
{
  status = (Smart::StatusCode)wiring.status;
}

