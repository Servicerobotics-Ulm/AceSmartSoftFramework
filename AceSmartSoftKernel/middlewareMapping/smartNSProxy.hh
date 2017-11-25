// --------------------------------------------------------------------------
//
//  Copyright (C) 2012-2014 Alex Lotz
//
//        lotz@hs-ulm.de
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


#ifndef SMARTNSPROXY_H_
#define SMARTNSPROXY_H_

#include <ace/Name_Space.h>
#include <ace/Name_Request_Reply.h>

#include "smartServiceHandler.hh"

namespace SmartACE {

// NSProxy implements the ACE_Name_Space interface and is a ServiceHandler object
// which can be connected to the remote name-space
class NSProxy : public ACE_Name_Space, public SmartACE::ServiceHandler
{
protected:
	enum REPLY_ID { REPLY_CONTINUE=1, REPLY_OK=0, REPLY_ERROR=-1, REPLY_ALREADY_BOUND=-2 };

	ACE_CString incoming_name;
	ACE_CString incoming_value;
	ACE_CString incoming_type;
	virtual int handle_incomming_message(ACE_CDR::Long command, ACE_InputCDR &cmd_is, ACE_InputCDR &msg_is);

	// internal helper methods
	int list_entries(ACE_BINDING_SET &set_out, 
		const ACE_NS_WString &pattern_in,
		const ACE_Name_Request::Constants &command_in);
	int lists(ACE_WSTRING_SET &set_out,
		const ACE_NS_WString &pattern_in,
		const ACE_Name_Request::Constants &command_in);
public:
	NSProxy();
	virtual ~NSProxy();

	// overload open method from SmartACE::ServiceHandler in order to deactivate reactor read mask
	virtual int open (void *handler = 0);

	/// Bind a new name to a naming context (Wide character strings).
	virtual int bind (const ACE_NS_WString &name_in,
					const ACE_NS_WString &value_in,
					const char *type_in = "");


	/**
	 * Overwrite the value or type of an existing name in a
	 * ACE_Name_Space or bind a new name to the context, if it didn't
	 * exist yet. (Wide charcter strings interface).
	 */
	virtual int rebind (const ACE_NS_WString &name_in,
					  const ACE_NS_WString &value_in,
					  const char *type_in = "");

	/// Delete a name from a ACE_Name_Space (Wide charcter strings
	/// Interface).
	virtual int unbind (const ACE_NS_WString &name_in);

	/// Get value and type of a given name binding (Wide chars).  The
	/// caller is responsible for deleting both <value_out> and <type_out>!
	virtual int resolve (const ACE_NS_WString &name_in,
					   ACE_NS_WString &value_out,
					   char *&type_out);

	/// Get a set of names matching a specified pattern (wchars). Matching
	/// means the names must begin with the pattern string.
	virtual int list_names (ACE_WSTRING_SET &set_out,
						  const ACE_NS_WString &pattern_in);

	/// Get a set of values matching a specified pattern (wchars). Matching
	/// means the values must begin with the pattern string.
	virtual int list_values (ACE_WSTRING_SET &set_out,
						   const ACE_NS_WString &pattern_in);

	/// Get a set of types matching a specified pattern (wchars). Matching
	/// means the types must begin with the pattern string.
	virtual int list_types (ACE_WSTRING_SET &set_out,
						  const ACE_NS_WString &pattern_in);

	/**
	 * Get a set of names matching a specified pattern (wchars). Matching
	 * means the names must begin with the pattern string. Returns the
	 * complete binding associated each pattern match.
	 */
	virtual int list_name_entries (ACE_BINDING_SET &set,
								 const ACE_NS_WString &pattern);

	/**
	 * Get a set of values matching a specified pattern (wchars). Matching
	 * means the values must begin with the pattern string. Returns the
	 * complete binding associated each pattern match.
	 */
	virtual int list_value_entries (ACE_BINDING_SET &set,
								  const ACE_NS_WString &pattern);

	/**
	 * Get a set of types matching a specified pattern (wchars). Matching
	 * means the types must begin with the pattern string. Returns the
	 * complete binding associated each pattern match.
	 */
	virtual int list_type_entries (ACE_BINDING_SET &set,
								 const ACE_NS_WString &pattern);

	virtual void dump (void) const;
};

} /* namespace Smart */
#endif /* SMARTNSPROXY_H_ */
