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


#ifndef _COMM_COMMPARAMETER_COMMPARAMETERREQUEST_HH
#define _COMM_COMMPARAMETER_COMMPARAMETERREQUEST_HH

#include <string>
#include <list>
#include <iostream>
#include <sstream>

// include core class
#include "smartCommParameterRequestIdl.hh"

#include <ace/CDR_Stream.h>

namespace SmartACE
{

class CommParameterRequest
{
public:
	CommParameterIDL::CommParameterRequest data;

	CommParameterRequest()
	{
	}

	CommParameterRequest(const CommParameterIDL::CommParameterRequest &obj) :
		data(obj)
	{
	}

	virtual ~CommParameterRequest()
	{
	}

	operator CommParameterIDL::CommParameterRequest() const
	{
		return data;
	}

	static inline std::string identifier(void)
	{
		return "CommParameter::CommParameterRequest";
	}

	//
	// add your customized interface here
	//
	int remove(const std::string &key);

	// getTag returns the string value for the top level tag
	std::string getTag() const;

	// setTag writes the string value for the top level tag
	void setTag(const std::string &value);

	/**
	 * Generic setter method (expects one of primitive data types for the value)
	 */
	template <class Value>
	int set(const std::string &key, const Value &value);

	/*
	 * Data type specific setter methods
	 */
	inline int setString(const std::string &key, const std::string &value) {
		return set(key,value);
	}
	inline int setInteger(const std::string &key, const int &value) {
		return set(key,value);
	}
	inline int setDouble(const std::string &key, const double &value) {
		return set(key,value);
	}
	inline int setBoolean(const std::string &key, const bool &value) {
		return set(key,value);
	}

	/**
	 * Generic getter method (expects one of primitive data types for the value)
	 */
	template <class Value>
	int get(const std::string &key, Value &value) const;

	template <class Value>
	int getList(const std::string &key, std::list<Value> &value) const;

	/*
	 * Data type specific getter methods (value by reference)
	 */

	// string getter can not be inline due to template specialization (see th file)
	int getString(const std::string &key, std::string &value) const;

	template <class Integer>
	inline int getInteger(const std::string &key, Integer &value) const {
		return get(key,value);
	}
	template <class Double>
	inline int getDouble(const std::string &key, Double &value) const {
		return get(key,value);
	}
	inline int getBoolean(const std::string &key, bool &value) const {
		return get(key,value);
	}

	int getStringList(const std::string &key, std::list<std::string> &value) const;

	template <class Integer>
	inline int getIntegerList(const std::string &key, std::list<Integer> &value) const {
		return getList(key,value);
	}
	template <class Double>
	inline int getDoubleList(const std::string &key, std::list<Double> &value) const {
		return getList(key,value);
	}
	inline int getBooleanList(const std::string &key, std::list<bool> &value) const {
		return getList(key,value);
	}

	/*
	 * Data type specific getter methods (value by return)
	 */

	// string getter can not be inline due to template specialization (see th file)
	std::string getString(const std::string &key) const;

	inline int getInteger(const std::string &key) const {
		int result;
		this->get(key,result);
		return result;
	}
	double getDouble(const std::string &key) const {
		double result;
		this->get(key,result);
		return result;
	}
	bool getBoolean(const std::string &key) const {
		bool result;
		this->get(key,result);
		return result;
	}

	std::list<std::string> getStringList(const std::string &key) const;

	inline std::list<int> getIntegerList(const std::string &key) const {
		std::list<int> result;
		this->getList(key,result);
		return result;
	}
	std::list<double> getDoubleList(const std::string &key) const {
		std::list<double> result;
		this->getList(key,result);
		return result;
	}
	std::list<bool> getBooleanList(const std::string &key) const {
		std::list<bool> result;
		this->getList(key,result);
		return result;
	}

	// helper method to easily implement output stream in derived classes
	void to_ostream(std::ostream &os = std::cout) const;
};

////////////////////////////////////////////////////////////////////////
//
// include template code
//
////////////////////////////////////////////////////////////////////////
#include "smartCommParameterRequest.th"

inline std::ostream &operator<<(std::ostream &os, const CommParameterRequest &co)
{
	co.to_ostream(os);
	return os;
}

} // end namespace SmartACE

////////////////////////////////////////////////////////////////////////
//
// serialization operators
//
////////////////////////////////////////////////////////////////////////
ACE_CDR::Boolean operator<<(ACE_OutputCDR &cdr, const SmartACE::CommParameterRequest &obj);
ACE_CDR::Boolean operator>>(ACE_InputCDR &cdr, SmartACE::CommParameterRequest &obj);

#endif
