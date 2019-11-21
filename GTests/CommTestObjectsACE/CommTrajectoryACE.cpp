//===================================================================================
//
//  Copyright (C) 2019 Alex Lotz
//
//        lotz@hs-ulm.de
//
//        Servicerobotik Ulm
//        Christian Schlegel
//        Ulm University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm
//        Germany
//
//  This file is part of the SmartSoft Component-Developer C++ API.
//
//  Redistribution and use in source and binary forms, with or without modification,
//  are permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGE.
//
//===================================================================================

#include "CommTrajectoryACE.h"

// serialization operator for CommunicationObject CommTrajectory
ACE_CDR::Boolean operator<<(ACE_OutputCDR &cdr, const CommTestObjects::CommTrajectory &object)
{
	ACE_CDR::Boolean good_bit = true;

	good_bit = good_bit && cdr << object.description;

	good_bit = good_bit && cdr << ACE_Utils::truncate_cast<ACE_CDR::ULong>(object.trajectory.size());
	for(const auto& point: object.trajectory) {
		good_bit = good_bit && cdr << point;
	}

	return good_bit;
}

// de-serialization operator for CommunicationObject CommTrajectory
ACE_CDR::Boolean operator>>(ACE_InputCDR &cdr, CommTestObjects::CommTrajectory &object)
{
	ACE_CDR::Boolean good_bit = true;

	good_bit = good_bit && cdr >> object.description;

	ACE_CDR::ULong trajectory_size;
	good_bit = good_bit && cdr >> trajectory_size;
	object.trajectory.clear();
	CommTestObjects::Comm3dPose point;
	for(ACE_CDR::ULong i=0; i<trajectory_size; ++i) {
		good_bit = good_bit && cdr >> point;
		object.trajectory.push_back(point);
	}

	return good_bit;
}

