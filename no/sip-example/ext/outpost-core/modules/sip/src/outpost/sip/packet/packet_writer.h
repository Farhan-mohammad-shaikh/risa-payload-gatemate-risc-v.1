/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2022-2023, Tobias Pfeffer
 * Copyright (c) 2023-2024, Felix Passenberg
 * Copyright (c) 2023-2024, Jan-Gerd Mess
 * Copyright (c) 2024, Pascal Pieper
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SIP_PACKET_WRITER_H_
#define OUTPOST_SIP_PACKET_WRITER_H_

#include <outpost/base/slice.h>
#include <outpost/parameter/sip.h>
#include <outpost/sip/constants.h>
#include <outpost/sip/operation_result.h>
#include <outpost/sip/packet/packet_reader.h>
#include <outpost/storage/serialize.h>
#include <outpost/utils/expected.h>

#include <stdint.h>

namespace outpost
{
namespace sip
{
/*
 * Write a SIP packet.
 */
class PacketWriter
{
public:
    /*
     * Constructs a packet
     *
     * /param bufferIn
     * 		The created packet will be wrote in this buffer.
     */
    explicit PacketWriter(outpost::Slice<uint8_t> bufferIn);

    ~PacketWriter();

    /*
     * Set WorkerID in the internal variables.
     */
    inline void
    setWorkerId(uint8_t workerId)
    {
        mWorkerId = workerId;
        mFinalized = false;
    }

    /*
     * Set counter in the internal variables.
     */
    inline void
    setCounter(uint8_t counter)
    {
        mCounter = counter;
        mFinalized = false;
    }

    /*
     * Set packet type in theinternal variables.
     */
    inline void
    setType(uint8_t type)
    {
        mType = type;
        mFinalized = false;
    }

    /*
     * Set payload data in the internal variables.
     */
    inline void
    setPayloadData(outpost::Slice<const uint8_t> payloadData)
    {
        if (payloadData.getNumberOfElements() > parameter::maxPayloadLength
            || payloadData.getNumberOfElements() == 0)
        {
            mFinalized = false;
            return;
        }
        mPayloadBuffer = payloadData;
        mFinalized = false;
    }

    /*
     * serialize packet data from intenal memory to Buffer.
     *
     * Note:
     * This does NOT increase the counter anymore.
     *
     * @return Expected
     * @retval lengthErrorTooSmall
     *      mBuffer is too short to contain header plus playload.
     * @retval success
     *      mBuffer contains the Sip packet.
     */
    outpost::Expected<OperationResult, OperationResult>
    update();

    /*
     * a const method to get the finalized packet.
     *
     * @return Expected
     * @retval success: Slice
     *      The Slice to the trunked Buffer containing only the SIP package.
     * @retval fail: OperationResult notFinalized
     *      The internal memory variables have not been written to the Buffer yet.
     *      Call /c update() first.
     */
    inline outpost::Expected<outpost::Slice<const uint8_t>, OperationResult>
    getSliceIfFinalized() const
    {
        if (mFinalized)
        {
            return mBufferToWrite.first(mLength - constants::structureInLength
                                        + constants::minPacketSize);
        }
        return outpost::unexpected<OperationResult>(OperationResult::notFinalized);
    }

    /**
     * Get A Packet Reader.
     *
     * Finalizes the Packet if nescesassary, cannot be const for this reason.
     *
     * @return Expected
     * @retval success: PacketReader
     *      A PacketReader on the current buffer
     * @retval failure: OperationResult notFinalized
     *      Packet could not be finalized. Something bad happened.
     **/
    inline outpost::Expected<PacketReader, OperationResult>
    getReader()
    {
        if (!mFinalized)
        {
            update();
        }
        auto const slice = getSliceIfFinalized();
        if (slice)
        {
            PacketReader reader(*slice);
            reader.readPacket();
            return reader;
        }
        return outpost::unexpected(OperationResult::notFinalized);
    }

    /**
     * @brief Get the entire buffer. Mostly for debug purposes and reception.
     */
    inline outpost::Slice<uint8_t>
    getFullSlice() const
    {
        return mBufferToWrite;
    }

private:
    size_t mLength;
    uint8_t mWorkerId;
    uint8_t mCounter;
    uint8_t mType;
    bool mFinalized;
    outpost::Slice<const uint8_t> mPayloadBuffer;
    outpost::Slice<uint8_t> mBufferToWrite;
};
}  // namespace sip
}  // namespace outpost
#endif /* OUTPOST_SIP_PACKET_WRITER_H_ */
