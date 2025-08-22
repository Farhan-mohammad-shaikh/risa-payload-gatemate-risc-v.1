/*
 * Copyright (c) 2018, Jan-Gerd Mess
 * Copyright (c) 2018, Fabian Greif
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2021, pfef_to
 * Copyright (c) 2022, Passenberg, Felix Constantin
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Felix Passenberg
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "shared_buffer.h"

namespace outpost
{
namespace container
{

SharedBuffer::SharedBuffer() : mReferenceCounter(0), mBuffer(outpost::Slice<uint8_t>::empty())
{
}

SharedBuffer::~SharedBuffer()
{
}

void
SharedBuffer::incrementCount()
{
    mReferenceCounter.fetch_add(1, std::memory_order_relaxed);
}

void
SharedBuffer::decrementCount()
{
    auto current = mReferenceCounter.load(std::memory_order_relaxed);
    OUTPOST_ASSERT(current > 0U, "For each decrement there must have been a previous increment");

    bool success = false;
    while (current > 0U && !success)
    {
        success = mReferenceCounter.compare_exchange_strong(
                current, current - 1, std::memory_order_acq_rel);
    }
}

bool
SharedBufferPointer::getChild(SharedChildPointer& ptr,
                              uint16_t type,
                              size_t pOffset,
                              size_t length) const
{
    bool res = false;
    if (isUsed())
    {
        if (((mOffset + pOffset + length) <= mOffset + mLength))
        {
            ptr = SharedChildPointer(mPtr, *this);
            ptr.mType = type;
            ptr.mOffset = mOffset + pOffset;
            ptr.mLength = length;
            res = true;
        }
    }
    return res;
}

bool
ConstSharedBufferPointer::getChild(ConstSharedChildPointer& ptr,
                                   uint16_t type,
                                   size_t pOffset,
                                   size_t length) const
{
    bool res = false;
    if (isUsed())
    {
        if (((mOffset + pOffset + length) <= mOffset + mLength))
        {
            ptr = ConstSharedChildPointer(mPtr, *this);
            ptr.mType = type;
            ptr.mOffset = mOffset + pOffset;
            ptr.mLength = length;
            res = true;
        }
    }
    return res;
}
}  // namespace container
}  // namespace outpost
