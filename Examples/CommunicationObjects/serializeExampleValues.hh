/*
 * serializeExampleValues.hh
 *
 *  Created on: Feb 2, 2018
 *      Author: alexej
 */

#ifndef EXAMPLES_COMMUNICATIONOBJECTS_SERIALIZEEXAMPLEVALUES_HH_
#define EXAMPLES_COMMUNICATIONOBJECTS_SERIALIZEEXAMPLEVALUES_HH_

#include "commExampleValues.hh"

#include <ace/CDR_Stream.h>
#include <ace/Truncate.h>

// serialization operator for element ExampleEvent1Parameter
ACE_CDR::Boolean operator<<(ACE_OutputCDR &cdr, const SmartACE::CommExampleValues &obj)
{
	ACE_CDR::Boolean good_bit = true;
	// serialize list-element newState
	good_bit = good_bit && cdr << ACE_Utils::truncate_cast<ACE_CDR::ULong>(obj.values.size());
	good_bit = good_bit && cdr.write_long_array(obj.values.data(), obj.values.size());

	return good_bit;
}

// de-serialization operator for element ExampleEvent1Parameter
ACE_CDR::Boolean operator>>(ACE_InputCDR &cdr, SmartACE::CommExampleValues &obj)
{
	ACE_CDR::Boolean good_bit = true;
	// deserialize type element newState
	ACE_CDR::ULong dataNbr;
	good_bit = good_bit && cdr >> dataNbr;
	obj.values.resize(dataNbr);
	good_bit = good_bit && cdr.read_long_array(obj.values.data(), dataNbr);

	return good_bit;
}

#endif /* EXAMPLES_COMMUNICATIONOBJECTS_SERIALIZEEXAMPLEVALUES_HH_ */
