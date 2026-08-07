/*
 *  Copyright (c) 2026, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file includes core definitions for Alternate PHY capabilities.
 */

#ifndef RADIO_ALTERNATE_PHY_HPP_
#define RADIO_ALTERNATE_PHY_HPP_

#include <stdint.h>

#include <openthread/platform/alternate_phy.h>

#include "common/array.hpp"
#include "common/encoding.hpp"

namespace ot {
namespace AlternatePhy {

typedef uint8_t                  PhyId;
typedef otAlternatePhyCapability Capability;

static constexpr uint8_t kMaxPhyCount    = OT_ALTERNATE_PHY_MAX_COUNT;
static constexpr uint8_t kParameterCount = OT_ALTERNATE_PHY_PARAMETER_COUNT;

namespace Tl3Gfsk {

static constexpr PhyId kPhyId = OT_ALTERNATE_PHY_ID_TL3_GFSK;

inline uint16_t GetMaxPsdu(const Capability &aCapability)
{
    return LittleEndian::ReadUint16(&aCapability.mParameters[OT_ALTERNATE_PHY_TL3_GFSK_PARAMETER_MAX_PSDU_LSB]);
}

inline void SetMaxPsdu(Capability &aCapability, uint16_t aMaxPsdu)
{
    LittleEndian::WriteUint16(aMaxPsdu, &aCapability.mParameters[OT_ALTERNATE_PHY_TL3_GFSK_PARAMETER_MAX_PSDU_LSB]);
}

inline bool IsValid(const Capability &aCapability)
{
    const uint16_t maxPsdu = GetMaxPsdu(aCapability);

    return (maxPsdu >= OT_ALTERNATE_PHY_TL3_GFSK_MAX_PSDU_MIN) &&
           (maxPsdu <= OT_ALTERNATE_PHY_TL3_GFSK_MAX_PSDU_MAX);
}

} // namespace Tl3Gfsk

inline bool IsValid(const Capability &aCapability)
{
    switch (aCapability.mPhyId)
    {
    case Tl3Gfsk::kPhyId:
        return Tl3Gfsk::IsValid(aCapability);

    default:
        return false;
    }
}

inline uint16_t GetMaxPsdu(const Capability &aCapability)
{
    uint16_t maxPsdu = 0;

    switch (aCapability.mPhyId)
    {
    case Tl3Gfsk::kPhyId:
        maxPsdu = Tl3Gfsk::GetMaxPsdu(aCapability);
        break;
    }

    return maxPsdu;
}

/**
 * Stores a set of Alternate PHY capabilities.
 *
 */
class Capabilities : public Array<Capability, kMaxPhyCount>
{
public:
    /**
     * Finds a capability by PHY Identifier.
     *
     * @param[in] aPhyId  The PHY Identifier to find.
     *
     * @returns A pointer to the matching capability, or `nullptr` if it is not present.
     *
     */
    const Capability *Find(PhyId aPhyId) const
    {
        const Capability *match = nullptr;

        for (const Capability &capability : *this)
        {
            if (capability.mPhyId == aPhyId)
            {
                match = &capability;
                break;
            }
        }

        return match;
    }

    /**
     * Indicates whether a capability for a given PHY Identifier is present.
     *
     * @param[in] aPhyId  The PHY Identifier to check.
     *
     * @retval TRUE   A matching capability is present.
     * @retval FALSE  A matching capability is not present.
     *
     */
    bool Contains(PhyId aPhyId) const { return Find(aPhyId) != nullptr; }

    /**
     * Adds or replaces a capability.
     *
     * @param[in] aCapability  The capability to add or replace.
     *
     * @retval kErrorNone    The capability was stored.
     * @retval kErrorNoBufs  The array is full and no existing capability has the same PHY Identifier.
     *
     */
    Error Upsert(const Capability &aCapability)
    {
        for (Capability &capability : *this)
        {
            if (capability.mPhyId == aCapability.mPhyId)
            {
                capability = aCapability;
                return kErrorNone;
            }
        }

        return PushBack(aCapability);
    }
};

} // namespace AlternatePhy
} // namespace ot

#endif // RADIO_ALTERNATE_PHY_HPP_
