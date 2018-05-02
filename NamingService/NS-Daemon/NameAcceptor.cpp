// --------------------------------------------------------------------------
//
//  Copyright (C) 2014 Alex Lotz
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

#include "NameAcceptor.h"

#include <ace/Get_Opt.h>
#include <ace/Signal.h>
#include <ace/Sched_Params.h>

void NameAcceptor::printHelp(std::ostream &oss)
{
    oss << "\nThis is the help for NamingService's command line parameters." << std::endl;
    oss << "In the following, the accepted parameters are listed:" << std::endl;
    oss << "-h or --help" << std::endl;
    oss << "\tPrints out this help." << std::endl;
    oss << "-p <nbr> or --ns-port <nbr> or --ns-port=<nbr>" << std::endl;
    oss << "\tIs used to define the port number of the naming service." << std::endl;
    oss << "\tReplace <nbr> by the decimal value for the port number." << std::endl;
    oss << "\te.g. --ns-port 12345" << std::endl;
    oss << "-f <file> or --ns-file <file> or --ns-file=<file>" << std::endl;
    oss << "\tIs used to define the database file name, which is used by" << std::endl;
    oss << "\tthe naming service to persistently store the name-value entries." << std::endl;
    oss << "\te.g. --ns-file SMART_NAMES" << std::endl;
    oss << "-d <path> or --ns-dir <path> or --ns-dir=<path>" << std::endl;
    oss << "\tThe <path> defines the directory where the database is stored." << std::endl;
    oss << "\te.g. --ns-dir /tmp" << std::endl;
    oss << std::endl;
    oss << "\tThe following options are partially accepted to be" << std::endl;
    oss << "\tbackwards compatible to commanline arguments used by" << std::endl;
    oss << "\tSmartSoft components:" << std::endl;
    oss << "-a <IP>:<nbr> or --ns-addr <IP>:<nbr> or --ns-addr=<IP>:<nbr>" << std::endl;
    oss << "\tIs used to define both, the TCP/IP address and Port number" << std::endl;
    oss << "\tof the naming service, separated by a ':'. However, the" << std::endl;
    oss << "\tthe Naming Service is always initialized with the IP 0.0.0.0" << std::endl;
    oss << "\twhich allows the Naming Service to accept incoming requests" << std::endl;
    oss << "\tfrom all network interfaces. Thus, only the port number from" << std::endl;
    oss << "\tthis parameter is used together with the IP 0.0.0.0." << std::endl;
    oss << "-i <IP> or --ns-ip <IP> or --ns-ip=<IP>" << std::endl;
    oss << "\tThis parameter is only available for compatibility" << std::endl;
    oss << "\treasons and is completely ignored at the moment. Instead," << std::endl;
    oss << "\tthe IP 0.0.0.0 is used to allow the Naming Service to" << std::endl;
    oss << "\taccept all request from all available network interfaces." << std::endl;
    oss << "-s <SchedPolicy> or --sched <SchedPolicy> or --sched=<SchedPolicy>" << std::endl;
    oss << "\tIs used to define the scheduling policy such as FIFO or RR" << std::endl;
    oss << std::endl;
    oss << "\tIf some or all the parameters above are not used," << std::endl;
    oss << "\tthen the following environment variable is checked:" << std::endl;
    oss << "$SMART_NS_ADDR=<IP>:<PORT> (for --ns-addr)" << std::endl;
    oss << "\tThis variable is shared by all SmartSoft components to" << std::endl;
    oss << "\tbe able to connect to this Naming Service. On the other hand," << std::endl;
    oss << "\tthe Naming Service itself can use this variable to initialize" << std::endl;
    oss << "\tits internal server. Therefore only the port number is used" << std::endl;
    oss << "\tand for the IP address the IP 0.0.0.0 is used instead." << std::endl;
    oss << std::endl;
    oss << "\tIn case none of the previously mentioned variants were used," << std::endl;
    oss << "\tframework's internal default values are taken. These are:" << std::endl;
    oss << "\t--ns-port: " << ACE_DEFAULT_SERVER_PORT << std::endl;
    oss << "\t--ns-file: SMART_NAMES" << std::endl;
    oss << "\t--ns-dir: /tmp" << std::endl;
    oss << "<<END OF PARAMETER HELP>>" << std::endl;
    oss << std::endl;
}

int
NameAcceptor::parse_args (int argc, ACE_TCHAR *argv[])
{
  ACE_TRACE (ACE_TEXT ("NameAcceptor::parse_args"));

  int port_number = -1;
  std::string file_str = "";
  std::string dir_str = "";
  std::string arg_str = "";
  std::string sched_str = "";
  size_t pos_separator = std::string::npos;

  ACE_Get_Opt get_opt(argc, argv, ACE_TEXT("p:f:d:a:i:s:h"));
  get_opt.long_option(ACE_TEXT("help"), 'h', ACE_Get_Opt::NO_ARG);
  get_opt.long_option(ACE_TEXT("ns-port"), 'p', ACE_Get_Opt::ARG_REQUIRED);
  get_opt.long_option(ACE_TEXT("ns-file"), 'f', ACE_Get_Opt::ARG_REQUIRED);
  get_opt.long_option(ACE_TEXT("ns-dir"), 'd', ACE_Get_Opt::ARG_REQUIRED);
  get_opt.long_option(ACE_TEXT("sched"), 's', ACE_Get_Opt::ARG_REQUIRED);
  // the following arguments are only for compatibility reasons to be
  // consistent with the SmartSoft components. Thereby the TCP/IP argument
  // is always ignored, instead the given port number together with
  // the IP 0.0.0.0 is used. This allows the Naming Service to accept
  // requests from any available network interface.
  get_opt.long_option(ACE_TEXT("ns-addr"), 'a', ACE_Get_Opt::ARG_REQUIRED);
  get_opt.long_option(ACE_TEXT("ns-ip"), 'i', ACE_Get_Opt::ARG_REQUIRED);

  for(int c; (c = get_opt()) != -1;) {
    switch(c) {
    case 'h':
    	// help is requested, print out the help
    	printHelp();
      break;
    case 'p':
    	// the port-number is passed as parameter
    	arg_str = get_opt.opt_arg();
    	port_number = ACE_OS::atoi(arg_str.c_str());
      break;
    case 'f':
    	file_str = get_opt.opt_arg();
      break;
    case 'd':
    	dir_str = get_opt.opt_arg();
      break;
    case 's':
    	sched_str = get_opt.opt_arg();
      break;
    case 'a':
    	arg_str = get_opt.opt_arg();
    	pos_separator = arg_str.find(':');
    	if(pos_separator != std::string::npos && (pos_separator+1) < arg_str.size()) {
    		port_number = ACE_OS::atoi(arg_str.substr(pos_separator+1).c_str());
			ACE_DEBUG ((LM_DEBUG, ACE_TEXT("--ns-addr argument was found, its IP part is ignored, instead the IP 0.0.0.0 with the port number %d is used!\n"), port_number ));
    	}
      break;
    case 'i':
    	ACE_DEBUG ((LM_DEBUG, ACE_TEXT("--ns-ip argument was found, it is ignored and the IP 0.0.0.0 is used instead!\n") ));
      break;
    default: break;
    }
  }

  if(sched_str == "") {
    processor_scheduling_policy_ = -1;
  } else {
    if(sched_str == "FIFO") {
      processor_scheduling_policy_ = ACE_SCHED_FIFO;
      processor_scheduling_priority_ = ACE_THR_PRI_FIFO_MIN;
    } else if(sched_str == "RR") {
      processor_scheduling_policy_ = ACE_SCHED_RR;
      processor_scheduling_priority_ = ACE_THR_PRI_RR_MIN;
    }
  }

  const char* env_var = ACE_OS::getenv("SMART_NS_ADDR");
  // if environment variable is defined, try to extract the argument
  if(env_var != 0) {
    // try to parse the string from the format '<hostname>:<port-nbr>'
	arg_str = env_var;
	pos_separator = arg_str.find(':');
	if(pos_separator != std::string::npos && (pos_separator+1) < arg_str.size()) {
		if(port_number == -1) {
			port_number = ACE_OS::atoi(arg_str.substr(pos_separator+1).c_str());
			ACE_DEBUG ((LM_DEBUG, ACE_TEXT("$SMART_NS_ADDR environment variable was found, the port number %d provided in this variable is used to initialize the naming service (the IP is ignored and 0.0.0.0 is used instead)!\n"), port_number ));
		}
	}
  }

  if(port_number == -1) {
    port_number = ACE_DEFAULT_SERVER_PORT;
  }

  if(file_str == "") {
	  file_str = "SMART_NAMES";
  }

  ACE_LOG_MSG->open (ACE_TEXT ("Name Service"));

  // set-up the name_options of the underlying naming service according to the
  // command line arguments (as parsed above)
  // (we do not use the parse_args method from name_options since we do
  //  not need this flexibility in this case here)
  if(dir_str != "") {
	  this->naming_context()->name_options()->namespace_dir(dir_str.c_str());
  }
  if(file_str != "") {
	  this->naming_context()->name_options()->database(file_str.c_str());
  }
  // we set the naming context to PROC_LOCAL, since it is used only internally in this process
  this->naming_context()->name_options()->context(ACE_Naming_Context::PROC_LOCAL);

  // by setting the port-number only, the IP is automatically set to 0.0.0.0
  this->service_addr_.set ((u_short)port_number);

  ACE_DEBUG((LM_DEBUG, ACE_TEXT("The following naming service parameters are detected:\n") ));
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("\tns-port: %d\n"), port_number ));
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("\tns-file: %s\n"), this->naming_context()->name_options()->database() ));
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("\tns-dir: %s\n"), this->naming_context()->name_options()->namespace_dir() ));
  if(sched_str != "") {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("\tscheduler: %s\n"), sched_str.c_str() ));
  }

  return 0;
}

int
NameAcceptor::init (int argc, ACE_TCHAR *argv[])
{
  ACE_TRACE (ACE_TEXT ("NameAcceptor::init"));

  // Use the options hook to parse the command line arguments and set
  // options.
  if( this->parse_args (argc, argv) == -1 )
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_TEXT ("%p\n"),
                       ACE_TEXT ("NameAcceptor::parse_args failed")),
                      -1);

  if(processor_scheduling_policy_ != -1) {
    ACE_Sched_Params sched_params(processor_scheduling_policy_, processor_scheduling_priority_, ACE_SCOPE_PROCESS);
    if(ACE_OS::sched_params(sched_params) == -1) {
      if(errno == EPERM || errno == ENOTSUP) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("Warning: user is not superuser, so the default scheduler will be used\n")));
      } else {
        ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"),ACE_TEXT("ACE_OS::sched_params()")),-1);
      }
    }
  }

  // initialize the internal naming context (only used within this process)
  if (this->naming_context()->open( naming_context()->name_options()->context() ) == -1)
	  ACE_ERROR_RETURN ((LM_ERROR,
						ACE_TEXT ("%n:\n open naming context failed.\n")),
					   -1);

  // Set the acceptor endpoint into listen mode (use the Singleton
  // global Reactor...).
  if (this->open (this->service_addr_,
                  ACE_Reactor::instance (),
                  0, 0, 0,
                  &this->scheduling_strategy_,
                  ACE_TEXT ("Name Server"),
                  ACE_TEXT ("ACE naming service")) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_TEXT ("%n: %p on port %d\n"),
                       ACE_TEXT ("acceptor::open failed"),
                       this->service_addr_.get_port_number ()),
                      -1);

  // Ignore SIGPIPE so that each <SVC_HANDLER> can handle this on its
  // own.
  ACE_Sig_Action sig ((ACE_SignalHandler) SIG_IGN, SIGPIPE);
  ACE_UNUSED_ARG (sig);

  ACE_INET_Addr server_addr;

  // Figure out what port we're really bound to.
  if (this->acceptor ().get_local_addr (server_addr) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_TEXT ("%p\n"),
                       ACE_TEXT ("get_local_addr")),
                      -1);

  //<alexej date="2009-08-21" description="Some sugar for debug output">
  ACE_DEBUG ((LM_DEBUG,
      //ACE_TEXT ("starting up Name Server at port %d on handle %d\n"),
      ACE_TEXT ("Naming service is started at port %d on PC %s \nand listens to addr: %s;\n")
                , server_addr.get_port_number()
                , server_addr.get_host_name()
                , server_addr.get_host_addr()
                ));
                //this->acceptor ().get_handle ()));

  //ACE_DEBUG(( LM_DEBUG, ACE_TEXT("Name_Handler: namespace_dir: %s\n"), naming_context()->name_options()->namespace_dir() ));
  //ACE_DEBUG(( LM_DEBUG, ACE_TEXT("Name_Handler: database_name: %s\n"), naming_context()->name_options()->database() ));
  //</alexej>
  return 0;
}

LockingProxy<ACE_Naming_Context>
NameAcceptor::naming_context (void)
{
  return LockingProxy<ACE_Naming_Context>(&naming_context_, mutex_);
}


