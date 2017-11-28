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

#include "smartSIDhandler.hh"

/////////////////////////////////////////////////////////////////////////
//
// SIDhandler (ServerInitiatedDisconnectHandler)
//
/////////////////////////////////////////////////////////////////////////
SmartACE::SIDhandler::SIDhandler(Smart::IComponent *component)
:	Task(component)
,	sid_mutex()
{
   // set the disconnect list to NULL
   listDisconnectHandle = 0;
}

void SmartACE::SIDhandler::sign_on(void *handle)
{
   SmartDisconnectHandleStruct *cptr;

   sid_mutex.acquire();
      cptr                 = new SmartDisconnectHandleStruct;
      cptr->handle         = handle;
      cptr->next           = listDisconnectHandle;
      listDisconnectHandle = cptr;
   sid_mutex.release();
}

void SmartACE::SIDhandler::sign_off(void *handle)
{
   SmartDisconnectHandleStruct *cptr;
   SmartDisconnectHandleStruct *hcptr;

   sid_mutex.acquire();

   // normally a condition variable is at most once in the list but you never know ...

   //
   // first remove entries from head as long as they match the handle variable
   //
   while ((listDisconnectHandle != 0) && (listDisconnectHandle->handle == handle)) {
     cptr = listDisconnectHandle;
     listDisconnectHandle = listDisconnectHandle->next;
     delete cptr;
   }

   //
   // now search through the rest of the list which might be empty already. If the
   // length is greater than zero, than the first element never matches and never
   // gets deleted
   //
   cptr = listDisconnectHandle;
   if (cptr == 0) {
     // list contains no element anymore => ready
   } else {
     // list can contain one or more elements with the first element matching never
     while (cptr->next != 0) {
       if (cptr->next->handle == handle) {
         hcptr      = cptr->next;
         cptr->next = cptr->next->next;
         delete hcptr;
       } else {
         cptr = cptr->next;
       }
     }
   }

   sid_mutex.release();
}

int SmartACE::SIDhandler::task_execution()
{
   SmartDisconnectHandleStruct *cptr;
   SmartMessageBlock *mblk;

   //<alexej date="2010-03-09">
   void (*internalServInitDisc)(void *,int);
   //<alexej date="2010-03-09">

   while(!this->test_canceled())
   {
      //blocking get message from internal queue
      if( this->getq(mblk) == -1 )
      {
         // error case
      }else{
         if(mblk->size() == 0 && mblk->msg_type() == ACE_Message_Block::MB_STOP)
         {
            //ACE_OS::printf("SIDHandler: MB_STOP message received!\n");
            mblk->release();
            break;
         }

         disc_node current_node;
         ACE_OS::memcpy((void*)&current_node, mblk->rd_ptr(), sizeof(disc_node));
         //delete message buffer
         mblk->release();

         sid_mutex.acquire();

         cptr = listDisconnectHandle;
         while(cptr != 0)
         {
            if( current_node.lptr == cptr->handle )
            {
               internalServInitDisc = current_node.fktPtr;
               internalServInitDisc(current_node.lptr, current_node.cid);
            }
            if(cptr != 0) cptr = cptr->next;
         }
         sid_mutex.release();
      }
   }

   return msg_queue()->close();
}

int SmartACE::SIDhandler::enqueue(void *ptr,
   //<alexej date="2010-03-09">
      void (*internalServInitDisc)(void *,int),
      int cid)
   //</alexej>
{
	// local variables
	SmartMessageBlock *mblk;
	disc_node temp;

	//create sid-node
	temp.lptr = ptr;
	temp.fktPtr = internalServInitDisc;
	temp.cid = cid;

	// create message for storing the sid-node
	ACE_NEW_RETURN(mblk, SmartMessageBlock(sizeof(disc_node)), -1);

	// copy sid-node into the message object
	mblk->copy((char*)&temp, sizeof(temp));

	// put message object onto the message queue
	return this->putq(mblk);
}

void SmartACE::SIDhandler::on_shutdown() {
	// do nothing
}

int SmartACE::SIDhandler::stop_mqueue(void)
{
   ACE_Message_Block *mbstop=0;
   ACE_NEW_RETURN(mbstop, ACE_Message_Block(0, ACE_Message_Block::MB_STOP), -1);

   return this->putq(mbstop);
}


