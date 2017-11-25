// --------------------------------------------------------------------------
//
//  Copyright (C) 2008/2012 Alex Lotz
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

#include "smartServiceHandler.hh"

// using C++11 unique_ptr
#include <memory>

namespace SmartACE {

namespace SERVICE {
	enum {
		// Header consists of following content:
		//
		// | ByteOrder | CMD-ID | Param-length | MSG-length |
		//      4B         4B         4B            4B

		DEFAULT_HADER_LENGTH = 16
	};
}

ServiceHandler::ServiceHandler(ACE_Reactor *reactor)
:	super(0, 0, reactor)
,	connectionClosedByUser(false)
,	connectionResetByReactor(false)
{
	this->reactor()->register_handler(this, ACE_Event_Handler::EXCEPT_MASK);
}

ServiceHandler::~ServiceHandler()
{
	// all dynamic memory objects are handled by parent class
	// so no memory clean up is necessary here
}

int ServiceHandler::open(void *handler)
{
	//ACE_DEBUG((LM_DEBUG, ACE_TEXT("SmartServiceHandler::open(void *handler)\n") ));
	return super::open(handler);

	// DEBUGGING information
	/*
	 ACE_TCHAR peer_name[MAXHOSTNAMELEN];
	 ACE_INET_Addr peer_addr;

	 //following if statement prints debugging infos for successful connection
	 if (this->peer().get_remote_addr(peer_addr) == 0 &&
	 peer_addr.addr_to_string (peer_name, MAXHOSTNAMELEN) == 0)
	 ACE_DEBUG ((LM_DEBUG,
	 ACE_TEXT ("(%P|%t) Connected with %s\n"),
	 peer_name));
	 */
}

void ServiceHandler::destroy()
{
	// just call the method of the parent class
	super::destroy();
}

void ServiceHandler::trigger_shutdown()
{
	connectionClosedByUser = true;
	this->reactor()->notify(this, ACE_Event_Handler::EXCEPT_MASK);
}

int ServiceHandler::handle_exception(ACE_HANDLE fd)
{
	if (connectionClosedByUser) {
		// the service handler is not used by external classes any more,
		// hence, it is now safe to call destroy() which
		// internally calls shutdown() and then calls "delete this" in case
		// this class was created on the heap
		// ACE_DEBUG((LM_DEBUG, ACE_TEXT("ServiceHandler::handle_exception()\n")));
		this->destroy();
	}
	return 0;
}

int ServiceHandler::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
	// ACE_DEBUG((LM_DEBUG, ACE_TEXT("ServiceHandler::handle_close()\n")));

	connectionResetByReactor = true;

	// do NOT call destroy() here, since a pointer to this class could still be used in
	// one of the communication patterns and would other cause a segmentation fault.
	this->shutdown();
	return 0;
}

int ServiceHandler::handle_input(ACE_HANDLE)
{
	//ACE_DEBUG((LM_DEBUG, ACE_TEXT("SmartServiceHandler->handle_input()\n")));

	// local variables and objects

	// header-message contains byte-order, payload-length and command
	ACE_Message_Block *header = 0;

	// payload will be initilized on the heap
	// containing user data
	ACE_Message_Block *further_commands = NULL;
	ACE_Message_Block *message_content = NULL;

	ACE_InputCDR commands_cdr((size_t) 0);
	ACE_InputCDR message_cdr((size_t) 0);

	// variables to store the remote byteorder, payload size and command-id
	ACE_CDR::Boolean byte_order;
	ACE_CDR::Long command_id;
	ACE_CDR::ULong command_length;
	ACE_CDR::ULong message_length;

	// temp variable storing amount of received bytes
	size_t recvd_bytes = 0;
	size_t total_received = 0;

	// timeout to wait on messages to receive
	ACE_Time_Value timeout(1);

	//create new MessageBlock for header data
	ACE_NEW_RETURN(header,
			ACE_Message_Block (ACE_CDR::MAX_ALIGNMENT + SERVICE::DEFAULT_HADER_LENGTH),
			-1);

	//set pointer under std::unique_ptr control
	std::unique_ptr<ACE_Message_Block> header_ap(header);

	// Align the Message Block for a CDR stream
	ACE_CDR::mb_align(header_ap.get());

	// receive header data from peer-stream
	if (this->peer().recv_n(header_ap->wr_ptr(), SERVICE::DEFAULT_HADER_LENGTH, &timeout, &recvd_bytes) == -1) {
		if (ACE_OS::last_error() == ETIME) {
			ACE_ERROR_RETURN(
					(LM_ERROR, ACE_TEXT ("(%P|%t) %p\n"), ACE_TEXT ("recv_n(header): ETIME")),
					-1);
		} else if(ACE_OS::last_error() == EPIPE) {
			connectionResetByReactor = true;
			ACE_ERROR_RETURN(
					(LM_ERROR, ACE_TEXT ("(%P|%t) %p\n"), ACE_TEXT ("recv_n(header): EPIPE")),
					-1);
		} else if(ACE_OS::last_error() == ECONNRESET) {
			connectionResetByReactor = true;
			ACE_ERROR_RETURN(
					(LM_ERROR, ACE_TEXT ("(%P|%t) %p\n"), ACE_TEXT ("recv_n(header): ECONNRESET")),
					-1);
		} else {
			ACE_ERROR_RETURN(
					(LM_ERROR, ACE_TEXT ("(%P|%t) %p\n"), ACE_TEXT ("recv_n(header)")),
					-1);
		}
	}

	// check whether complete header size has been received
	if (recvd_bytes == 0) {
		// this is the default case when closing the socket connection.
		// By returning -1, the reactor is informed to skip the currently pending
		// incoming messages in order to be able to close the connection.
		// No error message is printed, since this is a regular case for
		// disconnecting a communication port.
		//ACE_DEBUG(( LM_DEBUG, ACE_TEXT("ServiceHandler: received 0 bytes.\n") ));
		connectionResetByReactor = true;
		return -1;
	} else if (recvd_bytes < SERVICE::DEFAULT_HADER_LENGTH) {
		ACE_ERROR_RETURN(
				(LM_INFO, ACE_TEXT ("handling invalid message header\n")), -1);
	}

	header_ap->wr_ptr(SERVICE::DEFAULT_HADER_LENGTH);
	total_received = recvd_bytes;

	// Create a CDR stream to parse the header.
	ACE_InputCDR header_cdr(header_ap.get());

	// Extract the byte-order and use helper methods to disambiguate
	// octet, booleans, and chars.
	header_cdr >> ACE_InputCDR::to_boolean(byte_order);
	header_cdr.reset_byte_order(byte_order);

	// Extract the length and command_id
	header_cdr >> command_id;
	header_cdr >> command_length;
	header_cdr >> message_length;

	// ACE_DEBUG((LM_DEBUG, ACE_TEXT("ServiceHandler->recv_header->command_length: %d\n"), command_length ));

	// optionally receive further commands
	if (command_length > 0) {
		//create new MessageBlock with received message size and set the pointer under std::unique_ptr control
		ACE_NEW_RETURN(further_commands,
				ACE_Message_Block (ACE_CDR::MAX_ALIGNMENT + command_length),
				-1);

		// set payload pointer under std::unique_ptr control to get rid of memory leaks
		std::unique_ptr<ACE_Message_Block> further_commands_ap(further_commands);

		ACE_CDR::mb_align(further_commands_ap.get());

		// Use <recv_n> to obtain the contents.
		if (this->peer().recv_n(further_commands_ap->wr_ptr(), command_length,
				&timeout, &recvd_bytes) == -1) {
			if (ACE_OS::last_error() == ETIME) {
				ACE_ERROR_RETURN(
						(LM_ERROR, ACE_TEXT ("(%P|%t) %p\n"), ACE_TEXT ("recv_n(further_commands): ETIME")),
						-1);
			} else if(ACE_OS::last_error() == EPIPE) {
				connectionResetByReactor = true;
				ACE_ERROR_RETURN(
						(LM_ERROR, ACE_TEXT ("(%P|%t) %p\n"), ACE_TEXT ("recv_n(further_commands): EPIPE")),
						-1);
			} else if(ACE_OS::last_error() == ECONNRESET) {
				connectionResetByReactor = true;
				ACE_ERROR_RETURN(
						(LM_ERROR, ACE_TEXT ("(%P|%t) %p\n"), ACE_TEXT ("recv_n(further_commands): ECONNRESET")),
						-1);
			} else {
				ACE_ERROR_RETURN(
					(LM_ERROR, ACE_TEXT ("%p\n"), ACE_TEXT ("recv_n(further_commands)")),
					-1);
			}
		}

		further_commands_ap->wr_ptr(recvd_bytes);   // Reflect additional bytes
		total_received += recvd_bytes;

		commands_cdr.reset(further_commands_ap.get(), byte_order);
	}

	// optionally receive message content
	if (message_length > 0) {
		//create new MessageBlock with received message size and set the pointer under std::unique_ptr control
		ACE_NEW_RETURN(message_content,
				ACE_Message_Block (ACE_CDR::MAX_ALIGNMENT + message_length),
				-1);

		// set payload pointer under std::unique_ptr control to get rid of memory leaks
		std::unique_ptr<ACE_Message_Block> message_content_ap(message_content);

		ACE_CDR::mb_align(message_content_ap.get());

		// Use <recv_n> to obtain the contents.
		if (this->peer().recv_n(message_content_ap->wr_ptr(), message_length,
				&timeout, &recvd_bytes) == -1) {
			if (ACE_OS::last_error() == ETIME) {
				ACE_ERROR_RETURN(
						(LM_ERROR, ACE_TEXT ("(%P|%t) %p\n"), ACE_TEXT ("recv_n(message_content): ETIME")),
						-1);
			} else if(ACE_OS::last_error() == EPIPE) {
				connectionResetByReactor = true;
				ACE_ERROR_RETURN(
						(LM_ERROR, ACE_TEXT ("(%P|%t) %p\n"), ACE_TEXT ("recv_n(message_content): EPIPE")),
						-1);
			} else if(ACE_OS::last_error() == ECONNRESET) {
				connectionResetByReactor = true;
				ACE_ERROR_RETURN(
						(LM_ERROR, ACE_TEXT ("(%P|%t) %p\n"), ACE_TEXT ("recv_n(message_content): ECONNRESET")),
						-1);
			} else {
				ACE_ERROR_RETURN(
					(LM_ERROR, ACE_TEXT ("%p\n"), ACE_TEXT ("recv_n(message_content)")),
					-1);
			}
		}

		message_content_ap->wr_ptr(recvd_bytes);   // Reflect additional bytes
		total_received += recvd_bytes;

		message_cdr.reset(message_content_ap.get(), byte_order);
	}

	/****************************************************************************
	 * important step here:
	 * Incoming data or command message will be delegated to a derived class for further handling.
	 * This function should not block for a longer period of time otherwise this will
	 * block the following requests!
	 */
	if (total_received
			== (SERVICE::DEFAULT_HADER_LENGTH + command_length + message_length)) {
		// hand over the payload data to incomming_message callback method
		return this->handle_incomming_message(command_id, commands_cdr,
				message_cdr);
	}

	// we should not get there if everything worked fine
	// note: memory of header and payload objects will be freed automatically by std::unique_ptr
	return -1;
}

int ServiceHandler::synchron_message_send(ACE_CDR::Long command,
		const ACE_Message_Block *further_comands,
		const ACE_Message_Block *message)
{
	//ACE_DEBUG((LM_DEBUG, ACE_TEXT("ServiceHandler: handle_output()\n")));

	// Message vector
	size_t sended_message_bytes = 0;
	ACE_Time_Value timeout(1);
	ACE_CDR::ULong command_length = 0;
	ACE_CDR::ULong message_length = 0;

	if (further_comands != NULL) {
		command_length = ACE_Utils::truncate_cast<ACE_CDR::ULong>(
				further_comands->length());
	}

	if (message != NULL) {
		message_length = ACE_Utils::truncate_cast<ACE_CDR::ULong>(
				message->total_length());
	}

	// initialize the header stream
	ACE_OutputCDR header(
			ACE_CDR::MAX_ALIGNMENT + SERVICE::DEFAULT_HADER_LENGTH);

	// assemble the message header: <byteorder><command_id><command_length><payload_length>
	header << ACE_OutputCDR::from_boolean(ACE_CDR_BYTE_ORDER);
	header << command;
	header << command_length;
	header << message_length;

	// initiate the total_message with a duplicate of the header ACE_Message_Block
	ACE_Message_Block *total_message = header.begin()->duplicate();
	// be aware that duplicate increases the reference counter of the internal
	// smart-pointer. In order to prevent memory leaks, the total_message
	// needs to call release after it has been sent over the socket

	// next all the messages (header, commands and payload) are concatenated ising
	// the ACE_Message_Block linked list functionality
	// the order of messages is as follows:
	// header.cont() [-> cmds.cont()] [-> message.cont()...]
	// header is the fix content that is always sent
	// cmds are optional (depending on the current message type)
	// in case no cmds are available, the message(s) are directly linked to the header
	// the message(s) are optional as well (depending on current message type)
	// be aware that the message object internally might be a linked list of message chunks
	// so do NOT use iovec with rd_ptr, this would ignore the other message chunks
	if (further_comands != NULL) {
		// a duplicate is required to get a shallow copy that can be linked to the header
		// (no data is actually duplicated, only the internal reference counter is incremented)
		ACE_Message_Block *cmds = further_comands->duplicate();
		if(message != NULL) {
			ACE_Message_Block *msg = message->duplicate();
			// link together: cmds -> msgs
			cmds->cont(msg);
		}
		// link together: header -> cmds [-> msgs]
		total_message->cont(cmds);
	} else {
		if(message != NULL) {
			ACE_Message_Block *msg = message->duplicate();
			// no cmds available, so link together: header -> msgs
			total_message->cont(msg);
		}
	}


	if(this->peer().send_n(total_message, &timeout, &sended_message_bytes) == -1)
	{
		// an error occurred, so before exiting the method, the message content needs to be released
		// this release is recursive, so all concatenated messages (such as the cmds and msgs)
		// will also execute their own release method!!!
		total_message->release();

		// check the error code and print according error message
		if (ACE_OS::last_error() == ETIME) {
			ACE_ERROR_RETURN(
					(LM_ERROR, ACE_TEXT ("(%P|%t) %p\n"), ACE_TEXT ("send_n(total_message): ETIME")),
					-1);
		} else if(ACE_OS::last_error() == EPIPE) {
			connectionResetByReactor = true;
			ACE_ERROR_RETURN(
					(LM_ERROR, ACE_TEXT ("(%P|%t) %p\n"), ACE_TEXT ("send_n(total_message): EPIPE")),
					-1);
		} else if(ACE_OS::last_error() == ECONNRESET) {
			connectionResetByReactor = true;
			ACE_ERROR_RETURN(
					(LM_ERROR, ACE_TEXT ("(%P|%t) %p\n"), ACE_TEXT ("send_n(total_message): ECONNRESET")),
					-1);
		} else {
			ACE_ERROR_RETURN(
				(LM_ERROR, ACE_TEXT ("(%P|%t) %p\n"), ACE_TEXT ("send_n(total_message)")),
				-1);
		}
	}

	// NEVER forget to release the message, otherwise memory leaks will occur
	// this release is recursive, so all concatenated messages (such as the cmds and msgs)
	// will also execute their own release method!!!
	total_message->release();

	// return 0 in case the full message content could be transferred or -1 otherwise
	return ( sended_message_bytes == (command_length + message_length + SERVICE::DEFAULT_HADER_LENGTH) ?
			0 : -1);
}

int ServiceHandler::send_command_message(ACE_CDR::Long commandId,
		const ACE_Message_Block *further_comands,
		const ACE_Message_Block *message)
{
	// do not try to send any further messages since meanwhile the connection was closed by the
	// reactor due to a broken pipe or due to remotely called shutdown
	if(connectionResetByReactor) return -1;

	//
	// Send MessageBlock
	//
	return synchron_message_send(commandId, further_comands, message);
}

} // END namespace Smart
