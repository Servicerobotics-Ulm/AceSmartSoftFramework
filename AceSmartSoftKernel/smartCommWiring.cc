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

ACE_CDR::Boolean operator<<(ACE_OutputCDR &cdr, const SmartACE::SmartCommWiring &obj)
{
	ACE_CDR::Boolean good_bit = true;

	// wiring command
	good_bit = good_bit && cdr << ACE_CString(obj.wiring.command.c_str());

	// wiring slaveport
	good_bit = good_bit && cdr << ACE_CString(obj.wiring.slaveport.c_str());

	// wiring servercomponent
	good_bit = good_bit && cdr << ACE_CString(obj.wiring.servercomponent.c_str());

	// wiring serverservice
	good_bit = good_bit && cdr << ACE_CString(obj.wiring.serverservice.c_str());

	// wiring status
	good_bit = good_bit && cdr << static_cast<ACE_CDR::Long>(obj.wiring.status);

	return good_bit;
}

ACE_CDR::Boolean operator>>(ACE_InputCDR &cdr, SmartACE::SmartCommWiring &obj)
{
	ACE_CDR::Boolean good_bit = true;


	// wiring command
	ACE_CString command;
	good_bit = good_bit && cdr >> command;
	obj.wiring.command = command.c_str();

	// wiring slaveport
	ACE_CString slaveport;
	good_bit = good_bit && cdr >> slaveport;
	obj.wiring.slaveport = slaveport.c_str();

	// wiring servercomponent
	ACE_CString servercomponent;
	good_bit = good_bit && cdr >> servercomponent;
	obj.wiring.servercomponent = servercomponent.c_str();

	// wiring serverservice
	ACE_CString serverservice;
	good_bit = good_bit && cdr >> serverservice;
	obj.wiring.serverservice = serverservice.c_str();

	// wiring status
	good_bit = good_bit && cdr.read_long(obj.wiring.status);

	return good_bit;
}
//</alexej>

void SmartACE::SmartCommWiring::setCommand(const std::string cmd,const std::string slaveprt,const std::string servercmpt,const std::string serversvc)
{
  wiring.command         = cmd;
  wiring.slaveport       = slaveprt;
  wiring.servercomponent = servercmpt;
  wiring.serverservice   = serversvc;
}

void SmartACE::SmartCommWiring::getCommand(std::string& cmd,std::string& slaveprt,std::string& servercmpt,std::string& serversvc) const
{
  cmd        = wiring.command;
  slaveprt   = wiring.slaveport;
  servercmpt = wiring.servercomponent;
  serversvc  = wiring.serverservice;
}

void SmartACE::SmartCommWiring::setStatus(const Smart::StatusCode status)
{
  wiring.status = status;
}

void SmartACE::SmartCommWiring::getStatus(Smart::StatusCode& status) const
{
  status = (Smart::StatusCode)wiring.status;
}

