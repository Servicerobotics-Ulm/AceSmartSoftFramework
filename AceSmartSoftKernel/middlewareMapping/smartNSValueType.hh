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

#ifndef SMARTNSVALUETYPE_HH_
#define SMARTNSVALUETYPE_HH_

#include <ace/INET_Addr.h>
#include <ace/UUID.h>

namespace SmartACE {

class NSValueType
{
private:
   ACE_INET_Addr address;
   ACE_Utils::UUID uuid;

protected:
   /// helper function to convert <i>ACE_INET_Addr</i> into <i>String</i> representation.
   /** @internal
    * See strToAddr() for opposite direction.
    * @param addr  ACE_INET_Addr
    * @param string ACE_TString (compatible to std::string) value
    * @return 0 on success
    * @sa strToAddr(), addrToPortstr(), portStrToAddr()
    */
   int addrToStr(const ACE_INET_Addr &addr, ACE_TString &string) const;

   /// helper function to convert <i>String</i> into <i>ACE_INET_Addr</i> representation.
   /** @internal
    * See addrToStr() for opposite direction.
    * @param string ACE_TString (compatible to std::string) value
    * @param addr  ACE_INET_Addr
    * @return 0 on success
    * @sa addrToStr(), portStrToAddr(), portStrToAddr()
    */
   int strToAddr(const ACE_TString &string, ACE_INET_Addr &addr) const;

   /// helper function is similar to addrToStr().
   /** @internal
    * This function converst ACE_INET_Addr into ACE_TString representaion,
    * similar to addrToStr() with the difference that string contains only
    * the port number (without ip address).
    * @param addr  ACE_INET_Addr
    * @param value ACE_TString (compatible to std::string) value
    * @return 0 on success
    * @sa addrToStr(), strToAddr(), portStrToAddr()
    */
   int addrToPortStr(const ACE_INET_Addr &addr, ACE_TString &value) const;

   /// helper function is similar to strToAddr().
   /** @internal
    * This function converst ACE_TString into ACE_INET_Addr representaion,
    * similar to strToAddr() with the difference that string contains only
    * the port number (without ip address).
    * @param value ACE_TString (compatible to std::string) value
    * @param addr  ACE_INET_Addr
    * @return 0 on success
    * @sa addrToStr(), strToAddr(), addrToPortstr()
    */
   int portStrToAddr(const ACE_TString &value, ACE_INET_Addr &addr) const;

public:
   NSValueType();
   NSValueType(const ACE_INET_Addr &addr);
   NSValueType(const ACE_INET_Addr &addr, const ACE_Utils::UUID &uuid);
   NSValueType(const ACE_TString &strAddr);
   NSValueType(const NSValueType &);

   void set(const ACE_INET_Addr &addr, const ACE_Utils::UUID &uuid);
   NSValueType& operator=(const ACE_TString& strAddr);
   int setFromString(const ACE_TString &strAddr);

   ACE_TString toString() const;
   operator ACE_TString() const
   {
      return this->toString();
   }

   inline void setInetAddr(const ACE_INET_Addr &addr)
   {
      this->address = addr;
   }
   inline ACE_INET_Addr getInetAddr() const
   {
      return address;
   }
   inline void setUUID(const ACE_Utils::UUID &uuid)
   {
      this->uuid = uuid;
   }
   inline ACE_Utils::UUID getUUID() const
   {
      return uuid;
   }
   inline void setPortNumber(const u_short &nbr) {
	   address.set_port_number(nbr);
   }
   inline u_short getPortNumber() const {
	   return address.get_port_number();
   }

   static ACE_TString getSmartType()
   {
      return "smartip";
   }
};

} /* namespace CHS */
#endif /* SMARTNSVALUETYPE_HH_ */
