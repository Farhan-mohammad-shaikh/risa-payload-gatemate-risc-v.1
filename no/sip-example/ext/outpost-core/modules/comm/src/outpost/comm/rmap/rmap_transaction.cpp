/*
 * Copyright (c) 2017, Muhammad Bassam
 * Copyright (c) 2019, 2021, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "rmap_transaction.h"

using namespace outpost::comm;

RmapTransaction::RmapTransaction() :
    mTargetLogicalAddress(0),
    mInitiatorLogicalAddress(0),
    mTransactionID(0),
    mTimeoutDuration(outpost::time::Duration::zero()),
    mState(State::notInitiated),
    mBlockingMode(false),
    mReplyPacket(),
    mCommandPacket(),
    mReplyLock(outpost::rtos::BinarySemaphore::State::released),
    mBuffer()
{
}

RmapTransaction::~RmapTransaction()
{
}

void
RmapTransaction::reset()
{
    mTargetLogicalAddress = 0;
    mInitiatorLogicalAddress = 0;
    mTransactionID = 0;
    mTimeoutDuration = outpost::time::Duration::zero();
    mState = State::notInitiated;
    mBlockingMode = false;
    mReplyPacket.reset();
    mCommandPacket.reset();
    mBuffer = outpost::hal::SpWMessage();
}
