/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2023-2024, Felix Passenberg
 * Copyright (c) 2023, Jan-Gerd Mess
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SIP_PACKET_TRANSPORT_H
#define OUTPOST_SIP_PACKET_TRANSPORT_H

#include <outpost/base/slice.h>
#include <outpost/sip/operation_result.h>
#include <outpost/time/timeout.h>
#include <outpost/utils/expected.h>

namespace outpost
{
namespace sip
{
class PacketReader;

/**
 * Transmitting of a packet.
 *
 */
class PacketTransportTx
{
public:
    virtual ~PacketTransportTx() = default;

    /**
     * Transmit a packet.
     *
     * \param packet
     *      Packet to be send
     *
     * \return Expected
     * \retval true size_t
     *      Success Number of Bytes transmitted
     * \retval false OperationResult
     *      Failed
     * \retval other
     *      forwarded OpertionResult from \c PacketReader and \c PacketCoder.
     */
    virtual outpost::Expected<size_t, OperationResult>
    transmit(PacketReader const& packet) = 0;
};

class PacketTransportRx
{
public:
    virtual ~PacketTransportRx() = default;

    /**
     * Receive a packet.
     *
     * This is a synchronous operation, the function will only return after receiving
     * a packet or reaching the timeout.
     *
     * \param[in/out] packet
     *      Packet received into this Slice.
     * \param timeout
     *      max time for the operation.
     *
     * \return Expected
     * \retval success PacketReader
     *      A Packet Reader Containing the received Packet.
     * \retval false decodeError
     *      PacketCoder decode error
     * \retval false bufferError
     *      Buffer size error
     * \retval false timeOut
     *      Timeout occurred
     * \retval other
     *      forwarded OpertionResult from \c PacketReader and \c PacketWriter and \c PacketCoder.
     *
     */
    virtual outpost::Expected<PacketReader, OperationResult>
    receive(outpost::Slice<uint8_t> const& writeBuffer, outpost::time::Duration timeout) = 0;
};

class PacketTransport : public virtual PacketTransportTx, public virtual PacketTransportRx
{
public:
    virtual ~PacketTransport() = default;
};

}  // namespace sip
}  // namespace outpost
#endif
