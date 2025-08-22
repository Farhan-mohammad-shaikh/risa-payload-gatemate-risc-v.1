/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2022-2023, Tobias Pfeffer
 * Copyright (c) 2023-2024, Felix Passenberg
 * Copyright (c) 2024, Pascal Pieper
 * Copyright (c) 2024, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SIP_COORDINATOR_H_
#define OUTPOST_SIP_COORDINATOR_H_

#include <outpost/base/slice.h>
#include <outpost/parameter/sip.h>
#include <outpost/rtos/queue.h>
#include <outpost/sip/operation_result.h>
#include <outpost/sip/packet_transport/packet_transport.h>

namespace outpost
{
namespace sip
{
/*
 * Send a request to a worker and receive the response from the queue.
 *
 */
class Coordinator
{
public:
    /*
     * Constructs the Coordinator.
     *
     *  \param packetTransportIn
     *  	Packet Transport layer of SIP (UART, SpW, ...)
     */
    explicit Coordinator(outpost::sip::PacketTransport& packetTransportIn);

    /**
     * Send a request to the Worker with data
     *
     * \param workerId
     *      The ID of worker
     *
     * \param counter
     * 		Packet counter
     *
     * \param type
     * 		Type of the packet
     *
     * \param expectedResponseType
     * 		Expected response packet type
     *
     * \param sendData
     * 		Payload data to be send
     *
     * \retval success
     * 		Success
     * \retval transmitError
     * 		Transmit failed
     * \retval responseError
     * 		Response time out
     * \retval workerIdError
     * 		Wrong worker ID in the response
     * \retval responseTypeError
     * 		Wrong response Type
     */
    OperationResult
    sendRequest(uint8_t workerId,
                uint8_t counter,
                uint8_t type,
                uint8_t expectedResponseType,
                outpost::Slice<uint8_t> sendData = outpost::Slice<uint8_t>::empty());

    /**
     * Send a request to the Worker and get response data
     *
     * \param workerId
     *      The ID of worker
     *
     * \param counter
     * 		Packet counter
     *
     * \param type
     * 		Type of the packet
     *
     * \param expectedResponseType
     * 		Expected response packet type
     *
     * \param workerResponseData
     * 		Payload data to be read
     *
     * \retval success
     * 		Success
     * \retval transmitError
     * 		Transmit failed
     * \retval responseError
     * 		Response time out
     * \retval workerIdError
     * 		Wrong worker ID in the response
     * \retval responseTypeError
     * 		Wrong response Type
     */
    OperationResult
    sendRequestGetResponseData(uint8_t workerId,
                               uint8_t counter,
                               uint8_t type,
                               uint8_t expectedResponseType,
                               outpost::Slice<uint8_t> sendData,
                               outpost::Slice<uint8_t>& workerResponseData);

    struct ResponseData
    {
        uint16_t length;
        uint8_t workerId;
        uint8_t counter;
        uint8_t type;
        uint16_t payloadDataLength;
        std::array<uint8_t, outpost::sip::parameter::maxPayloadLength> payloadData;
    };

    /*
     * Send response data to the queue.
     *
     * \param data
     * 		Response data to be send to the queue
     *
     * \retval true
     * 		Success
     * \retval false
     * 		failed
     */
    inline bool
    sendResponseQueue(ResponseData data)
    {
        return mResponseQueue.send(data);
    }

private:
    outpost::sip::PacketTransport& mPacketTransport;
    std::array<uint8_t, outpost::sip::parameter::maxPacketLength> mBuffer;
    outpost::Slice<uint8_t> mBufferToWrite;

    // Parallel transaction are not allowed therefore for the queue is enough to hold one response.
    const static uint8_t sizeOfQueue = 1;
    outpost::rtos::Queue<ResponseData> mResponseQueue;

    inline bool
    receiveResponseQueue(ResponseData& data, outpost::time::Duration timeout)
    {
        return mResponseQueue.receive(data, timeout);
    }
};

}  // namespace sip
}  // namespace outpost
#endif /* OUTPOST_SIP_COORDINATOR_H_ */
