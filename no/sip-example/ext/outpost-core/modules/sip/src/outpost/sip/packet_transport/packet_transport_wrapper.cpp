/*
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "packet_transport_wrapper.h"

namespace outpost
{
namespace sip
{

PacketTransportTxWrapper::PacketTransportTxWrapper(
        outpost::transport::FrameTransportTx& frameTransportTx) :
    mFrameTransportTx(frameTransportTx)
{
}

outpost::Expected<size_t, outpost::sip::OperationResult>
PacketTransportTxWrapper::transmit(PacketReader const& packet)
{
    Slice<const uint8_t> packetData = packet.getSlice();
    auto maybeTransmittedBytes = mFrameTransportTx.transmit(packetData);
    if (maybeTransmittedBytes)
    {
        return *maybeTransmittedBytes;
    }
    return outpost::unexpected(outpost::sip::OperationResult::transportError);
}

PacketTransportRxWrapper::PacketTransportRxWrapper(
        outpost::transport::FrameTransportRx& frameTransportRx) :
    mFrameTransportRx(frameTransportRx)
{
}

outpost::Expected<outpost::sip::PacketReader, outpost::sip::OperationResult>
PacketTransportRxWrapper::receive(outpost::Slice<uint8_t> const& writeBuffer,
                                  outpost::time::Duration timeout)
{
    auto maybeBytes = mFrameTransportRx.receive(writeBuffer, timeout);

    if (maybeBytes)
    {
        outpost::sip::PacketReader packetReader(*maybeBytes);
        auto const decodeResult = packetReader.readPacket();
        if (decodeResult == OperationResult::success)
        {
            return packetReader;
        }
        return outpost::unexpected(decodeResult);
    }
    else
    {
        return outpost::unexpected(outpost::sip::OperationResult::transportError);
    }
}

PacketTransportWrapper::PacketTransportWrapper(outpost::transport::FrameTransport& frameTransport) :
    PacketTransportWrapper::PacketTransportTxWrapper(frameTransport),
    PacketTransportWrapper::PacketTransportRxWrapper(frameTransport)
{
}

PacketTransportWrapper::PacketTransportWrapper(
        outpost::transport::FrameTransportTx& frameTransportTx,
        outpost::transport::FrameTransportRx& frameTransportRx) :
    PacketTransportWrapper::PacketTransportTxWrapper(frameTransportTx),
    PacketTransportWrapper::PacketTransportRxWrapper(frameTransportRx)
{
}

}  // namespace sip

}  // namespace outpost
