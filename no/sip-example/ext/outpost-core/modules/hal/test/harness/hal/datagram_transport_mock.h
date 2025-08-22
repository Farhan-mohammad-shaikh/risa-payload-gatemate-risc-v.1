/*
 * Copyright (c) 2024, Reinking, Janosch
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef HARNESS_HAL_DATAGRAM_TRANSPORT_MOCK_H
#define HARNESS_HAL_DATAGRAM_TRANSPORT_MOCK_H

#include <outpost/hal/datagram_transport.h>

#include <gtest/gmock.h>
#include <gtest/gtest.h>

namespace harness
{
namespace hal
{

/**
 * \class DatagramTransportMock
 */
class DatagramTransportMock : public outpost::hal::DatagramTransport
{
public:
    DatagramTransportMock()
    {
    }

    MOCK_METHOD(bool, connect, (), (override));
    MOCK_METHOD(void, close, (), (override));
    MOCK_METHOD(DatagramTransport::Address, getAddress, (), (const, override));
    MOCK_METHOD(MaybeBool, isAvailable, (), (override));
    MOCK_METHOD(MaybeSize, getNumberOfBytesAvailable, (), (override));
    MOCK_METHOD(DataErrorCode, joinMulticastGroup, (const IpAddress&), (override));

    MOCK_METHOD((outpost::Expected<size_t, DataErrorCode>),
                sendTo,
                (outpost::Slice<const uint8_t> data,
                 const DatagramTransport::Address& address,
                 outpost::time::Duration timeout),
                (override));

    MOCK_METHOD(MaybeSlice,
                receiveFrom,
                (const outpost::Slice<uint8_t>& data,
                 DatagramTransport::Address& address,
                 outpost::time::Duration timeout),
                (override));

    MOCK_METHOD(void, setAddress, (const Address& newAddress), (override));

    MOCK_METHOD(size_t, getMaximumDatagramSize, (), (const, override));

    MOCK_METHOD(void, clearReceiveBuffer, (), (override));
};

}  // namespace hal
}  // namespace harness

#endif  // HARNESS_HAL_DATAGRAM_TRANSPORT_MOCK_H
