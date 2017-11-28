//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.12
// The SmartSoft Toolchain has been developed by:
//
// ZAFH Servicerobotic Ulm
// Christian Schlegel (schlegel@hs-ulm.de)
// Alex Lotz (lotz@hs-ulm.de)
// University of Applied Sciences
// Prittwitzstr. 10
// 89075 Ulm (Germany)
//
// Information about the SmartSoft MDSD Toolchain is available at:
// smart-robotics.sourceforge.net
//
// This file is generated once. Modify this file to your needs.
// If you want the toolchain to re-generate this file, please
// delete it before running the code generator.
//--------------------------------------------------------------------------

// --------------------------------------------------------------------------
//
//  Copyright (C) 2014 Alex Lotz
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



#include <sstream>

#include "smartCommParameterRequest.hh"

using namespace SmartACE;

int CommParameterRequest::getString(const std::string &key, std::string &value) const
{
	return get(key,value);
}

std::string CommParameterRequest::getString(const std::string &key) const
{
	std::string result;
	this->get(key,result);
	return result;
}

int CommParameterRequest::getStringList(const std::string &key, std::list<std::string> &value) const
{
	return getList(key,value);
}

std::list<std::string> CommParameterRequest::getStringList(const std::string &key) const
{
	std::list<std::string> result;
	this->getList(key,result);
	return result;
}

// getTag returns the string value for the top level tag
std::string CommParameterRequest::getTag() const
{
	return getString("slot");
}

// setTag writes the string value for the top level tag
void CommParameterRequest::setTag(const std::string &value)
{
	setString("slot", value);
}

void CommParameterRequest::get(ACE_Message_Block *&msg) const
{
	ACE_OutputCDR cdr(ACE_DEFAULT_CDR_BUFSIZE);

	cdr << static_cast<ACE_CDR::Long> (idl_CommParameterRequest.items.size());
	std::vector<CommParameterIDL::NameValue>::const_iterator
			idl_CommParameterRequest_itemsIt;
	for (idl_CommParameterRequest_itemsIt
			= idl_CommParameterRequest.items.begin(); idl_CommParameterRequest_itemsIt
			!= idl_CommParameterRequest.items.end(); idl_CommParameterRequest_itemsIt++)
	{

		cdr << (*idl_CommParameterRequest_itemsIt).key;
		cdr << static_cast<ACE_CDR::Long> ((*idl_CommParameterRequest_itemsIt).values.size());
		for(unsigned i=0; i<(*idl_CommParameterRequest_itemsIt).values.size(); ++i) {
			cdr << (*idl_CommParameterRequest_itemsIt).values[i];
		}
	}

	msg = cdr.begin()->duplicate();
}

void CommParameterRequest::set(const ACE_Message_Block *msg)
{
	ACE_InputCDR cdr(msg);

	ACE_CDR::Long idl_CommParameterRequest_itemsNbr;
	cdr >> idl_CommParameterRequest_itemsNbr;
	idl_CommParameterRequest.items.clear();
	CommParameterIDL::NameValue idl_CommParameterRequest_itemsItem;
	for (ACE_CDR::Long i = 0; i < idl_CommParameterRequest_itemsNbr; ++i)
	{
		cdr >> idl_CommParameterRequest_itemsItem.key;

		ACE_CDR::Long idl_CommParameterRequest_items_valuesNbr;
		cdr >> idl_CommParameterRequest_items_valuesNbr;
		idl_CommParameterRequest_itemsItem.values.clear();
		for(ACE_CDR::Long i2 = 0; i2 < idl_CommParameterRequest_items_valuesNbr; ++i2)
		{
			ACE_CString idl_CommParameterRequest_itemsItem_valueItem;
			cdr >> idl_CommParameterRequest_itemsItem_valueItem;
			idl_CommParameterRequest_itemsItem.values.push_back(idl_CommParameterRequest_itemsItem_valueItem);
		}

		idl_CommParameterRequest.items.push_back(
				idl_CommParameterRequest_itemsItem);
	}
}

int CommParameterRequest::remove(const std::string &key) {
	bool found = false;
	std::vector<CommParameterIDL::NameValue>::iterator it;
	for(it=idl_CommParameterRequest.items.begin(); it!=idl_CommParameterRequest.items.end(); it++) {
		if(it->key.c_str() == key.c_str()) {
			found = true;
			break;
		}
	}
	idl_CommParameterRequest.items.erase(it);
	return found? 0 : -1;
}

void CommParameterRequest::to_ostream(std::ostream &os) const
{
  os << "CommParameterRequest(";

  std::vector<CommParameterIDL::NameValue>::const_iterator
		idl_CommParameterRequest_itemsIt;
  for (idl_CommParameterRequest_itemsIt
		= idl_CommParameterRequest.items.begin(); idl_CommParameterRequest_itemsIt
		!= idl_CommParameterRequest.items.end(); idl_CommParameterRequest_itemsIt++)
  {

	os << "([" << (*idl_CommParameterRequest_itemsIt).key << "] ";
	for(unsigned i=0; i<idl_CommParameterRequest_itemsIt->values.size(); ++i) {
		os << idl_CommParameterRequest_itemsIt->values[i] << " ";
	}
	os << ")";
  }
  os << ")";
}

