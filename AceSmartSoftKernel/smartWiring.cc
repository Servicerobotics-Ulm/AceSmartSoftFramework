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

#include "smartWiring.hh"

#include "smartQuery.hh"

/////////////////////////////////////////////////////////////////////////
//
// master part of wiring pattern
//
/////////////////////////////////////////////////////////////////////////

//
// default constructor
//
SmartACE::WiringMaster::WiringMaster(void) 
{
  std::cerr << "CommPattern (WiringMaster): ERROR: Entered default constructor WiringMaster" << std::endl;
  component=0;
  _use_ns=true;
  throw(SmartACE::SmartError(Smart::SMART_ERROR,"CommPattern (WiringMaster): ERROR: Entered default constructor WiringMaster"));
}

//
// standard constructor
//
SmartACE::WiringMaster::WiringMaster(SmartComponent* m)  : component(m)
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
SmartACE::WiringMaster::~WiringMaster(void) 
{
  // client does not bind object at naming service,
  // therefore no unbind of objects needed here
}

//
//
//
Smart::StatusCode SmartACE::WiringMaster::connect(const std::string& slavecmpt,const std::string& slaveprt, const std::string& servercmpt,const std::string& serversvc) 
{
  SmartACE::QueryClient<SmartCommWiring,SmartCommWiring> client(component);
  SmartCommWiring request,answer;
  Smart::StatusCode status;

  // connect manually since connect with constructor is retrying
  //<alexej date="2009-06-17">
  if(_use_ns) {
	status = client.connect(slavecmpt,"wiring");
  }else{
	status = client.connect(SMART_NONS, slavecmpt);
  }
  //</alexej>
  switch (status) {
    case Smart::SMART_OK:
      // everything ok
      break;
    case Smart::SMART_SERVICEUNAVAILABLE:
      // the servercmpt is unknown or does not provide a wiring slave
      return Smart::SMART_UNKNOWNCOMPONENT;
      break;
    case Smart::SMART_INCOMPATIBLESERVICE:
      // the "wiring" service of the servercmpt is not compatible to the master
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

  request.setCommand("connect",slaveprt,servercmpt,serversvc);

  status = client.query(request,answer);
  if (status != Smart::SMART_OK) return status;

  answer.getStatus(status);

  return status;
}
  
//
//
//
 Smart::StatusCode SmartACE::WiringMaster::disconnect(const std::string& slavecmpt,const std::string& slaveprt) 
{
  SmartACE::QueryClient<SmartCommWiring,SmartCommWiring> client(component);
  SmartCommWiring request,answer;
   Smart::StatusCode status;

  // connect manually since connect with constructor is retrying
  //<alexej date="2009-06-17">
  if(_use_ns) {
	status = client.connect(slavecmpt,"wiring");
  }else{
	status = client.connect(SMART_NONS, slavecmpt);
  }
  //</alexej>
  switch (status) {
    case Smart::SMART_OK:
      // everything ok
      break;
    case Smart::SMART_SERVICEUNAVAILABLE:
      // the servercmpt is unknown or does not provide a wiring slave
      return Smart::SMART_UNKNOWNCOMPONENT;
      break;
    case Smart::SMART_INCOMPATIBLESERVICE:
      // the "wiring" service of the servercmpt is not compatible to the master
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

  request.setCommand("disconnect",slaveprt,"","");

  status = client.query(request,answer);
  if (status != Smart::SMART_OK) return status;

  answer.getStatus(status);

  return status;
}


/////////////////////////////////////////////////////////////////////////
//
// handler of slave part of wiring pattern
//
/////////////////////////////////////////////////////////////////////////

//
// default constructor
//
SmartACE::WiringHandler::WiringHandler(void) 
:	QueryServerHandler<SmartCommWiring,SmartCommWiring>(0)
{
  std::cerr << "CommPattern (WiringHandler): ERROR: Entered default constructor WiringHandler" << std::endl;
  wiringslave=0;
  throw(SmartACE::SmartError(Smart::SMART_ERROR,"CommPattern (WiringHandler): ERROR: Entered default constructor WiringHandler"));
}

//
// standard constructor
//
SmartACE::WiringHandler::WiringHandler(SmartACE::WiringSlave *s) 
:	QueryServerHandler<SmartCommWiring,SmartCommWiring>(s->wiring)
,	wiringslave(s)
{
  //
}

//
// destructor
//
SmartACE::WiringHandler::~WiringHandler() 
{
  //
}

//
//
//
void SmartACE::WiringHandler::handleQuery(const QueryId &id, const SmartCommWiring& request)
{
  SmartCommWiring answer;

  answer = wiringslave->handleWiring(request);

  this->server->answer(id,answer);
}




/////////////////////////////////////////////////////////////////////////
//
// slave part of wiring pattern
//
/////////////////////////////////////////////////////////////////////////

//
// default constructor
//
SmartACE::WiringSlave::WiringSlave(void) 
{
  std::cerr << "CommPattern (WiringSlave): ERROR: Entered default constructor WiringSlave" << std::endl;
  component=0;
  ports=0;
  handler=0;
  wiring=0;
  threadHandler=0;
  throw(SmartACE::SmartError(Smart::SMART_ERROR,"CommPattern (WiringSlave): ERROR: Entered default constructor WiringSlave"));
}

//
//
//
SmartACE::WiringSlave::WiringSlave(SmartComponent* m, std::string slaveaddress)  : component(m)
{
  // reset port list
  ports = 0;

  // added functionality to work without NamingService
  if(component->getName().compare(SMART_NONS) == 0) {
	  if(slaveaddress == "") {
		wiring = 0;
		SMARTSOFT_PRINT( ACE_TEXT("<WiringSlave> Warning: wiring slave is not initialized properly. Please use second parameter of WiringSlave-constuctor!") );
	  }else{
		  // Slaveaddress has to contain a propper "ip:portnr" string for WiringSlave.
		  // This "ip:portnr" will be used by WiringMaster to connect to WiringSlave!
		  wiring = new QueryServer<SmartCommWiring,SmartCommWiring>(component,slaveaddress);
	  }
  }else{
	wiring  = new QueryServer<SmartCommWiring,SmartCommWiring>(component,"wiring");
  }

  handler = new WiringHandler(this);

  //<alexej date="2009-06-17">
  // Handling is done in separate thread, otherwise blocking of main thread occures!
  threadHandler = new ThreadQueueQueryHandler<SmartCommWiring,SmartCommWiring>(component,handler);

  //</alexej>
}

//
//
//
SmartACE::WiringSlave::~WiringSlave(void) 
{
  PortList *lptr;

  mutex.acquire();

  // free memory
  while ( ports != 0 ) {
    lptr  = ports;
    ports = ports->next;
    delete lptr;
  }

  delete handler;
  delete threadHandler;

  mutex.release();
}

//
//
//
SmartACE::SmartCommWiring SmartACE::WiringSlave::handleWiring(const SmartACE::SmartCommWiring& request)
{
  SmartACE::SmartCommWiring answer;
   Smart::StatusCode      status;
  std::string          command,portname,servername,servicename;
  PortList             *lptr = 0;

  request.getCommand(command,portname,servername,servicename);
  
  mutex.acquire();

  if (command == "connect") {
    //
    // look for the portname
    //  
    for( lptr = ports; lptr != 0; lptr = lptr->next )
      if( lptr->portname == portname)
        break;

    if (lptr != 0) {
      //
      // portname known, now call the connect method of the client object
      //
      status = (lptr->cPtr)(lptr->tPtr,servername,servicename);      

      answer.setStatus(status);
    } else {
      answer.setStatus(Smart::SMART_UNKNOWNPORT);
    }
  } else if (command == "disconnect") {
    //
    // look for the portname
    //
    for( lptr = ports; lptr != 0; lptr = lptr->next )
      if( lptr->portname == portname)
        break;

    if (lptr != 0) {
      //
      // portname known, now call the disconnect method of the client object
      //
      status = (lptr->dPtr)(lptr->tPtr);

      answer.setStatus(status);
    } else {
      answer.setStatus(Smart::SMART_UNKNOWNPORT);
    }
  } else {
    answer.setStatus(Smart::SMART_ERROR);
  }

  mutex.release();

  return answer;
}


//
//
//
 Smart::StatusCode SmartACE::WiringSlave::add(const std::string& port, void* tPtr,  Smart::StatusCode (*cPtr)(void*,const std::string&,const std::string&),  Smart::StatusCode (*dPtr)(void*))
{
  PortList        *lptr;
   Smart::StatusCode result;

  if(wiring == 0) {
    SMARTSOFT_PRINT( ACE_TEXT("<WiringSlave> Warning: WiringSlave is not initialized properly. Probably WiringSlave is configured to work without NamingService - please check second parameter of WiringSlave-constructor!") );
  }

  mutex.acquire();

  // first check, whether portname is already in use
  for( lptr = ports; lptr != 0; lptr = lptr->next )
    if( lptr->portname == port)
      break;

  if (lptr != 0) {
    //
    // portname already used
    //
    result = Smart::SMART_PORTALREADYUSED;
  } else {
    //
    // add client to list of ports
    //
    lptr           = new PortList;
    lptr->portname = port;
    lptr->tPtr     = tPtr;
    lptr->cPtr     = cPtr;
    lptr->dPtr     = dPtr;
    lptr->next     = ports;
    ports          = lptr;

    result = Smart::SMART_OK;
  }

  mutex.release();

  return result;
}

//
//
//
 Smart::StatusCode SmartACE::WiringSlave::remove(const std::string& port)
{
  PortList        *lptr, *hlptr;
   Smart::StatusCode result;

  mutex.acquire();

  // first check, whether portname is known
  for( lptr = ports; lptr != 0; lptr = lptr->next )
    if( lptr->portname == port)
      break;

  if (lptr != 0) {
    //
    // found port to be removed and now delete list entry
    //
    if ( lptr == ports) {
      ports = lptr->next;
      delete lptr;
    } else {
      for (hlptr = ports; hlptr && hlptr->next != 0; hlptr = hlptr->next) {
        if( hlptr->next == lptr) {
          hlptr->next = hlptr->next->next;
          delete lptr;
        }
      }
    }
    result = Smart::SMART_OK;
  } else {
    result = Smart::SMART_UNKNOWNPORT;
  }

  mutex.release();

  return result;
}

