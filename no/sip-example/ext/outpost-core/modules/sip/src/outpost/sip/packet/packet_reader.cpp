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

#include "packet_reader.h"

#include <outpost/coding/crc16.h>
#include <outpost/parameter/sip.h>

outpost::sip::PacketReader::PacketReader(outpost::Slice<const uint8_t> bufferIn) :
    mLength(0),
    mWorkerId(0),
    mCounter(0),
    mType(0),
    mPayloadDataLength(0),
    mCrc(0),
    mBuffer(bufferIn)
{
}

outpost::sip::OperationResult
outpost::sip::PacketReader::readPacket()
{
    auto const result = deserialize();
    if (result != OperationResult::success)
    {
        // Length failed
        return result;
    }

    // calculate CRC
    const uint16_t crcCalculated = calculateCrc();

    // check CRC
    if (mCrc != crcCalculated)
    {
        // CRC failed
        return outpost::sip::OperationResult::crcError;
    }

    return outpost::sip::OperationResult::success;
}

outpost::sip::OperationResult
outpost::sip::PacketReader::deserialize()
{
    // check buffer size
    if (mBuffer.getNumberOfElements() < outpost::sip::constants::minPacketSize)
    {
        // read out error
        return OperationResult::lengthErrorTooSmall;
    }

    outpost::Deserialize packet(mBuffer);

    mLength = packet.read<uint16_t>();
    mWorkerId = packet.read<uint8_t>();
    mCounter = packet.read<uint8_t>();
    mType = packet.read<uint8_t>();
    mPayloadDataLength = mLength - outpost::sip::constants::structureInLength;
    static_assert(parameter::maxPayloadLength <= constants::maxPayloadLengthProtocolLimit,
                  "Max length violated. \nThe length field consists of 2 bytes, its max value is "
                  "0xffff.");

    if (mPayloadDataLength > outpost::sip::parameter::maxPayloadLength)
    {
        // Length failed
        return OperationResult::lengthErrorEndOfFrame;
    }

    // TODO: This will be easier once safer_serialize is merged
    OUTPOST_ASSERT(packet.getPosition() == 5, "Header was not deserialized as expected");
    const size_t remainingBytes = mBuffer.getNumberOfElements() - packet.getPosition();

    if (mPayloadDataLength + 2 /* TODO: Use new serializer knowledge for width */
        > remainingBytes)
    {
        return OperationResult::lengthErrorTooSmall;
    }

    packet.skip(mPayloadDataLength);
    mCrc = packet.read<uint16_t>();

    return OperationResult::success;
}

uint16_t
outpost::sip::PacketReader::calculateCrc() const
{
    return outpost::Crc16Ccitt::calculate(mBuffer.first(mLength));
}
