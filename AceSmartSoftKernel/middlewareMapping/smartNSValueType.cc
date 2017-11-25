// --------------------------------------------------------------------------
//
//  Copyright (C) 2012 Alex Lotz
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

#include "smartNSValueType.hh"

namespace SmartACE {

NSValueType::NSValueType() :
      address((u_short) 0), uuid()
{
   ACE_TRACE ("NSValueType::NSValueType");
}
NSValueType::NSValueType(const ACE_TString &strAddr) :
      address((u_short) 0), uuid()
{
   ACE_TRACE ("NSValueType::NSValueType");
   setFromString(strAddr);
}
NSValueType::NSValueType(const ACE_INET_Addr &addr) :
      address(addr), uuid()
{
   ACE_TRACE ("NSValueType::NSValueType");
}
NSValueType::NSValueType(const ACE_INET_Addr &addr, const ACE_Utils::UUID &uuid) :
      address(addr), uuid(uuid)
{
   ACE_TRACE ("NSValueType::NSValueType");
}

NSValueType::NSValueType(const NSValueType &v)
{
	address = v.address;
	uuid = v.uuid;
}

void NSValueType::set(const ACE_INET_Addr &addr, const ACE_Utils::UUID &uuid)
{
   ACE_TRACE ("NSValueType::set");
   this->address = addr;
   this->uuid = uuid;
}

int NSValueType::addrToStr(const ACE_INET_Addr &addr, ACE_TString &value) const
{
   ACE_TRACE ("NSValueType::addrToStr");
   ACE_TCHAR adress[MAXNAMELEN];
   if (addr.addr_to_string(adress, MAXNAMELEN) == 0) {
      value.clear();
      value = adress;
      return 0;
   }

   return -1;
}
int NSValueType::strToAddr(const ACE_TString &value, ACE_INET_Addr &addr) const
{
   ACE_TRACE ("NSValueType::strToAddr");
   return addr.string_to_addr(value.c_str());
}

int NSValueType::addrToPortStr(const ACE_INET_Addr &addr,
      ACE_TString &value) const
{
   ACE_TRACE ("NSValueType::addrToPortstr");
   ACE_TCHAR address[MAXNAMELEN];
   ACE_OS::itoa(addr.get_port_number(), address, 10);

   value.clear();
   value.set(address);

   return 0;
}
int NSValueType::portStrToAddr(const ACE_TString &value,
      ACE_INET_Addr &addr) const
{
   ACE_TRACE ("NSValueType::portStrToAddr");
   int portnumber = ACE_OS::atoi(value.c_str());
   return addr.set((u_short) portnumber);
}

ACE_TString NSValueType::toString() const
{
   ACE_TRACE ("NSValueType::toString");
   ACE_TString retval;

   // 1) add inet-addr as string
   if (addrToStr(address, retval) != 0)
      return "";

   // 2) add uuid-separator
   retval += ACE_TEXT("#");

   // 3) add uui as string
   retval += uuid.to_string()->c_str();

   return retval;
}

NSValueType& NSValueType::operator=(const ACE_TString& strAddr)
{
   ACE_TRACE ("NSValueType::operator=");
   this->setFromString(strAddr);
   return *this;
}

int NSValueType::setFromString(const ACE_TString &strAddr)
{
   ACE_TRACE ("NSValueType::setFromString");
   // local variables
   ACE_TString str_addr;
   ACE_TString str_uuid;

   // 1) get first substring representing <IP-Address:PORT-Nr>
   str_addr = strAddr.substr(0, strAddr.find(ACE_TEXT("#")));
   if (str_addr.length() == 0)
      return -1;

   // 2) get the second substring representing a UUID
   str_uuid = strAddr.substr(strAddr.find(ACE_TEXT("#")) + 1, strAddr.length());
   if (str_uuid.length() == 0)
      return -1;

   // 3) convert first substring into inet-address
   if (strToAddr(str_addr, address) != 0)
      return -1;

   // 4) convert second substring into UUID object
   uuid.from_string(str_uuid.c_str());

   return 0;
}

} /* namespace CHS */
