/*
 * Copyright (c) 2017, Jan Sommer
 * Copyright (c) 2017-2018, Fabian Greif
 * Copyright (c) 2021, Jan-Gerd Mess
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Felix Passenberg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/hal_posix/udp_socket.h>
#include <outpost/rtos/thread.h>

#include <unittest/harness.h>

namespace udp_tests
{
using outpost::hal::DatagramTransport;
using outpost::posix::UdpSocket;
using outpost::time::Duration;
using namespace ::testing;

TEST(SocketConnectionTest, checkConnectingAndClosingWorks)
{
    constexpr DatagramTransport::Address address(DatagramTransport::IpAddress(127, 0, 0, 1), 44555);
    UdpSocket udpSocket(address);
    // Check address is stored correctly
    EXPECT_EQ(address.getPort(), udpSocket.getAddress().getPort());
    EXPECT_EQ(address.getIpAddress().getArray(), udpSocket.getAddress().getIpAddress().getArray());
    EXPECT_TRUE(udpSocket.connect());
    udpSocket.close();
}

TEST(SocketConnectionTest, checkInterfaceOnUnconnectedSocket)
{
    // Check that the return values are as expected for an
    // unconnected socket
    constexpr DatagramTransport::Address address(DatagramTransport::IpAddress(127, 0, 0, 1), 44556);
    DatagramTransport::Address tmp(DatagramTransport::IpAddress(127, 0, 0, 1), 44557);
    UdpSocket udpSocket(address);
    uint8_t buffer[1];
    outpost::Slice<uint8_t> array(buffer);
    EXPECT_FALSE(udpSocket.isAvailable().has_value());
    EXPECT_FALSE(udpSocket.getNumberOfBytesAvailable().has_value());
    const auto sendBytes = udpSocket.sendTo(array, tmp, Duration::zero());
    ASSERT_FALSE(sendBytes);
    const auto recvBytes = udpSocket.receiveFrom(array, tmp, Duration::zero());
    ASSERT_FALSE(recvBytes);
    EXPECT_EQ(65467U, udpSocket.getMaximumDatagramSize());
}

TEST(SocketConnectionTest, connectFailsForUsedPort)
{
    constexpr DatagramTransport::Address address(DatagramTransport::IpAddress(127, 0, 0, 1), 44558);
    UdpSocket udpSocket(address);
    EXPECT_TRUE(udpSocket.connect());

    // Opening a 2nd socket with the same port should fail
    // as only one socket can bind to a port
    UdpSocket failSocket(address);
    EXPECT_FALSE(failSocket.connect());
    udpSocket.close();
}

TEST(SocketConnectionTest, shouldSendDataToItself)
{
    constexpr size_t bufferSize = 256;
    uint8_t sendBuffer[bufferSize];
    uint8_t receiveBuffer[bufferSize];
    outpost::Slice<uint8_t> sendData(sendBuffer);
    outpost::Slice<uint8_t> receiveData(receiveBuffer);
    for (size_t i = 0; i < bufferSize; ++i)
    {
        sendBuffer[i] = i;
        receiveBuffer[i] = 0;
    }

    DatagramTransport::Address address(DatagramTransport::IpAddress(127, 0, 0, 1), 44559);
    UdpSocket udpSocket(address);
    EXPECT_TRUE(udpSocket.connect());
    EXPECT_FALSE(*udpSocket.isAvailable());
    const auto sentBytes = udpSocket.sendTo(sendData, address);
    ASSERT_TRUE(sentBytes);
    EXPECT_EQ(bufferSize, *sentBytes);
    EXPECT_TRUE(*udpSocket.isAvailable());
    ASSERT_EQ(bufferSize, *udpSocket.getNumberOfBytesAvailable());
    const auto recvBytes = udpSocket.receiveFrom(receiveData, address, outpost::time::Seconds(10));
    ASSERT_TRUE(recvBytes);
    EXPECT_EQ(bufferSize, recvBytes->getNumberOfElements());
    EXPECT_EQ(0, memcmp(receiveBuffer, sendBuffer, bufferSize));
    udpSocket.close();
}

TEST(SocketConnectionTest, shouldFailBecausePacketTooLarge)
{
    constexpr size_t bufferSize = 70000;
    uint8_t sendBuffer[bufferSize];
    DatagramTransport::Address address(DatagramTransport::IpAddress(127, 0, 0, 1), 44560);
    outpost::Slice<uint8_t> sendData(sendBuffer);
    UdpSocket udpSocket(address);
    EXPECT_TRUE(udpSocket.connect());
    const auto sentBytes = udpSocket.sendTo(sendData, address);
    ASSERT_FALSE(sentBytes);
    EXPECT_EQ(sentBytes.error(), DatagramTransport::ErrorCode::permanent);
}

TEST(SocketConnectionTest, shouldClearReceiveBuffer)
{
    constexpr size_t bufferSize = 256;
    uint8_t sendBuffer[bufferSize];
    DatagramTransport::Address address(DatagramTransport::IpAddress(127, 0, 0, 1), 44561);
    outpost::Slice<uint8_t> sendData(sendBuffer);
    UdpSocket udpSocket(address);
    EXPECT_TRUE(udpSocket.connect());
    auto sentBytes = udpSocket.sendTo(sendData, address);
    ASSERT_TRUE(sentBytes);
    EXPECT_EQ(bufferSize, *sentBytes);
    sentBytes = udpSocket.sendTo(sendData, address);
    ASSERT_TRUE(sentBytes);
    EXPECT_EQ(bufferSize, *sentBytes);
    sentBytes = udpSocket.sendTo(sendData, address);
    ASSERT_TRUE(sentBytes);
    EXPECT_EQ(bufferSize, *sentBytes);

    // Should have data available
    EXPECT_TRUE(*udpSocket.isAvailable());
    udpSocket.clearReceiveBuffer();
    // All available data should be discarded
    EXPECT_FALSE(*udpSocket.isAvailable());
}

TEST(SocketConnectionTest, shouldTransmitDataBetweenTwoSockets)
{
    constexpr size_t bufferSize = 256;
    std::array<uint8_t, bufferSize> sendBuffer;
    std::generate(sendBuffer.begin(), sendBuffer.end(), []() {
        static int i;
        return ++i;
    });
    outpost::Slice<uint8_t> sendData(sendBuffer);
    uint8_t receiveBuffer[bufferSize];
    outpost::Slice<uint8_t> receiveData(receiveBuffer);
    DatagramTransport::Address address1(DatagramTransport::IpAddress(127, 0, 0, 1), 44562);
    DatagramTransport::Address address2(DatagramTransport::IpAddress(127, 0, 0, 1), 44563);
    UdpSocket udp1(address1);
    UdpSocket udp2(address2);
    EXPECT_TRUE(udp1.connect());
    EXPECT_TRUE(udp2.connect());

    // Send message from 1 to 2
    const auto sentBytes = udp1.sendTo(sendData, address2);
    ASSERT_TRUE(sentBytes);
    EXPECT_EQ(bufferSize, *sentBytes);
    EXPECT_TRUE(*udp2.isAvailable());
    DatagramTransport::Address remote;
    const auto recvBytes = udp2.receiveFrom(receiveData, remote);
    ASSERT_TRUE(recvBytes);
    EXPECT_EQ(bufferSize, recvBytes->getNumberOfElements());
    for (size_t i = 0; i < bufferSize; i++)
    {
        EXPECT_EQ(receiveData[i], sendData[i]);
    }
    EXPECT_EQ(address1.getPort(), remote.getPort());
    EXPECT_EQ(address1.getIpAddress().getArray(), address2.getIpAddress().getArray());
}

TEST(SocketConnectionTest, shouldReturnSizeOfNextPacketsEvenIfTwoAreAvailable)
{
    constexpr size_t bufferSize = 256;
    uint8_t sendBuffer[bufferSize];
    outpost::Slice<uint8_t> sendData(sendBuffer);
    uint8_t receiveBuffer[bufferSize];
    outpost::Slice<uint8_t> receiveData(receiveBuffer);

    DatagramTransport::Address address1(DatagramTransport::IpAddress(127, 0, 0, 1), 44562);
    UdpSocket udp1(address1);
    EXPECT_TRUE(udp1.connect());

    DatagramTransport::Address address2(DatagramTransport::IpAddress(127, 0, 0, 1), 44563);
    UdpSocket udp2(address2);
    EXPECT_TRUE(udp2.connect());

    // send number 1
    {
        const auto sentBytes = udp1.sendTo(sendData, address2);
        ASSERT_TRUE(sentBytes);
        EXPECT_EQ(bufferSize, *sentBytes);
    }

    EXPECT_TRUE(*udp2.isAvailable());
    EXPECT_EQ(*udp2.getNumberOfBytesAvailable(), bufferSize);

    // send number 2
    {
        const auto sentBytes = udp1.sendTo(sendData, address2);
        ASSERT_TRUE(sentBytes);
        EXPECT_EQ(bufferSize, *sentBytes);
    }

    outpost::rtos::Thread::sleep(outpost::time::Milliseconds(100));

    EXPECT_TRUE(*udp2.isAvailable());
    EXPECT_EQ(*udp2.getNumberOfBytesAvailable(), bufferSize);
}

TEST(SocketConnectionTest, shouldBeAvailableIfEmptyPacketWasReceived)
{
    constexpr size_t bufferSize = 256;
    uint8_t sendBuffer[bufferSize];
    outpost::Slice<uint8_t> sendData(sendBuffer);
    uint8_t receiveBuffer[bufferSize];
    outpost::Slice<uint8_t> receiveData(receiveBuffer);

    DatagramTransport::Address address1(DatagramTransport::IpAddress(127, 0, 0, 1), 44562);
    UdpSocket udp1(address1);
    EXPECT_TRUE(udp1.connect());

    DatagramTransport::Address address2(DatagramTransport::IpAddress(127, 0, 0, 1), 44563);
    UdpSocket udp2(address2);
    EXPECT_TRUE(udp2.connect());

    // send number 1
    {
        const auto sentBytes = udp1.sendTo(outpost::Slice<const uint8_t>::empty(), address2);
        ASSERT_TRUE(sentBytes);
        EXPECT_EQ(0U, *sentBytes);
    }

    EXPECT_TRUE(*udp2.isAvailable());
    EXPECT_EQ(*udp2.getNumberOfBytesAvailable(), 0);

    // send number 2
    {
        const auto sentBytes = udp1.sendTo(sendData, address2);
        ASSERT_TRUE(sentBytes);
        EXPECT_EQ(bufferSize, *sentBytes);
    }

    outpost::rtos::Thread::sleep(outpost::time::Milliseconds(100));

    EXPECT_TRUE(*udp2.isAvailable());
    EXPECT_EQ(*udp2.getNumberOfBytesAvailable(), 0);

    // receive 1
    {
        outpost::hal::DatagramTransport::Address address;
        auto packet = udp2.receiveFrom(outpost::asSlice(receiveBuffer), address);
        ASSERT_THAT(packet, IsTrue());
        ASSERT_THAT(packet->getNumberOfElements(), Eq(0U));
    }

    EXPECT_TRUE(*udp2.isAvailable());
    EXPECT_EQ(*udp2.getNumberOfBytesAvailable(), bufferSize);

    // receive 2
    {
        outpost::hal::DatagramTransport::Address address;
        auto packet = udp2.receiveFrom(outpost::asSlice(receiveBuffer), address);
        ASSERT_THAT(packet, IsTrue());
        ASSERT_THAT(packet->getNumberOfElements(), Eq(bufferSize));
    }

    EXPECT_FALSE(*udp2.isAvailable());
    EXPECT_EQ(*udp2.getNumberOfBytesAvailable(), 0U);
}
}  // namespace udp_tests
