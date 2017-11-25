// --------------------------------------------------------------------------
//
//  Copyright (C) 2008/2012/2013 Alex Lotz
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

#ifndef SMARTSERVICEHANDLER_HH_
#define SMARTSERVICEHANDLER_HH_

#include <ace/Svc_Handler.h>
#include <ace/CDR_Stream.h>
#include <ace/SOCK_Stream.h>

// include acceptor/connector classes
#include <ace/Connector.h>
#include <ace/SOCK_Connector.h>
#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>

namespace SmartACE {

/** @internal
 * ACE ServiceHandler Abstract base class
 *
 * This class implements a bidirectional communication mechanism that allows to communicate
 * between two end-points in a network (using in this case the SOCK_STREAM). In SmartSoft this class
 * is used in all Server communication ports together with the ACE_Acceptor class and in all Client
 * communication ports together with the ACE_Connector class.
 *
 * Since 2013, this class additionally implements a more robust error handling mechanism, which directly
 * deals with the broken pipe error inside of the communication methods. It is important to know that
 * therefore, on Unix systems the broken pipe error should be deactivated (redirected to a no-op handler),
 * because the default handling results in closing down the whole process (which is not nice the smartsoft case).
 *
 * Also an adjusted clean-up procedure is implemented which decouples the destruction
 * called from an external thread to be executed by the reactor itself. This is needed since otherwise a thread
 * could destroy this class while the reactor still uses it, which in some cases leads to unexpected behavior.
 */
class ServiceHandler: public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> {
private:
	/** This function provides synchronous message send functionality.
	 *  It implements reliable send policy.
	 *  It returns 0 on success or -1 on communication error.
	 */
	int synchron_message_send(ACE_CDR::Long command,
			const ACE_Message_Block *further_comands,
			const ACE_Message_Block *message);
protected:
	/** This internal variable is used to distinguish whether a connection is closed
	 *  actively by an external thread (using the method trigger_shutdown() ) or the
	 *  connection is closed because the underlying socket is broken.
	 */
	bool connectionClosedByUser;

	/** This internal variable indicates that a connection was closed by reactor
	 *  e.g. due to broken pipe or if the remote end-point calls shutdown
	 *  (this variable prevents the service handler to run into broken pipe error
	 *  in regular cases, however, as broken pipe is also handled inside of the receive/send
	 *  methods, this functionality is not critical, it just improves the overall performance)
	 */
	bool connectionResetByReactor;

	/** Definition for parent class. This allows to use functions from parent class
	 *  in a more readable way.
	 */
	typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> super;

	/** This handler should be implemented in derived classes.
	 *  It should implement the dispatching of the incoming requests to the
	 *  corresponding call-back methods (from the communication pattern).
	 */
	virtual int handle_incomming_message(ACE_CDR::Long command,
			ACE_InputCDR &cmd_is, ACE_InputCDR &msg_is) = 0;

	/** This callback handler is indirectly triggered (using the reactor's notify mechanism)
	 *  from the method trigger_shutdown() which can be used by external threads.
	 */
	virtual int handle_exception (ACE_HANDLE fd = ACE_INVALID_HANDLE);

	/** This class is overloaded from base class to change visibility
	 *  (in order to prevent the destruction from external threads).
	 */
	virtual void destroy (void);

public:
	/** Default constructor and destructor
	 */
	ServiceHandler(ACE_Reactor *reactor = ACE_Reactor::instance());
	virtual ~ServiceHandler();

	/** Initialization to create and activate an instance of this ACE_Svc_Handler class.
	 *  Here, memory allocation and ACE_Svc_Handler parameterization takes place.
	 *  The default implementation just calls the same method from parent class.
	 *
	 * @param handler	A pointer to the parent controlling class (in this case ACE_Connector or ACE_Acceptor)
	 *
	 * @return error-code
	 * 		- A "-1" determines an error
	 * 		- A "0" is returned if all goes right
	 */
	virtual int open(void *handler = 0);

	/** This is the main input-handler function that is called by the reactor to handle incoming data streams.
	 *  Within this function the de-marshaling takes place.
	 */
	virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);

	/** Hook handler method to handle closing connection.
	 *  This default behavior is changed here. This method should not destroy the object itself
	 *  (as implemented by default). Instead, the destruction is explicitly triggered by the
	 *  user method triggerShutdown().
	 */
	virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

	/** This method encapsulates the message sending behavior.
	 *  Within this method the marshaling takes place. It is safe to call this method
	 *  from a user thread, however, this method is not thread safe and can not be called
	 *  by several threads in parallel.
	 *  It returns 0 on success or -1 on communication error.
	 */
	virtual int send_command_message(ACE_CDR::Long commandId,
			const ACE_Message_Block *further_comands = NULL,
			const ACE_Message_Block *message = NULL);

	/** Trigger the shutdown procedure to be executed by the reactor.
	 *  The shutdown procedure should not be triggered by external threads directly.
	 *  Instead the reactor should be notified to shutdown this service handler as soon as possible.
	 *  This prevents the error, which can occur if a thread removes the service handler while
	 *  it is still used by the reactor for still pending events.
	 */
	void trigger_shutdown();

	/** This method returns true if reactor has closed the connection (due to e.g. a broken pipe or
	 *  remotely called shutdown)
	 */
	inline bool is_disconnected() const { return connectionResetByReactor; }
};

} // END namespace Smart

#endif /*SMARTSERVICEHANDLER_HH_*/
