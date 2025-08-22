/*
 * Copyright (c) 2020-2023, Jan-Gerd Mess
 * Copyright (c) 2023, Hannah Kirstein
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Reinking, Janosch
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "data_processor_thread.h"

#include "data_block.h"
#include "legall_wavelet.h"

#include <outpost/base/fixedpoint.h>
#include <outpost/container/reference_queue.h>
#include <outpost/container/shared_object_pool.h>
#include <outpost/support/heartbeat.h>

namespace outpost
{
namespace compression
{
constexpr outpost::time::Duration DataProcessorThread::waitForBlockTimeout;
constexpr outpost::time::Duration DataProcessorThread::processingTimeout;

DataProcessorThread::DataProcessorThread(
        uint8_t thread_priority,
        outpost::support::parameter::HeartbeatSource heartbeatSource,
        outpost::container::SharedBufferPoolBase& pool,
        outpost::container::ReferenceQueueBase<DataBlock>& inputQueue,
        outpost::container::ReferenceQueueBase<DataBlock>& outputQueue,
        uint8_t numOutputRetries,
        outpost::time::Duration retryTimeout,
        const char* name,
        outpost::rtos::CpuMask affinity) :
    outpost::rtos::Thread(
            thread_priority, 1024, name, outpost::rtos::Thread::floatingPoint, affinity),
    mHeartbeatSource(heartbeatSource),
    mInputQueue(inputQueue),
    mOutputQueue(outputQueue),
    mPool(pool),
    mCheckpoint(outpost::rtos_utils::Checkpoint::State::suspending),
    mNumIncomingBlocks(0),
    mNumProcessedBlocks(0),
    mNumForwardedBlocks(0),
    mNumLostBlocks(0),
    mRetrySendTimeout(retryTimeout),
    mMaxSendRetries(numOutputRetries)
{
}

DataProcessorThread::~DataProcessorThread()
{
}

void
DataProcessorThread::run()
{
    while (1)
    {
        outpost::support::Heartbeat::suspend(mHeartbeatSource);
        mCheckpoint.pass();
        processSingleBlock();
    }
}

void
DataProcessorThread::enable()
{
    mCheckpoint.resume();
}

void
DataProcessorThread::disable()
{
    mCheckpoint.suspend();
}

bool
DataProcessorThread::isEnabled() const
{
    return mCheckpoint.getState() == outpost::rtos_utils::Checkpoint::State::running;
}

void
DataProcessorThread::processSingleBlock(outpost::time::Duration timeout)
{
    outpost::support::Heartbeat::send(mHeartbeatSource,
                                      timeout + processingTimeout * 2U
                                              + mRetrySendTimeout * mMaxSendRetries);
    auto res = mInputQueue.receive(timeout);
    if (res)
    {
        DataBlock b = *res;
        mNumIncomingBlocks++;
        if (compress(b))
        {
            mNumProcessedBlocks++;
            bool success = false;
            for (uint8_t tries = 0; tries < mMaxSendRetries && !success; tries++)
            {
                if (outpost::utils::OperationResult::success == mOutputQueue.send(b))
                {
                    success = true;
                }
                else
                {
                    outpost::rtos::Thread::sleep(mRetrySendTimeout);
                }
            }
            if (success)
            {
                mNumForwardedBlocks++;
            }
            else
            {
                mNumLostBlocks++;
            }
        }
        else
        {
            mNumLostBlocks++;
        }
    }
}

bool
DataProcessorThread::compress(DataBlock& b)
{
    if (b.applyWaveletTransform() && b.getCoefficients().getNumberOfElements() > 0U)
    {
        outpost::container::SharedBufferPointer p;
        if (mPool.allocate(p))
        {
            DataBlock outputBlock(
                    p, b.getParameterId(), b.getStartTime(), b.getSamplingRate(), b.getBlocksize());
            if (b.encode(outputBlock, mEncoder))
            {
                b = outputBlock;
                return true;
            }
        }
    }
    return false;
}

}  // namespace compression
}  // namespace outpost
