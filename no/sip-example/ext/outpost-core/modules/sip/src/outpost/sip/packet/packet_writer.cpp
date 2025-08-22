/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2023, Tobias Pfeffer
 * Copyright (c) 2023-2024, Felix Passenberg
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

#include "packet_writer.h"

#include <outpost/coding/crc16.h>
#include <outpost/sip/constants.h>

outpost::sip::PacketWriter::PacketWriter(outpost::Slice<uint8_t> bufferIn) :
    mLength(0),
    mWorkerId(0),
    mCounter(0),
    mType(0),
    mFinalized(false),
    mPayloadBuffer(outpost::Slice<const uint8_t>::empty()),
    mBufferToWrite(bufferIn)
{
}

outpost::sip::PacketWriter::~PacketWriter()
{
}

outpost::Expected<outpost::sip::OperationResult, outpost::sip::OperationResult>
outpost::sip::PacketWriter::update()
{
    outpost::Serialize packet(mBufferToWrite);
    packet.reset();

    if (mPayloadBuffer.getNumberOfElements() + outpost::sip::constants::minPacketSize
        > mBufferToWrite.getNumberOfElements())
    {
        return outpost::unexpected(OperationResult::lengthErrorTooSmall);
    }

    mLength = mPayloadBuffer.getNumberOfElements() + outpost::sip::constants::structureInLength;
    packet.store<uint16_t>(mLength);
    // store WorkerId
    packet.store<uint8_t>(mWorkerId);
    // store counter
    // WARNING: THE COUNTER IS NOT INCREASED ANYMORE.
    packet.store<uint8_t>(mCounter);
    // store Type
    packet.store<uint8_t>(mType);
    // store payloadData
    packet.store(mPayloadBuffer);

    // compute CRC
    const uint16_t crc = outpost::Crc16Ccitt::calculate(packet.asSlice());

    // store crc
    packet.store<uint16_t>(crc);

    mFinalized = true;
    return OperationResult::success;
}
