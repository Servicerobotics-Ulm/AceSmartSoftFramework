/*
 * serializeExamplePrint.hh
 *
 *  Created on: Feb 5, 2018
 *      Author: alexej
 */

#ifndef EXAMPLES_COMMUNICATIONOBJECTS_SERIALIZEEXAMPLEPRINT_HH_
#define EXAMPLES_COMMUNICATIONOBJECTS_SERIALIZEEXAMPLEPRINT_HH_


#include "commExamplePrint.hh"

#include <ace/CDR_Stream.h>

// serialization operator for element ExampleEvent1Parameter
ACE_CDR::Boolean operator<<(ACE_OutputCDR &cdr, const SmartACE::CommExamplePrint &obj)
{
	ACE_CDR::Boolean good_bit = true;
	// serialize list-element newState
	good_bit = good_bit && cdr.write_short(obj.message.hour);
	good_bit = good_bit && cdr.write_short(obj.message.minute);
	good_bit = good_bit && cdr.write_short(obj.message.second);
	good_bit = good_bit && cdr << ACE_CString(obj.message.text.c_str());

	return good_bit;
}

// de-serialization operator for element ExampleEvent1Parameter
ACE_CDR::Boolean operator>>(ACE_InputCDR &cdr, SmartACE::CommExamplePrint &obj)
{
	ACE_CDR::Boolean good_bit = true;
	// deserialize type element newState
	good_bit = good_bit && cdr.read_short(obj.message.hour);
	good_bit = good_bit && cdr.read_short(obj.message.minute);
	good_bit = good_bit && cdr.read_short(obj.message.second);
	ACE_CString text;
	good_bit = good_bit && cdr >> text;
	obj.message.text = text.c_str();

	return good_bit;
}


#endif /* EXAMPLES_COMMUNICATIONOBJECTS_SERIALIZEEXAMPLEPRINT_HH_ */
