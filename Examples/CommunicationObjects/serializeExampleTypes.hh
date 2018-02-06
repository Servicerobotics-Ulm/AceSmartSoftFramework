/*
 * serializeExampleTypes.hh
 *
 *  Created on: Feb 2, 2018
 *      Author: alexej
 */

#ifndef EXAMPLES_COMMUNICATIONOBJECTS_SERIALIZEEXAMPLETYPES_HH_
#define EXAMPLES_COMMUNICATIONOBJECTS_SERIALIZEEXAMPLETYPES_HH_

#include "commExampleTypes.hh"

#include <ace/CDR_Stream.h>

// serialization operator for element ExampleEvent1Parameter
ACE_CDR::Boolean operator<<(ACE_OutputCDR &cdr, const SmartACE::CommExampleTypes &obj)
{
	ACE_CDR::Boolean good_bit = true;
	// serialize list-element newState
	good_bit = good_bit && cdr.write_boolean(obj.data.cdr_bool);
	good_bit = good_bit && cdr.write_char(obj.data.cdr_char);
	good_bit = good_bit && cdr.write_double(obj.data.cdr_dbl);
	good_bit = good_bit && cdr.write_float(obj.data.cdr_float);
	good_bit = good_bit && cdr.write_long(obj.data.cdr_long);
	good_bit = good_bit && cdr.write_longlong(obj.data.cdr_longlong);
	good_bit = good_bit && cdr.write_ulong(obj.data.cdr_ulong);
	good_bit = good_bit && cdr.write_ulonglong(obj.data.cdr_ulonglong);
	good_bit = good_bit && cdr.write_ushort(obj.data.cdr_ushort);

	return good_bit;
}

// de-serialization operator for element ExampleEvent1Parameter
ACE_CDR::Boolean operator>>(ACE_InputCDR &cdr, SmartACE::CommExampleTypes &obj)
{
	ACE_CDR::Boolean good_bit = true;
	// deserialize type element newState
	good_bit = good_bit && cdr.read_boolean(obj.data.cdr_bool);
	good_bit = good_bit && cdr.read_char(obj.data.cdr_char);
	good_bit = good_bit && cdr.read_double(obj.data.cdr_dbl);
	good_bit = good_bit && cdr.read_float(obj.data.cdr_float);
	good_bit = good_bit && cdr.read_long(obj.data.cdr_long);
	good_bit = good_bit && cdr.read_longlong(obj.data.cdr_longlong);
	good_bit = good_bit && cdr.read_ulong(obj.data.cdr_ulong);
	good_bit = good_bit && cdr.read_ulonglong(obj.data.cdr_ulonglong);
	good_bit = good_bit && cdr.read_ushort(obj.data.cdr_ushort);

	return good_bit;
}

#endif /* EXAMPLES_COMMUNICATIONOBJECTS_SERIALIZEEXAMPLETYPES_HH_ */
