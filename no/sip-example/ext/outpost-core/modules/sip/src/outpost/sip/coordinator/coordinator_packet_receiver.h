/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2023, Tobias Pfeffer
 * Copyright (c) 2024, Felix Passenberg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SIP_COORDINATOR_PACKET_RECEIVER_H_
#define OUTPOST_SIP_COORDINATOR_PACKET_RECEIVER_H_

#include <outpost/rtos/thread.h>
#include <outpost/sip/coordinator/coordinator.h>
#include <outpost/sip/packet_transport/packet_transport.h>
#include <outpost/support/heartbeat.h>

// for unit test
class TestCoordinatorPacketReceiver;

namespace outpost
{
namespace sip
{
/*
 * Receive responses from the workers.
 * Successfully received responses will be send to a queue.
 * The communication is synchronized, it means if a request is sent than the Coordinator wait for a
 * response with time out. The Coordinator don't send parallel messages. Parallel transaction are
 * not allowed.
 */
class CoordinatorPacketReceiver : public outpost::rtos::Thread
{
    // for unit test
    friend class ::TestCoordinatorPacketReceiver;

public:
    /*
     * Constructs the Packet Receiver of the Coordinator
     *
     *  \param packetTransportIn
     *  	Packet Transport layer of SIP (UART, SpW, ...)
     *  \param coordinatorIn
     *  	Coordinator in order send data to the queue
     *  \param threadPriority
     *  	Priority of the thread
     *  \param stackSize
     *  	Stack size of the thread
     */
    CoordinatorPacketReceiver(outpost::sip::PacketTransport& packetTransportIn,
                              outpost::sip::Coordinator& coordinatorIn,
                              outpost::support::parameter::HeartbeatSource heartbeatSource,
                              uint8_t threadPriority,
                              size_t stackSize = outpost::rtos::Thread::defaultStackSize);

private:
    outpost::sip::PacketTransport& mPacketTransport;
    outpost::sip::Coordinator& mCoordinator;
    outpost::support::parameter::HeartbeatSource mHeartbeatSource;
    uint8_t mBuffer[sip::parameter::maxPacketLength];

    static constexpr outpost::time::Duration packetWaitTime = outpost::time::Seconds(1);

    void
    run() override;

    enum class receiveResult
    {
        success,
        readError,
        receiveError,
        queueError
    };

    receiveResult
    receivePacket(outpost::time::Duration timeout);
};
}  // namespace sip
}  // namespace outpost
#endif /* OUTPOST_SIP_COORDINATOR_PACKET_RECEIVER_H_ */
