/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2022, Jan-Gerd Mess
 * Copyright (c) 2023-2024, Felix Passenberg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "worker.h"

#include <outpost/sip/packet/packet_writer.h>

outpost::sip::Worker::Worker(uint8_t workerIdIn, outpost::sip::PacketTransport& packetTransportIn) :
    mWorkerId(workerIdIn), mPacketTransport(packetTransportIn), mBuffer{}, mBufferToWrite(mBuffer)
{
}

outpost::Expected<size_t, outpost::sip::OperationResult>
outpost::sip::Worker::sendResponse(uint8_t counter,
                                   uint8_t type,
                                   outpost::Slice<uint8_t> payloadData)
{
    outpost::sip::PacketWriter packetWriter(mBufferToWrite);
    packetWriter.setWorkerId(mWorkerId);
    packetWriter.setCounter(counter);
    packetWriter.setType(type);
    packetWriter.setPayloadData(payloadData);
    // send response

    auto maybeReader = packetWriter.getReader();
    if (!maybeReader)
    {
        return outpost::unexpected(maybeReader.error());
    }

    return mPacketTransport.transmit(*maybeReader);
}
