/*
 * serializeExampleEvent1.hh
 *
 *  Created on: Feb 2, 2018
 *      Author: alexej
 */

#ifndef EXAMPLES_COMMUNICATIONOBJECTS_SERIALIZEEXAMPLEEVENT1_HH_
#define EXAMPLES_COMMUNICATIONOBJECTS_SERIALIZEEXAMPLEEVENT1_HH_

#include "commExampleEvent1.hh"

#include <ace/CDR_Stream.h>

// serialization operator for element ExampleEvent1Parameter
ACE_CDR::Boolean operator<<(ACE_OutputCDR &cdr, const SmartACE::CommExampleEvent1Parameter &obj)
{
	ACE_CDR::Boolean good_bit = true;
	// serialize list-element newState
	good_bit = good_bit && cdr.write_long(obj.parameter.value);

	return good_bit;
}

// de-serialization operator for element ExampleEvent1Parameter
ACE_CDR::Boolean operator>>(ACE_InputCDR &cdr, SmartACE::CommExampleEvent1Parameter &obj)
{
	ACE_CDR::Boolean good_bit = true;
	// deserialize type element newState
	good_bit = good_bit && cdr.read_long(obj.parameter.value);

	return good_bit;
}


// serialization operator for element ExampleEvent1Result
ACE_CDR::Boolean operator<<(ACE_OutputCDR &cdr, const SmartACE::CommExampleEvent1Result &obj)
{
	ACE_CDR::Boolean good_bit = true;
	// serialize list-element newState
	good_bit = good_bit && cdr.write_long(obj.result.value);

	return good_bit;
}

// de-serialization operator for element ExampleEvent1Result
ACE_CDR::Boolean operator>>(ACE_InputCDR &cdr, SmartACE::CommExampleEvent1Result &obj)
{
	ACE_CDR::Boolean good_bit = true;
	// deserialize type element newState
	good_bit = good_bit && cdr.read_long(obj.result.value);

	return good_bit;
}

#endif /* EXAMPLES_COMMUNICATIONOBJECTS_SERIALIZEEXAMPLEEVENT1_HH_ */
