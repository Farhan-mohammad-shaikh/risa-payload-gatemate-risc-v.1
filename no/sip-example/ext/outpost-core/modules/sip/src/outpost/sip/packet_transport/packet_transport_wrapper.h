/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2023, Tobias Pfeffer
 * Copyright (c) 2023-2024, Felix Passenberg
 * Copyright (c) 2023, Janosch Reinking
 * Copyright (c) 2024, Pascal Pieper
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SIP_PACKET_TRANSPORT_WRAPPER_H
#define OUTPOST_SIP_PACKET_TRANSPORT_WRAPPER_H

#include <outpost/parameter/sip.h>
#include <outpost/sip/packet/packet_reader.h>
#include <outpost/sip/packet/packet_writer.h>
#include <outpost/sip/packet_transport/packet_transport.h>
#include <outpost/time/duration.h>
#include <outpost/transport/frame_transport/frame_transport.h>

namespace outpost
{
namespace sip
{
/**
 * Transmitting a packet through an underlying frame transport tx
 */
class PacketTransportTxWrapper : public virtual PacketTransportTx
{
public:
    /**
     * Creates the packet transport wrapper tx
     *
     * \param frameTransportTx
     *      Underlying frame transporter tx
     */
    explicit PacketTransportTxWrapper(transport::FrameTransportTx& frameTransportTx);

    virtual ~PacketTransportTxWrapper() = default;

    /**
     * Transmit a packet.
     *
     * \param packet
     *      Packet to be send
     *
     * \return Number of transmitted bytes (Including framing bytes!)
     *         See also base class \c PacketTransport
     *
     */
    Expected<size_t, OperationResult>
    transmit(PacketReader const& packet) override;

private:
    transport::FrameTransportTx& mFrameTransportTx;
};

/**
 * Receiving of a packet through an underlying frame transport rx
 */
class PacketTransportRxWrapper : public virtual PacketTransportRx
{
public:
    /**
     * Creates the packet transport wrapper rx
     *
     * \param frameTransportRx
     *      Underlying frame transporter rx
     */
    explicit PacketTransportRxWrapper(transport::FrameTransportRx& frameTransportRx);

    virtual ~PacketTransportRxWrapper() = default;

    /**
     * Receive a packet.
     *
     * This a synchronous operation, the function will only return after receiving
     * a packet or reaching a timeout.
     *
     * \param packet
     *      Packet receive
     * \param timeout
     *      Time out
     *
     * \return see base class \c PacketTransport
     *
     */
    Expected<PacketReader, OperationResult>
    receive(Slice<uint8_t> const& writeBuffer, time::Duration timeout) override;

private:
    transport::FrameTransportRx& mFrameTransportRx;
};

/**
 * Transmitting and receiving of a packet through an underlying frame transport
 *
 * Created to maintain compatability for SIP.
 */
class PacketTransportWrapper : public PacketTransport,
                               public PacketTransportTxWrapper,
                               public PacketTransportRxWrapper
{
public:
    /**
     * Creates the packet transport wrapper
     *
     * \param frameTransport
     *      Underlying frame transporter
     */
    explicit PacketTransportWrapper(transport::FrameTransport& frameTransport);
    PacketTransportWrapper(transport::FrameTransportTx& frameTransportTx,
                           transport::FrameTransportRx& frameTransportRx);

    using PacketTransportRxWrapper::receive;
    using PacketTransportTxWrapper::transmit;

    virtual ~PacketTransportWrapper() = default;
};

}  // namespace sip
}  // namespace outpost

#endif
