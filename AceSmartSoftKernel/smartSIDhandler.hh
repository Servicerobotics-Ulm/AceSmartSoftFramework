// --------------------------------------------------------------------------
//
//  Copyright (C) 2011 Alex Lotz
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

#ifndef SMARTSIDHANDLER_HH_
#define SMARTSIDHANDLER_HH_

#include "smartOSMapping.hh"
#include "smartTask.hh"

namespace SmartACE {

    /** @internal
     *  Class SIDhandler(ServerInitiatedDisconnect-Handler)
     *
     *  This class is used for handling various ServerIniated disconnect requests.
     *  This is necessary, because this requests are blocking (they wait for answer-disconnect
     *  message). The problem is, that to get the answer - services should be in unblocked state.
     *  To get this problem solved, an own thread for all SID request is presented in Component.
     *  So all requests from all Ports are handled sequentially in SIDhandler class.
     */
	class SIDhandler
		:	public SmartACE::Task
	{
	private:
           /** @internal
            * reference to parent component class
            */
           //SmartComponent *component;

           // internal structure for the sid list
           typedef struct SmartDisconnectHandleStruct {
             void *handle;
             struct SmartDisconnectHandleStruct *next;
           }SmartDisconnectHandleStruct;

           // sid list ptr
           SmartDisconnectHandleStruct *listDisconnectHandle;

           /** @internal
            * structure for storing nodes
            */
           struct disc_node{
              void *lptr;
              //<alexej date="2010-03-09">
                 void (*fktPtr)(void *,int);
                 int cid;
              //</alexej>
           };

           /** @internal
            * Main work routine. It reacts on an internal active message queue.
            * Gets all SID requests from this queue and performs them.
            */
           virtual int task_execution();

           virtual void on_shutdown();

           // @internal mutex
           SmartMutex sid_mutex;

   public:

           /** @ internal
            * default constuctor
            */
          SIDhandler(Smart::IComponent *component);

          /** @internal
           * see thesis from Schlegel on page 195!
           */
          void sign_on(void *ptr);

          /** @internal
           * see thesis from Schlegel on page 195!
           */
          void sign_off(void *ptr);

           /** @internal
            * With that function one can register a new SID request.
            * This function assembles the appropriate message and
            * enques it on the internal message queue, whitch is than
            * handled by svc() function (see above).
            */
           int enqueue(void *ptr,
              //<alexej date="2010-03-09">
                 void (*internalServInitDisc)(void *,int),
                 int cid);
              //</alexej>

           /** @internal
            *
            *
            */
           int stop_mqueue(void);
	};

} // end namespace CHS

#endif // SMARTSIDHANDLER_HH_
