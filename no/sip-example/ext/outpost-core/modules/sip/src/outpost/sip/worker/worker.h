/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2022-2023, Tobias Pfeffer
 * Copyright (c) 2023-2024, Felix Passenberg
 * Copyright (c) 2024, Pascal Pieper
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SIP_WORKER_H_
#define OUTPOST_SIP_WORKER_H_

#include <outpost/base/slice.h>
#include <outpost/parameter/sip.h>
#include <outpost/sip/operation_result.h>
#include <outpost/sip/packet_transport/packet_transport.h>
#include <outpost/utils/expected.h>

namespace outpost
{
namespace sip
{
/*
 * Create a response packet and send back to the coordinator.
 */
class Worker
{
public:
    /*
     * Created the Worker
     *
     * /param packetTransportIn
     * 		Packet transport layer for SIP.
     */
    Worker(uint8_t workerIdIn, outpost::sip::PacketTransport& packetTransportIn);

    /*
     * \retval WokrerId
     */
    inline uint8_t
    getId() const
    {
        return mWorkerId;
    }

    /**
     * Send a response back to the Coordinator
     *
     * \param counter
     * 		Packet counter
     *
     * \param type
     * 		Type of the packet
     *
     * \param payloadData
     * 		Payload data to be send
     *
     * \return Expected
     * \retval true size_t
     *      Data successfully sent. Amount of sent Data.
     * \retval false OperationResult
     *      Data sent failed.
     *      see Error message. See \c PacketWriter and \c PacketTransport.
     */
    outpost::Expected<size_t, OperationResult>
    sendResponse(uint8_t counter,
                 uint8_t type,
                 outpost::Slice<uint8_t> payloadData = outpost::Slice<uint8_t>::empty());

private:
    uint8_t mWorkerId;
    outpost::sip::PacketTransport& mPacketTransport;
    std::array<uint8_t, outpost::sip::parameter::maxPacketLength> mBuffer;
    outpost::Slice<uint8_t> mBufferToWrite;
};
}  // namespace sip
}  // namespace outpost

#endif /* OUTPOST_SIP_WORKER_H_ */
