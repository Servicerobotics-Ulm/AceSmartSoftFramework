/*
 * serializeExampleTime.hh
 *
 *  Created on: Feb 2, 2018
 *      Author: alexej
 */

#ifndef EXAMPLES_COMMUNICATIONOBJECTS_SERIALIZEEXAMPLETIME_HH_
#define EXAMPLES_COMMUNICATIONOBJECTS_SERIALIZEEXAMPLETIME_HH_

#include "commExampleTime.hh"

#include <ace/CDR_Stream.h>

// serialization operator for element ExampleEvent1Parameter
ACE_CDR::Boolean operator<<(ACE_OutputCDR &cdr, const SmartACE::CommExampleTime &obj)
{
	ACE_CDR::Boolean good_bit = true;
	// serialize list-element newState
	good_bit = good_bit && cdr.write_longlong(obj.time.nanoseconds);

	return good_bit;
}

// de-serialization operator for element ExampleEvent1Parameter
ACE_CDR::Boolean operator>>(ACE_InputCDR &cdr, SmartACE::CommExampleTime &obj)
{
	ACE_CDR::Boolean good_bit = true;
	// deserialize type element newState
	good_bit = good_bit && cdr.read_longlong(obj.time.nanoseconds);

	return good_bit;
}

#endif /* EXAMPLES_COMMUNICATIONOBJECTS_SERIALIZEEXAMPLETIME_HH_ */
