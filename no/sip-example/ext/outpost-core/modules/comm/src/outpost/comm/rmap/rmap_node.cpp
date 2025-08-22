/*
 * Copyright (c) 2017-2018, Muhammad Bassam
 * Copyright (c) 2017, Fabian Greif
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "rmap_node.h"

#include "rmap_packet.h"
#include "rmap_status.h"

using namespace outpost::comm;

//------------------------------------------------------------------------------
RmapTargetNode::RmapTargetNode() :
    mTargetSpaceWireAddressLength(0),
    mTargetSpaceWireAddress(),
    mReplyAddressLength(0),
    mReplyAddress(),
    mTargetLogicalAddress(rmap::defaultLogicalAddress),
    mKey(0),
    mId(0)
{
    strcpy(mName, "Default");
    memset(mTargetSpaceWireAddress, 0, sizeof(mTargetSpaceWireAddress));
    memset(mReplyAddress, 0, sizeof(mReplyAddress));
}

RmapTargetNode::RmapTargetNode(const char* name,
                               uint8_t id,
                               uint8_t targetLogicalAddress,
                               uint8_t key) :
    mTargetSpaceWireAddressLength(0),
    mReplyAddressLength(0),
    mTargetLogicalAddress(targetLogicalAddress),
    mKey(key),
    mId(id)
{
    if (strlen(name) < rmap::maxNodeNameLength)
    {
        strcpy(mName, name);
    }
    else
    {
        strcpy(mName, "Default");
    }

    memset(mTargetSpaceWireAddress, 0, sizeof(mTargetSpaceWireAddress));
    memset(mReplyAddress, 0, sizeof(mReplyAddress));
}

RmapTargetNode::~RmapTargetNode()
{
}

bool
RmapTargetNode::setReplyAddress(outpost::Slice<uint8_t> replyAddress)
{
    bool result = false;
    if (replyAddress.getNumberOfElements() <= sizeof(mReplyAddress))
    {
        if (replyAddress.getNumberOfElements() > 0)
        {
            // avoid nullptr copy
            memcpy(mReplyAddress, replyAddress.begin(), replyAddress.getNumberOfElements());
        }
        mReplyAddressLength = replyAddress.getNumberOfElements();
        result = true;
    }
    return result;
}

bool
RmapTargetNode::setTargetSpaceWireAddress(outpost::Slice<uint8_t> targetSpaceWireAddress)
{
    bool result = false;
    if (targetSpaceWireAddress.getNumberOfElements() <= sizeof(mTargetSpaceWireAddress))
    {
        if (targetSpaceWireAddress.getNumberOfElements())
        {
            // avoid nullptr copy
            memcpy(mTargetSpaceWireAddress,
                   targetSpaceWireAddress.begin(),
                   targetSpaceWireAddress.getNumberOfElements());
        }
        mTargetSpaceWireAddressLength = targetSpaceWireAddress.getNumberOfElements();
        result = true;
    }
    return result;
}
