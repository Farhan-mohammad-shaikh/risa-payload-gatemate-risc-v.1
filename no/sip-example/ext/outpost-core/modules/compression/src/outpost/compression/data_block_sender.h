/*
 * Copyright (c) 2020, Jan-Gerd Mess
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_COMPRESSION_DATA_BLOCK_SENDER_H_
#define OUTPOST_COMPRESSION_DATA_BLOCK_SENDER_H_

namespace outpost
{
namespace container
{
template <typename T>
class ReferenceQueueBase;
}

namespace compression
{
class DataBlock;

/**
 * In order to anticipate different transmission mechanisms (queues, smpc, etc.) and policies (i.e.
 * optmizing for safety, delay, jitter, etc.) for DataBlocks, the abstract DataBlockSender allows
 * for implementing these through generalization. This, in terms, enables dynamic switching of
 * policies during runtime if desired.
 */
class DataBlockSender
{
public:
    /**
     * Default constructor
     * @param queue Output queue to send the DataBlocks to.
     */
    DataBlockSender() = default;

    /**
     * Default destructor
     */
    virtual ~DataBlockSender() = default;

    /**
     * Sends a DataBlock.
     * @param block The DataBlock to send
     * @return Returns true if successful, false otherwise.
     */
    virtual bool
    send(DataBlock& block) = 0;
};

/**
 * OneTimeSender tries to send the DataBlock to a ReferenceQueue once and returns the result.
 */
class OneTimeQueueSender : public DataBlockSender
{
public:
    /**
     * Constructor that is handed an outgoing queue.
     * @param queue ReferenceQueue to send DataBlocks to.
     */
    explicit OneTimeQueueSender(outpost::container::ReferenceQueueBase<DataBlock>& queue);
    ~OneTimeQueueSender() = default;

    /**
     * @see DataBlockSender::send
     */
    bool
    send(DataBlock&) override;

protected:
    outpost::container::ReferenceQueueBase<DataBlock>& mOutputQueue;
};

}  // namespace compression
}  // namespace outpost

#endif /* EXT_OUTPOST_CORE_MODULES_COMPRESSION_SRC_OUTPOST_COMPRESSION_DATA_BLOCK_SENDER_H_ */
