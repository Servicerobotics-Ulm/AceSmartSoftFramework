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

#ifndef _SMARTPROCESSINGPATTERNS_HH_
#define _SMARTPROCESSINGPATTERNS_HH_

#include <smartIProcessingPatterns_T.h>

#include "smartQueryServerHandler.hh"
//#include "smartSend.hh"

namespace SmartACE {

/** Decorator for QueryServerHandler to defer handling to another
 *  thread.
 *
 *  This Decorator (see Design Patterns by Gamma et. al) can be used
 *  if a QueryServerHandler is slow and/or blocking and would
 *  therefore block the framework. This handler is an active object
 *  and has a single thread, in which the Queries are handled one after
 *  another.
 *
 *  example usage:
 *  \code
 *
 *  MySlowQueryHandler: public QueryServerHandler<R,A>
 *  {
 *    ...
 *  };
 *
 *  ...
 *
 *  MySlowQueryHandler slowHandler;
 *  ThreadQueueQueryHandler<R,A> threadedHandler(slowHandler);
 *  QueryServer queryService<R,A>(component,"heavy_computation", threadedHandler);
 *  \endcode
 */
template<class RequestType, class AnswerType>
class ThreadQueueQueryHandler
:	public Smart::IActiveQueryServerHandler<RequestType,AnswerType>
,	public SmartACE::Task
{
private:
	virtual int task_execution() override {
		return this->process_fifo_queue();
	}
public:
	using IQueryServerHandlerPtr = std::shared_ptr<Smart::IQueryServerHandler<RequestType,AnswerType>>;

	virtual int start() override {
		return SmartACE::Task::start();
	}

	virtual int stop(const bool wait_till_stopped=true) override
	{
		this->signal_to_stop();
		return SmartACE::Task::stop();
	}

	/** Create a new threaded QueryServerHandler Decorator.
	 *
	 *  The internal handling thread is started/stopped automatically.
	 *
	 *  @param component          the pointer to the surrounding component
	 *  @param inner_handler_ptr  which will be called in a separate thread.
	 */
	ThreadQueueQueryHandler(Smart::IComponent *component, IQueryServerHandlerPtr inner_handler_ptr)
	:	Smart::IActiveQueryServerHandler<RequestType,AnswerType>(inner_handler_ptr)
	,	SmartACE::Task(component)
	{
		this->start();
	}

	virtual ~ThreadQueueQueryHandler()
	{
		this->stop();
	}
};

//FIXME:
//  /** Decorator for SendServerHandler to defer handling to another
//   *  thread.
//   *
//   *  This decorator (see Design Patterns by Gamma et. al) can be used
//   *  if a SendServerHandler is slow and/or blocking and would
//   *  therefore block the framework. This handler is an active object
//   *  and has a single thread, in which the Sends are handled one after
//   *  another.
//   *
//   *  example usage:
//   *  \code
//   *
//   *  MySlowSendHandler: public SendServerHandler<...>
//   *  {
//   *    ...
//   *  };
//   *
//   *  ...
//   *
//   *  MySlowSendHandler slowHandler;
//   *  ThreadQueueSendHandler threadedHandler(slowHandler);
//   *  SendServer sendService<R>(component,"longcmd", threadedHandler);
//   *  \endcode
//   */
//  template <class C>
//  class ThreadQueueSendHandler : public Smart::Task,
//				 public SendServerHandler<C>
//  {
//  protected:
//    /** original handler.
//     */
//    SendServerHandler<C> & handler;
//
//    /** queue entry for a command */
//    struct MsgQueueItem{
//      MsgQueueItem() : cmd() {};
//      MsgQueueItem(const C& cmd) : cmd(cmd) {};
//      C cmd;
//    };
//
//    /** message queue to our thread.
//     */
//    MessageQueue<MsgQueueItem> msgQueue;
//
//  public:
//
//    /** Create a new threaded SendServerHandler Decorator.
//     *
//     *  the handling thread can be started automatically, or with a
//     *  separate open();
//     *
//     *  @param handler which will be called in a separate thread.
//     *  @param start   start the handler thread (default)
//     *
//     */
//    ThreadQueueSendHandler(SendServerHandler<C> & handler, bool start=true)
//      : handler(handler)
//      {
//	if (start) {
//	  open();
//	}
//      };
//
//    /** @internal method called by the pattern. */
//    void handleSend( const C& cmd) 
//      {
//	    // defer query to thread.
//	    msgQueue.enqueue(MsgQueueItem(cmd));
//      };
//
//    /** method that runs in the active objects thread.
//     *
//     *  The default svc() just sits in an loop waiting for incoming
//     *  sends, and calling the handler registred on construction. If
//     *  another behaviour is needed it can be added by subclassing.
//     *
//     *  The loop can be cooperatively cancelled with SmartThreadManager.
//     */
//    virtual int svc() {
//      // sit here and wait for requests
//      MsgQueueItem item;
//      while(! SmartACE::SmartThreadManager::instance()->testcancel(ACE_Thread::self ())) {
//	    item = msgQueue.dequeue();
//	    handler.handleSend(item.cmd);
//      }
//      return 0;
//    }
//  };
//
//  /** Decorator for PushTimedServerHandler to defer handling to another
//   *  thread.
//   *
//   *  This decorator (see Design Patterns by Gamma et. al) can be used
//   *  if a PushTimedServerHandler is slow and/or blocking and would
//   *  therefore block the framework. This handler is an active object
//   *  and has a single thread, in which the Pushes are handled one after
//   *  another.
//   *
//   *  example usage:
//   *  \code
//   *
//   *  MySlowPushTimedHandler: public PushTimedHandler<...>
//   *  {
//   *    ...
//   *  };
//   *
//   *  ...
//   *
//   *  MySlowPushTimedHandler slowHandler;
//   *  ThreadQueuePushTimedHandler threadedHandler(slowHandler);
//   *  PushTimedServer pushService<R>(component,"longcmd", threadedHandler);
//   *  \endcode
//   */
//
//  template <class C>
//  class ThreadQueuePushTimedHandler : public Smart::Task,
//				 public PushTimedHandler<C>
//  {
//  protected:
//    /** original handler.
//     */
//    PushTimedHandler<C> & handler;
//
//    /** queue entry for a command */
//    struct MsgQueueItem{
//      MsgQueueItem() : server(0) {};
//      MsgQueueItem(PushTimedServer<C> & server) : server(&server) {};
//      SmartACE::PushTimedServer<C> * server;
//    };
//
//    /** message queue to our thread.
//     */
//    MessageQueue<MsgQueueItem> msgQueue;
//
//  public:
//
//    /** Create a new threaded PushTimedHandler Decorator.
//     *
//     *  the handling thread can be started automatically, or with a
//     *  separate open();
//     *
//     *  @param handler which will be called in a separate thread.
//     *  @param start   start the handler thread (default)
//     *
//     */
//    ThreadQueuePushTimedHandler(PushTimedHandler<C> & handler, bool start=true)
//      : handler(handler)
//      {
//	if (start) {
//	  open();
//	}
//      };
//
//    /** @internal method called by the pattern. */
//    void handlePushTimer(SmartACE::PushTimedServer<C> & server) 
//      {
//	    // defer query to thread.
//	    msgQueue.enqueue(MsgQueueItem(server));
//      };
//
//    /** method that runs in the active objects thread.
//     *
//     *  The default svc() just sits in an loop waiting for incoming
//     *  timeouts, and calling the handler registred on construction. If
//     *  another behaviour is needed it can be added by subclassing.
//     *
//     *  The loop can be cooperatively cancelled with SmartThreadManager.
//     */
//    virtual int svc() {
//      // sit here and wait for requests
//      MsgQueueItem item;
//      while(! SmartACE::SmartThreadManager::instance()->testcancel(ACE_Thread::self ())) {
//	    item = msgQueue.dequeue();
//	    handler.handlePushTimer(*(item.server));
//      }
//      return 0;
//    }
//  };


};
  
  

#endif // _SMARTPROCESSINGPATTERNS_HH_
