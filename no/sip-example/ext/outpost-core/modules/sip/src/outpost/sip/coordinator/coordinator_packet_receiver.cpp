/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023-2024, Felix Passenberg
 * Copyright (c) 2023-2024, Pascal Pieper
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/sip/coordinator/coordinator_packet_receiver.h>
#include <outpost/sip/packet/packet_reader.h>

constexpr outpost::time::Duration outpost::sip::CoordinatorPacketReceiver::packetWaitTime;

outpost::sip::CoordinatorPacketReceiver::CoordinatorPacketReceiver(
        outpost::sip::PacketTransport& packetTransportIn,
        outpost::sip::Coordinator& coordinatorIn,
        outpost::support::parameter::HeartbeatSource heartbeatSource,
        uint8_t threadPriority,
        size_t stackSize) :
    outpost::rtos::Thread(threadPriority, stackSize, "CooReceiver"),
    mPacketTransport(packetTransportIn),
    mCoordinator(coordinatorIn),
    mHeartbeatSource(heartbeatSource),
    mBuffer{}
{
}

outpost::sip::CoordinatorPacketReceiver::receiveResult
outpost::sip::CoordinatorPacketReceiver::receivePacket(outpost::time::Duration timeout)
{
    outpost::Slice<uint8_t> bufferToRead(mBuffer);
    // wait for incoming response
    auto maybePacket = mPacketTransport.receive(bufferToRead, timeout);
    if (maybePacket)
    {
        outpost::sip::PacketReader packetReader(bufferToRead);

        if (packetReader.readPacket() == outpost::sip::OperationResult::success)
        {
            outpost::sip::Coordinator::ResponseData data;
            data.length = packetReader.getLength();
            data.workerId = packetReader.getWorkerId();
            data.counter = packetReader.getCounter();
            data.type = packetReader.getType();
            // copy payload data
            outpost::Slice<const uint8_t> payloadData(packetReader.getPayloadData());
            data.payloadDataLength = payloadData.getNumberOfElements();
            if (payloadData.getNumberOfElements() > 0)
            {
                outpost::Slice<uint8_t> dataPayloadSlice(data.payloadData);
                dataPayloadSlice.copyFrom(payloadData);
            }

            // send response to the queue
            bool result = mCoordinator.sendResponseQueue(data);
            if (result == false)
            {
                // send to Queue: failed
                return outpost::sip::CoordinatorPacketReceiver::receiveResult::queueError;
            }
            else
            {
                return outpost::sip::CoordinatorPacketReceiver::receiveResult::success;
            }
        }
        else
        {
            // read failed
            return outpost::sip::CoordinatorPacketReceiver::receiveResult::readError;
        }
    }
    else
    {
        // receive failed
        return outpost::sip::CoordinatorPacketReceiver::receiveResult::receiveError;
    }
}

void
outpost::sip::CoordinatorPacketReceiver::run()
{
    while (1)
    {
        outpost::support::Heartbeat::send(
                mHeartbeatSource, packetWaitTime + outpost::support::parameter::heartbeatTolerance);

        receivePacket(packetWaitTime);
    }
}
