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

#ifndef ACESMARTSOFTKERNEL_SMARTQUERYSERVERHANDLER_HH_
#define ACESMARTSOFTKERNEL_SMARTQUERYSERVERHANDLER_HH_

#include <smartIQueryServerPattern_T.h>

namespace SmartACE {

  /** Handler Class for QueryServer for incoming requests.
   *
   *  Used by the QueryServer to handle incoming queries.
   *  The user should provide the handleQuery() method by
   *  subclassing and register an instance of this handler
   *  class with the QueryServer.
   */
  template<class R, class A>
  class QueryServerHandler : public Smart::IQueryServerHandler<R,A>
  {
  public:
    virtual ~QueryServerHandler() = default;

    /// this Alias simplifies usage in the handleQuery method
    /// @sa handleQuery
    using IQueryServer = Smart::IQueryServerPattern<R,A>;

    /** Handler method for an incoming query request.
     *
     *  This method is called by the communication pattern every time
     *  a new query request is received. It must be provided by the
     *  component developer to handle incoming requests. Since the
     *  method is executed by the communication thread, it must be
     *  very fast and non-blocking.
     *
     *  Usually the request and the id will be inserted into a queue
     *  and another working thread processes the request and provides
     *  the result. The ThreadedQueryHandler decorator provides such
     *  a processing pattern.
     *
     *  @param server   a reference to the "parent" query-server that triggers this handler callback; can be used to provide the query-answer
     *  @param id       the query-id can be used to provide the query answer
     *  @param request  the request itself
     *
     *  @sa IQueryServer
     */
    virtual void handleQuery(IQueryServer &server, const Smart::QueryIdPtr &id, const R& request) = 0;
  };


}


#endif /* ACESMARTSOFTKERNEL_SMARTQUERYSERVERHANDLER_HH_ */
