/*
 * serializeExampleEvent2.hh
 *
 *  Created on: Feb 2, 2018
 *      Author: alexej
 */

#ifndef EXAMPLES_COMMUNICATIONOBJECTS_SERIALIZEEXAMPLEEVENT2_HH_
#define EXAMPLES_COMMUNICATIONOBJECTS_SERIALIZEEXAMPLEEVENT2_HH_

#include "commExampleEvent2.hh"

#include <ace/CDR_Stream.h>
#include <ace/SString.h>

// serialization operator for element ExampleEvent1Parameter
ACE_CDR::Boolean operator<<(ACE_OutputCDR &cdr, const SmartACE::CommExampleEvent2Parameter &obj)
{
	ACE_CDR::Boolean good_bit = true;
	// serialize list-element newState
	good_bit = good_bit && cdr.write_long(obj.parameter.lower);
	good_bit = good_bit && cdr.write_long(obj.parameter.upper);

	return good_bit;
}

// de-serialization operator for element ExampleEvent1Parameter
ACE_CDR::Boolean operator>>(ACE_InputCDR &cdr, SmartACE::CommExampleEvent2Parameter &obj)
{
	ACE_CDR::Boolean good_bit = true;
	// deserialize type element newState
	good_bit = good_bit && cdr.read_long(obj.parameter.lower);
	good_bit = good_bit && cdr.read_long(obj.parameter.upper);

	return good_bit;
}


// serialization operator for element ExampleEvent1Result
ACE_CDR::Boolean operator<<(ACE_OutputCDR &cdr, const SmartACE::CommExampleEvent2Result &obj)
{
	ACE_CDR::Boolean good_bit = true;
	// serialize list-element newState
	good_bit = good_bit && cdr << ACE_CString(obj.result.state.c_str());

	return good_bit;
}

// de-serialization operator for element ExampleEvent1Result
ACE_CDR::Boolean operator>>(ACE_InputCDR &cdr, SmartACE::CommExampleEvent2Result &obj)
{
	ACE_CDR::Boolean good_bit = true;
	// deserialize type element newState
	ACE_CString state_str;
	good_bit = good_bit && cdr >> state_str;
	obj.result.state = state_str.c_str();

	return good_bit;
}



#endif /* EXAMPLES_COMMUNICATIONOBJECTS_SERIALIZEEXAMPLEEVENT2_HH_ */
