/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2023, Tobias Pfeffer
 * Copyright (c) 2023-2024, Felix Passenberg
 * Copyright (c) 2024, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "coordinator.h"

#include <outpost/parameter/sip.h>
#include <outpost/sip/packet/packet_reader.h>
#include <outpost/sip/packet/packet_writer.h>

outpost::sip::Coordinator::Coordinator(outpost::sip::PacketTransport& packetTransportIn) :
    mPacketTransport(packetTransportIn),
    mBuffer{},
    mBufferToWrite(mBuffer),
    mResponseQueue(sizeOfQueue)
{
}

outpost::sip::OperationResult
outpost::sip::Coordinator::sendRequest(uint8_t workerId,
                                       uint8_t counter,
                                       uint8_t type,
                                       uint8_t expectedResponseType,
                                       outpost::Slice<uint8_t> sendData)
{
    // packet writer
    outpost::sip::PacketWriter packetWriter(mBufferToWrite);

    packetWriter.setWorkerId(workerId);
    packetWriter.setCounter(counter);
    packetWriter.setType(type);
    packetWriter.setPayloadData(sendData);

    auto const dataToSend = packetWriter.getReader();

    if (!dataToSend)
    {
        return dataToSend.error();
    }

    // send request
    if (!mPacketTransport.transmit(*dataToSend))
    {
        return outpost::sip::OperationResult::transmitError;
    }

    // check response
    ResponseData data;
    if (receiveResponseQueue(data, outpost::time::Seconds(2)) == false)
    {
        return outpost::sip::OperationResult::responseError;
    }

    // check worker id
    if (data.workerId != workerId)
    {
        return outpost::sip::OperationResult::workerIdError;
    }

    // check response type
    if (data.type != expectedResponseType)
    {
        return outpost::sip::OperationResult::responseTypeError;
    }

    return outpost::sip::OperationResult::success;
}

outpost::sip::OperationResult
outpost::sip::Coordinator::sendRequestGetResponseData(uint8_t workerId,
                                                      uint8_t counter,
                                                      uint8_t type,
                                                      uint8_t expectedResponseType,
                                                      outpost::Slice<uint8_t> sendData,
                                                      outpost::Slice<uint8_t>& workerResponseData)
{
    // packet writer
    outpost::sip::PacketWriter packetWriter(mBufferToWrite);

    packetWriter.setWorkerId(workerId);
    packetWriter.setCounter(counter);
    packetWriter.setType(type);
    packetWriter.setPayloadData(sendData);

    auto const dataToSend = packetWriter.getReader();
    if (!dataToSend)
    {
        return dataToSend.error();
    }

    // send request
    if (!mPacketTransport.transmit(*dataToSend))
    {
        return outpost::sip::OperationResult::transmitError;
    }

    // check response
    ResponseData data;
    if (receiveResponseQueue(data, outpost::time::Seconds(2)) == false)
    {
        return outpost::sip::OperationResult::responseError;
    }

    // check worker id
    if (data.workerId != workerId)
    {
        return outpost::sip::OperationResult::workerIdError;
    }

    // check response type
    if (data.type != expectedResponseType)
    {
        return outpost::sip::OperationResult::responseTypeError;
    }

    // get worker data
    for (uint16_t i = 0; i < data.payloadDataLength; i++)
    {
        workerResponseData[i] = data.payloadData[i];
    }

    return outpost::sip::OperationResult::success;
}
