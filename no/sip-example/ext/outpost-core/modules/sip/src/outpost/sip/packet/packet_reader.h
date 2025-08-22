/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2022-2023, Tobias Pfeffer
 * Copyright (c) 2023-2024, Felix Passenberg
 * Copyright (c) 2023, Jan-Gerd Mess
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

#ifndef OUTPOST_SIP_PAKCET_READER_H_
#define OUTPOST_SIP_PAKCET_READER_H_

#include <outpost/base/slice.h>
#include <outpost/sip/constants.h>
#include <outpost/sip/operation_result.h>
#include <outpost/storage/serialize.h>
#include <outpost/utils/expected.h>

namespace outpost
{
namespace sip
{
class PacketReader
{
    /*
     * Read a SIP packet.
     * Deserialize it and check the CRC.
     */
public:
    /*
     * Constructs the SIP packet reader
     *
     * \param bufferIn
     * 		Consist a packet, which will be read out
     */
    explicit PacketReader(outpost::Slice<const uint8_t> bufferIn);

    ~PacketReader() = default;

    /*
     * Read a SIP packet
     *
     * \retval success
     *      Success
     * \retval crcError
     *      CRC error.
     * \retval other
     *      see \c deserialize
     */
    OperationResult
    readPacket();

    /*
     * Get length of the packet.
     *
     * Note: length can be larger than 0xffff
     * as the lenght field itself is not counted in the length field.
     * -> 0xffff + 2
     */
    inline size_t
    getLength() const
    {
        return mLength;
    }

    /*
     * Get WorkerID.
     */
    inline uint8_t
    getWorkerId() const
    {
        return mWorkerId;
    }

    /*
     * Get counter.
     */
    inline uint8_t
    getCounter() const
    {
        return mCounter;
    }

    /*
     * Get packet type.
     */
    inline uint8_t
    getType() const
    {
        return mType;
    }

    /*
     * Get payload data.
     */
    inline outpost::Slice<const uint8_t>
    getPayloadData() const
    {
        return mBuffer.subSlice(sip::constants::packetHeaderLength, mPayloadDataLength);
    }

    /**
     * @brief Get the entire packet. Mostly for transmission purposes.
     */
    inline outpost::Slice<const uint8_t>
    getSlice() const
    {
        return mBuffer.first(mLength - constants::structureInLength + constants::minPacketSize);
    }

    /**
     * @brief Get the entire buffer. Mostly for debug purposes.
     */
    inline outpost::Slice<const uint8_t>
    getFullSlice() const
    {
        return mBuffer;
    }

private:
    size_t mLength;
    uint8_t mWorkerId;
    uint8_t mCounter;
    uint8_t mType;
    size_t mPayloadDataLength;
    uint16_t mCrc;
    outpost::Slice<const uint8_t> mBuffer;

    /**
     * @brief Read the Packet from Byte Array to member variables.
     *
     * @retval lengthErrorTooSmall
     *      The Slice is too short to contain a valid SIP packet.
     * @retval lengthErrorEndOfFrame
     *      The Length read from the header is larger than the Slice.
     * @retval success
     *      The PacketReader Class now contains valid data.
     **/
    OperationResult
    deserialize();

    /**
     * @brief calculates the CRC over the SIP defined range
     **/
    uint16_t
    calculateCrc() const;
};

}  // namespace sip
}  // namespace outpost
#endif /* OUTPOST_SIP_PAKCET_READER_H_ */
