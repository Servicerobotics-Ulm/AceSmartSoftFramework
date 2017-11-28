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

#ifndef _SMARTMESSAGEQUEUE_HH
#define _SMARTMESSAGEQUEUE_HH

#include "smartOSMapping.hh"

#include <queue>

namespace SmartACE {

  /** unbounded message queue for communication between threads.
   *
   *  This queue can be used to connect two threads together, for
   *  example to enqueue commands and queries that can't be executed
   *  inside a SendHandler, or QueryHandler.
   *
   *  Queue items are processed in FIFO order.
   */
  template <class Item>
  class MessageQueue
  {
  private:
    // used for notification
    ACE_Semaphore sema;
    // used to protect access to stl container
    ACE_Mutex     mutex;
    std::queue<Item> queue;
  public:
    /** constructor.
     */
    MessageQueue();

    /** destructor */
    virtual ~MessageQueue();


    /** enqueue an item.
     *
     *  @param item to enqueue. A copy of the item is stored in the queue
     */
    void enqueue(const Item & item);


    /** dequeue an item.
     *
     *  dequeues the head item. if no item is available, blocks till
     *  one arrives.
     *
     *  @return copy of the enqueued item.
     */
    Item dequeue();
  };
};

// include template implementation
#include "smartMessageQueue.th"

#endif // _SMARTMESSAGEQUEUE_HH
