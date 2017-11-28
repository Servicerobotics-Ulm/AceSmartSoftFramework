// --------------------------------------------------------------------------
//
//  Copyright (C) 2009 Alex Lotz
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

#include "ace/Service_Config.h"
#include "ace/Logging_Strategy.h"
#include "ace/Sig_Adapter.h"
#include "ace/Get_Opt.h"
#if ACE_MAJOR_VERSION < 6 
#include "ace/OS.h"
#else
#include "ace/OS_main.h"
#endif

#include "NameHandler.h"
#include "NameAcceptor.h"
#include "smartVersionHelper.hh"

#include "RemoteNSTask.h"

#if ACE_MAJOR_VERSION < 6 
ACE_RCSID(servers, main, "$Id: main.cpp 27 2011-02-16 17:07:52Z alotz $")
#endif


//---------------------------------------------------------
//
// MAIN
//
//---------------------------------------------------------
int
ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
   ACE_TString version;

   //<alexej date="2009-10-19">
   //if( SmartACE::VersionHelper::get_ace_version(version) == 0 )
   //{
   //   ACE_DEBUG((LM_INFO, ACE_TEXT("\nFound ACE version file:\n%s\n"), version.c_str() ));
   //}

   ACE_DEBUG((LM_DEBUG, ACE_TEXT("\nThis progam uses ACE version %u.%u.%u\n"), ACE::major_version(), ACE::minor_version(), ACE::beta_version() ));

   if( SmartACE::VersionHelper::get_smartsoft_version(version) == 0 )
   {
      ACE_DEBUG((LM_INFO, ACE_TEXT("\nFound SmartSoft version file:\n%s\n\n"), version.c_str() ));
   }

   NameAcceptor acceptor;
   // init opens both, the internal ACE_Naming_Context and the acceptor interface
   if(acceptor.init(argc, argv) != 0) {
      ACE_DEBUG((LM_ERROR, ACE_TEXT("NameAcceptor initialization ERROR, return -1!\n")));
      return -1;
   }

   // propagate the pointer to the acceptor (used to access the local Name_Space)
   REMOTE_NS::instance()->setAcceptor(&acceptor);

   if(REMOTE_NS::instance()->init(argc, argv) != 0) {
	   REMOTE_NS::instance()->fini();
	   ACE_DEBUG((LM_ERROR, ACE_TEXT("RemoteNSTask initialization ERROR, return -1!\n")));
	   return -1;
   }

   return ACE_Reactor::instance()->run_reactor_event_loop();
}


