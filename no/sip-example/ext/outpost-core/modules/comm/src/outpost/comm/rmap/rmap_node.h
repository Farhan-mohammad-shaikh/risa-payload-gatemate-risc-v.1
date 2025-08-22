/*
 * Copyright (c) 2017, Muhammad Bassam
 * Copyright (c) 2017-2018, Fabian Greif
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_COMM_RMAP_NODE_H_
#define OUTPOST_COMM_RMAP_NODE_H_

#include "rmap_common.h"

#include <outpost/base/slice.h>

#include <stdint.h>
#include <string.h>

namespace outpost
{
namespace comm
{
/**
 * RMAP target node.
 *
 * Provides the RMAP object level information for the listed RMAP targets.
 *
 * \author  Muhammad Bassam
 */
class RmapTargetNode
{
public:
    RmapTargetNode();
    RmapTargetNode(const char* name, uint8_t id, uint8_t targetLogicalAddress, uint8_t key);
    ~RmapTargetNode();

    /**
     * Sets the reply address of the target to the given value
     *
     * \param replyAddress
     *      Reply address to be set
     *
     * \return
     *      True for successful, false for wrong size parameter
     *
     * */
    bool
    setReplyAddress(outpost::Slice<uint8_t> replyAddress);

    /**
     * Sets the SpW target addresses to the given value
     *
     * \param targetSpaceWireAddress
     *      SpW addresses to be set
     *
     * \return
     *      True for successful, false for wrong size parameter
     *
     * */
    bool
    setTargetSpaceWireAddress(outpost::Slice<uint8_t> targetSpaceWireAddress);

    //--------------------------------------------------------------------------

    inline uint8_t
    getKey() const
    {
        return mKey;
    }

    inline outpost::Slice<uint8_t>
    getReplyAddress()
    {
        return outpost::Slice<uint8_t>::unsafe(mReplyAddress, mReplyAddressLength);
    }

    inline uint8_t
    getTargetLogicalAddress() const
    {
        return mTargetLogicalAddress;
    }

    inline outpost::Slice<uint8_t>
    getTargetSpaceWireAddress()
    {
        return outpost::Slice<uint8_t>::unsafe(mTargetSpaceWireAddress,
                                               mTargetSpaceWireAddressLength);
    }

    inline void
    setKey(uint8_t defaultKey)
    {
        mKey = defaultKey;
    }

    inline void
    setTargetLogicalAddress(uint8_t targetLogicalAddress)
    {
        mTargetLogicalAddress = targetLogicalAddress;
    }

    inline const char*
    getName() const
    {
        return mName;
    }

    inline uint8_t
    getId() const
    {
        return mId;
    }

private:
    // TODO Replace with bounded array
    uint8_t mTargetSpaceWireAddressLength;
    uint8_t mTargetSpaceWireAddress[rmap::maxNumberOfTargetNodes];
    // TODO Replace with bounded array
    uint8_t mReplyAddressLength;
    uint8_t mReplyAddress[rmap::maxNumberOfTargetNodes];
    uint8_t mTargetLogicalAddress;
    uint8_t mKey;
    char mName[rmap::maxNodeNameLength];
    uint8_t mId;
};

}  // namespace comm
}  // namespace outpost

#endif /* OUTPOST_COMM_RMAP_NODE_H_ */
