/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2020, Felix Passenberg
 * Copyright (c) 2020, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/hal/datagram_transport.h>

#include <unittest/harness.h>

using outpost::hal::DatagramTransport;

TEST(DatagramTransportTest, shouldAllowToComposeAddressConstants)
{
    constexpr DatagramTransport::Address address(DatagramTransport::IpAddress(127, 0, 0, 1), 8080);

    EXPECT_EQ(address.getPort(), 8080);

    std::array<uint8_t, 4> Ip = address.getIpAddress().getArray();
    std::array<uint8_t, 4> refIp = DatagramTransport::IpAddress(127, 0, 0, 1).getArray();
    for (size_t i = 0; i < refIp.size(); i++)
    {
        EXPECT_EQ(Ip[i], refIp[i]);
    }
}
