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

#include "smartParameter.hh"

/////////////////////////////////////////////////////////////////////////
//
// master part of Parameter pattern
//
/////////////////////////////////////////////////////////////////////////

//
// standard constructor
//
SmartACE::ParameterMaster::ParameterMaster(SmartComponent* m) throw(SmartACE::SmartError)
: mutex()
, component(m)
, query_client(component)
{
	if(component->getName().compare(SMART_NONS) == 0) {
		_use_ns = false;
	}else{
		_use_ns = true;
	}
}

//
// destructor
//
SmartACE::ParameterMaster::~ParameterMaster(void) throw()
{
  // client does not bind object at naming service,
  // therefore no unbind of objects needed here
}

//
//
//
Smart::StatusCode SmartACE::ParameterMaster::sendParameterWait(
		const SmartACE::CommParameterRequest &request,
		SmartACE::CommParameterResponse &response,
		const std::string &comp_name, const std::string &service_name)
{
	SmartACE::SmartGuard g(mutex);

	Smart::StatusCode status;

	// connect manually since connect with constructor is retrying
	//<alexej date="2009-06-17">
	if(_use_ns) {
	   status = query_client.connect(comp_name,service_name);
	}else{
	   status = query_client.connect(SMART_NONS, service_name);
	}
	//</alexej>
	switch (status) {
	case Smart::SMART_OK:
	  // everything ok
	  break;
	case Smart::SMART_SERVICEUNAVAILABLE:
	  // the comp_name is unknown or does not provide a Parameter slave
	  return Smart::SMART_UNKNOWNCOMPONENT;
	  break;
	case Smart::SMART_INCOMPATIBLESERVICE:
	  // the "param" service of the comp_name is not compatible to the master
	  return Smart::SMART_UNKNOWNCOMPONENT;
	  break;
	case Smart::SMART_ERROR_COMMUNICATION:
	  // problems while connecting to the slave
	  return Smart::SMART_ERROR_COMMUNICATION;
	  break;
	default:
	  // problems while connecting to the slave
	  return Smart::SMART_ERROR;
	  break;
	}

	status = query_client.query(request, response);

	query_client.disconnect();

	return status;
}


/////////////////////////////////////////////////////////////////////////
//
// handler of slave part of Parameter pattern
//
/////////////////////////////////////////////////////////////////////////

//
// standard constructor
//
SmartACE::ParameterSlave::ParameterQueryHandler::ParameterQueryHandler(
		QueryServer<SmartACE::CommParameterRequest,SmartACE::CommParameterResponse>* server,
		ParameterUpdateHandler *param_handler) throw()
  : param_handler(param_handler)
  , QueryServerHandler<SmartACE::CommParameterRequest,SmartACE::CommParameterResponse>(server)
{
  //
}

//
// destructor
//
SmartACE::ParameterSlave::ParameterQueryHandler::~ParameterQueryHandler() throw()
{
  //
}

//
//
//
void SmartACE::ParameterSlave::ParameterQueryHandler::handleQuery(const QueryId &id, const SmartACE::CommParameterRequest & request) throw()
{
  SmartACE::CommParameterResponse answer;

  answer = param_handler->handleParameter(request);

  this->server->answer(id,answer);
}




/////////////////////////////////////////////////////////////////////////
//
// slave part of Parameter pattern
//
/////////////////////////////////////////////////////////////////////////

//
//
//
SmartACE::ParameterSlave::ParameterSlave(SmartComponent* comp, ParameterUpdateHandler *hnd, std::string slave_address) throw(SmartACE::SmartError)
: component(comp)
, query_server(comp, slave_address)
, query_handler(&query_server,hnd)
, thread_handler(comp,&query_handler)
{  }

//
//
//
SmartACE::ParameterSlave::~ParameterSlave(void) throw()
{  }


