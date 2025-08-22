/*
 * Copyright (c) 2020, Jan-Gerd Mess
 * Copyright (c) 2020, Jan Malburg
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

#ifndef OUTPOST_COMPRESSION_DATA_PROCESSOR_THREAD_H_
#define OUTPOST_COMPRESSION_DATA_PROCESSOR_THREAD_H_

#include "nls_encoder.h"

#include <outpost/parameter/support.h>
#include <outpost/rtos/thread.h>
#include <outpost/rtos_utils/checkpoint.h>
#include <outpost/storage/bitstream.h>

namespace outpost
{
namespace container
{
template <typename T>
class ReferenceQueueBase;

class SharedBufferPoolBase;
}  // namespace container

namespace compression
{
class DataBlock;

/**
 * The DataProcessorThread is responsible for taking over the workload of transforming and encoding
 * DataBlocks one at a time.
 */
class DataProcessorThread : public rtos::Thread
{
public:
    /** Constructor
     * @param thread_priority Priority in the OS' scheduler
     * @param pool SharedBufferPool for allocation of new DataBlocks
     * @param inputQueue Queue to listen to for incoming raw DataBlocks
     * @param outputQueue Queue to send encoded DataBlocks to for long-term storage or transmission
     * @param affinity CPU(s) on which the thread can be executed
     * to ground
     */
    DataProcessorThread(uint8_t thread_priority,
                        support::parameter::HeartbeatSource heartbeatSource,
                        container::SharedBufferPoolBase& pool,
                        container::ReferenceQueueBase<DataBlock>& inputQueue,
                        container::ReferenceQueueBase<DataBlock>& outputQueue,
                        uint8_t numOutputRetries = 5U,
                        time::Duration retryTimeout = time::Milliseconds(500),
                        const char* name = "DPT",
                        rtos::CpuMask affinity = rtos::inheritFromCaller);

    virtual ~DataProcessorThread();

    /**
     * The method is called by the OS' scheduler. It awaits DataBlocks on the incoming queue,
     * transforms them using the wavelet transform and then encodes the data to a newly allocated
     * DataBlock.
     */
    void
    run() override;

    /**
     * Enables the processing of DataBlocks
     */
    void
    enable();

    /**
     * Disables the processing of DataBlocks
     */
    void
    disable();

    /**
     * Getter for the number of DataBlocks that have been received from the input queue.
     * @return Returns the number of incoming blocks.
     */
    inline uint32_t
    getNumberOfReceivedBlocks() const
    {
        return mNumIncomingBlocks;
    }

    /**
     * Getter for the number of DataBlocks that have been processed.
     * @return Returns the number of processed blocks.
     */
    inline uint32_t
    getNumberOfProcessedBlocks() const
    {
        return mNumProcessedBlocks;
    }

    /**
     * Getter for the number of DataBlocks that haven been forwarded to the output queue.
     * @return Returns the number of forwarded blocks.
     */
    inline uint32_t
    getNumberOfForwardedBlocks() const
    {
        return mNumForwardedBlocks;
    }

    /**
     * Getter for the number of DataBlocks that have been lost because they could not be sent to
     * the output queue.
     * @return Returns the number of forwarded blocks.
     */
    inline uint32_t
    getNumberOfLostBlocks() const
    {
        return mNumLostBlocks;
    }

    /**
     * Getter for the thread's state.
     * @return Returns true if processing is currently enabled, false otherwise.
     */
    bool
    isEnabled() const;

    /**
     * Resets the counters for incoming, processed and forwarded blocks.
     */
    inline void
    resetCounters()
    {
        mNumIncomingBlocks = 0;
        mNumProcessedBlocks = 0;
        mNumForwardedBlocks = 0;
        mNumLostBlocks = 0;
    }

    /**
     * Goes through the entire processing sequence for a single block,
     * from reception from the input queue through compression to forwarding to the output queue.
     * @param timeout Timeout for reception of a DataBlock on the input queue.
     */
    void
    processSingleBlock(time::Duration timeout = waitForBlockTimeout);

    static constexpr uint16_t maximumEncodingBufferLength = 16500;

private:
    bool
    compress(DataBlock& b);

    support::parameter::HeartbeatSource mHeartbeatSource;

    container::ReferenceQueueBase<DataBlock>& mInputQueue;
    container::ReferenceQueueBase<DataBlock>& mOutputQueue;

    container::SharedBufferPoolBase& mPool;

    rtos_utils::Checkpoint mCheckpoint;

    uint32_t mNumIncomingBlocks;
    uint32_t mNumProcessedBlocks;
    uint32_t mNumForwardedBlocks;
    uint32_t mNumLostBlocks;

    NLSEncoder mEncoder;

    static constexpr time::Duration waitForBlockTimeout = time::Seconds(5);
    static constexpr time::Duration processingTimeout = time::Seconds(1);

    time::Duration mRetrySendTimeout;
    uint8_t mMaxSendRetries;
};

}  // namespace compression
}  // namespace outpost

#endif /* OUTPOST_COMPRESSION_DATA_PROCESSOR_THREAD_H_ */
