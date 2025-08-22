/*
 * Copyright (c) 2024, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/rtos/thread.h>
#include <outpost/sip/packet_transport/packet_transport.h>

namespace example
{
namespace sip
{

class WorkerThread : public outpost::rtos::Thread
{
public:
    WorkerThread(uint8_t priority, outpost::sip::PackerTransport& transport) :
        outpost::rtos::Thread(priority),
        mPacketTransport(transport)
    {

    }

private:
    outpost::sip::PacketTransport& mPacketTransport;
};
    
}
}