/*
 * Copyright (c) 2024, Felix Passenberg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/hal_posix/tcp_ip_socket.h>
#include <outpost/hal_posix/tcp_server.h>
#include <outpost/rtos/thread.h>
#include <outpost/time/duration.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unittest/harness.h>

namespace tcp_tests
{
using outpost::hal::DatagramTransport;
using outpost::posix::TcpIpSocket;
using outpost::posix::TcpServer;
using outpost::time::Duration;

class TcpConnectionTest : public ::testing::Test
{
public:
    TcpConnectionTest() : mTcpServer(mAddress1), mTcpIpSocket1(mAddress1), mTcpIpSocket2()
    {
    }

    void
    SetUp()
    {
        ASSERT_TRUE(mTcpServer.listen());
        outpost::rtos::Thread::sleep(
                outpost::time::Milliseconds(10));  // OS needs to do stuff, handshake etc.
        ASSERT_TRUE(mTcpIpSocket1.connect());
        outpost::rtos::Thread::sleep(
                outpost::time::Milliseconds(10));  // OS needs to do stuff, handshake etc.
        ASSERT_TRUE(mTcpServer.accept(mTcpIpSocket2));
        outpost::rtos::Thread::sleep(
                outpost::time::Milliseconds(10));  // OS needs to do stuff, handshake etc.
    };

    void
    TearDown()
    {
        mTcpIpSocket1.close();
        outpost::rtos::Thread::sleep(
                outpost::time::Milliseconds(10));  // OS needs to do stuff, handshake etc.
        mTcpIpSocket2.close();
        outpost::rtos::Thread::sleep(
                outpost::time::Milliseconds(10));  // OS needs to do stuff, handshake etc.
        mTcpServer.close();
        outpost::rtos::Thread::sleep(
                outpost::time::Milliseconds(10));  // OS needs to do stuff, handshake etc.
    };

    DatagramTransport::Address mAddress1 =
            DatagramTransport::Address(DatagramTransport::IpAddress(127, 0, 0, 1), 4200);

    TcpServer mTcpServer;
    TcpIpSocket mTcpIpSocket1;
    TcpIpSocket mTcpIpSocket2;
};

TEST_F(TcpConnectionTest, checkConnectingAndClosingWorks)
{
    // Check address is stored correctly
    EXPECT_EQ(mAddress1.getPort(), mTcpServer.getAddress().getPort());
    EXPECT_EQ(mAddress1.getIpAddress().getArray(),
              mTcpServer.getAddress().getIpAddress().getArray());
    EXPECT_TRUE(mTcpIpSocket1.getAddress().getPort());
    EXPECT_TRUE(mTcpIpSocket2.getAddress().getPort());

    EXPECT_NE(mTcpIpSocket2.getAddress().getPort(), 0);  // port of client shall be set
    EXPECT_NE(mTcpIpSocket2.getAddress().getPort(),
              mAddress1.getPort());  // port is not the port of the server
    EXPECT_EQ(mTcpIpSocket1.getAddress().getPort(), mAddress1.getPort());

    EXPECT_EQ(mTcpIpSocket1.getAddress().getIpAddress().getArray(),
              mAddress1.getIpAddress().getArray());
    EXPECT_EQ(mTcpIpSocket2.getAddress().getIpAddress().getArray(),
              mAddress1.getIpAddress().getArray());
}

TEST_F(TcpConnectionTest, checkInterfaceOnUnconnectedSocket)
{
    // Check that the return values are as expected for an
    // unconnected socket
    mTcpIpSocket1.close();
    outpost::rtos::Thread::sleep(
            outpost::time::Milliseconds(10));  // OS needs to do stuff, handshake etc.
    mTcpIpSocket2.close();

    uint8_t buffer[1];
    outpost::Slice<uint8_t> array(buffer);
    ASSERT_FALSE(mTcpIpSocket1.isAvailable().has_value());
    const auto sendBytes = mTcpIpSocket1.send(array, Duration::zero());
    ASSERT_FALSE(sendBytes);
    const auto recvBytes = mTcpIpSocket1.receive(array, Duration::zero());
    ASSERT_FALSE(recvBytes);
    EXPECT_EQ(0xffffu - 60 - 60, mTcpIpSocket1.getMaximumTransferSize());
}

TEST_F(TcpConnectionTest, allowsMultipleConnections)
{
    // not further connections waiting
    ASSERT_TRUE(mTcpServer.isAvailable().has_value());
    EXPECT_FALSE(*(mTcpServer.isAvailable()));
    outpost::rtos::Thread::sleep(
            outpost::time::Milliseconds(10));  // OS needs to do stuff, handshake etc.

    // create new client and server sockets
    TcpIpSocket newSocketC(mAddress1);
    TcpIpSocket newSocketS;
    outpost::rtos::Thread::sleep(
            outpost::time::Milliseconds(10));  // OS needs to do stuff, handshake etc.

    // still not waiting
    ASSERT_TRUE(mTcpServer.isAvailable().has_value());
    EXPECT_FALSE(*(mTcpServer.isAvailable()));
    outpost::rtos::Thread::sleep(
            outpost::time::Milliseconds(10));  // OS needs to do stuff, handshake etc.

    // connect client to server
    EXPECT_TRUE(newSocketC.connect());
    outpost::rtos::Thread::sleep(
            outpost::time::Milliseconds(10));  // OS needs to do stuff, handshake etc.

    // server shall see a client is waiting
    ASSERT_TRUE(mTcpServer.isAvailable().has_value());
    EXPECT_TRUE(*(mTcpServer.isAvailable()));
    outpost::rtos::Thread::sleep(
            outpost::time::Milliseconds(10));  // OS needs to do stuff, handshake etc.

    // accepting the waiting client to the new socket
    EXPECT_TRUE(mTcpServer.accept(newSocketS));
    outpost::rtos::Thread::sleep(
            outpost::time::Milliseconds(10));  // OS needs to do stuff, handshake etc.

    // no further clients are waiting
    ASSERT_TRUE(mTcpServer.isAvailable().has_value());
    EXPECT_FALSE(*(mTcpServer.isAvailable()));

    // std::cout << "1 " << mTcpIpSocket1.getAddress().getPort() << std::endl;
    // std::cout << "2 " << mTcpIpSocket2.getAddress().getPort() << std::endl;

    // std::cout << "cp " << newSocketC.getAddress().getPort() << std::endl;
    // std::cout << "sp " << newSocketS.getAddress().getPort() << std::endl;

    // send data from client should arrive only at one of the two server sockets
    std::array<uint8_t, 10> sendBuffer;
    outpost::Slice<uint8_t> sendData(sendBuffer);
    std::generate(sendBuffer.begin(), sendBuffer.end(), []() {
        static int i = 17;
        return ++i;
    });
    newSocketC.send(sendData);
    ASSERT_TRUE(mTcpIpSocket2.isAvailable().has_value());
    EXPECT_FALSE(*(mTcpIpSocket2.isAvailable()));
    ASSERT_TRUE(newSocketS.isAvailable().has_value());
    EXPECT_TRUE(*(newSocketS.isAvailable()));

    newSocketS.close();
    outpost::rtos::Thread::sleep(
            outpost::time::Milliseconds(10));  // OS needs to do stuff, handshake etc.
    newSocketC.close();
    outpost::rtos::Thread::sleep(
            outpost::time::Milliseconds(10));  // OS needs to do stuff, handshake etc.
}

TEST_F(TcpConnectionTest, connectFailsForUsedPort)
{
    // Opening a 2nd socket with the same port should fail
    // as only one socket can bind to a port
    TcpServer tcpServer2(mAddress1);
    EXPECT_FALSE(tcpServer2.listen());
    tcpServer2.close();
}

TEST_F(TcpConnectionTest, shouldTransmitClientToServer)
{
    constexpr size_t bufferSize = 256;
    std::array<uint8_t, bufferSize> sendBuffer;
    std::generate(sendBuffer.begin(), sendBuffer.end(), []() {
        static int i = 42;
        return ++i;
    });
    outpost::Slice<uint8_t> sendData(sendBuffer);
    std::array<uint8_t, bufferSize> receiveBuffer{{0}};
    outpost::Slice<uint8_t> receiveData(receiveBuffer);

    // Send message from 1 to 2
    const auto sentBytes = mTcpIpSocket1.send(sendData);
    ASSERT_TRUE(sentBytes);
    EXPECT_EQ(bufferSize, *sentBytes);
    outpost::rtos::Thread::sleep(outpost::time::Milliseconds(1));  // OS needs to do stuff
    const auto av = mTcpIpSocket2.isAvailable();
    ASSERT_TRUE(av.has_value());
    EXPECT_TRUE(*av);
    EXPECT_TRUE(*(mTcpIpSocket2.isAvailable()));
    const auto recvBytes = mTcpIpSocket2.receive(receiveData);
    ASSERT_TRUE(recvBytes);
    EXPECT_EQ(bufferSize, recvBytes->getNumberOfElements());
    for (size_t i = 0; i < bufferSize; i++)
    {
        // std::cout << i << " " << (*recvBytes)[i] << std::endl;
        EXPECT_EQ((*recvBytes)[i], sendData[i]);
    }
}

TEST_F(TcpConnectionTest, shouldTransmitServerToClient)
{
    constexpr size_t bufferSize = 256;
    std::array<uint8_t, bufferSize> sendBuffer;
    std::generate(sendBuffer.begin(), sendBuffer.end(), []() {
        static int i;
        return ++i;
    });
    outpost::Slice<uint8_t> sendData(sendBuffer);
    std::array<uint8_t, bufferSize> receiveBuffer{{0}};
    outpost::Slice<uint8_t> receiveData(receiveBuffer);

    // Send message from 2 to 1
    const auto sentBytes = mTcpIpSocket1.send(sendData);
    ASSERT_TRUE(sentBytes);
    EXPECT_EQ(bufferSize, *sentBytes);
    outpost::rtos::Thread::sleep(outpost::time::Milliseconds(10));  // OS needs to do stuff
    ASSERT_TRUE(mTcpIpSocket2.isAvailable().has_value());
    EXPECT_TRUE(*(mTcpIpSocket2.isAvailable()));
    const auto recvBytes = mTcpIpSocket2.receive(receiveData);
    ASSERT_TRUE(recvBytes);
    EXPECT_EQ(bufferSize, recvBytes->getNumberOfElements());
    for (size_t i = 0; i < bufferSize; i++)
    {
        EXPECT_EQ((*recvBytes)[i], sendData[i]);
    }
}

TEST_F(TcpConnectionTest, shouldFailBecausePacketTooLarge)
{
    constexpr size_t bufferSize = 90000;
    std::array<uint8_t, bufferSize> sendBuffer{0};
    std::generate(sendBuffer.begin(), sendBuffer.end(), []() {
        static int i;
        return ++i;
    });

    std::array<uint8_t, bufferSize> recvBuffer{0};
    outpost::Slice<uint8_t> sendData(sendBuffer);
    outpost::Slice<uint8_t> receiveData(recvBuffer);

    const auto sentBytes = mTcpIpSocket2.send(sendData);
    ASSERT_TRUE(sentBytes);
    EXPECT_EQ(*sentBytes, bufferSize);
    outpost::rtos::Thread::sleep(outpost::time::Milliseconds(10));  // OS needs to do stuff

    ASSERT_TRUE(mTcpIpSocket1.isAvailable().has_value());
    EXPECT_TRUE(*(mTcpIpSocket1.isAvailable()));
    const auto recvBytes = mTcpIpSocket1.receive(receiveData);
    outpost::rtos::Thread::sleep(outpost::time::Milliseconds(10));  // OS needs to do stuff

    ASSERT_TRUE(recvBytes);
    EXPECT_NE(bufferSize, recvBytes->getNumberOfElements());
    // partial packet should match
    for (size_t i = 0; i < mTcpIpSocket1.maxIpPacketSize; i++)
    {
        EXPECT_EQ(receiveData[i], sendData[i]);
    }
}

TEST_F(TcpConnectionTest, shouldClearReceiveBuffer)
{
    constexpr size_t bufferSize = 256;
    std::array<uint8_t, bufferSize> sendBuffer;
    outpost::Slice<uint8_t> sendData(sendBuffer);

    auto sentBytes = mTcpIpSocket1.send(sendData);
    ASSERT_TRUE(sentBytes);
    EXPECT_EQ(bufferSize, *sentBytes);
    sentBytes = mTcpIpSocket1.send(sendData);
    ASSERT_TRUE(sentBytes);
    EXPECT_EQ(bufferSize, *sentBytes);
    sentBytes = mTcpIpSocket1.send(sendData);
    ASSERT_TRUE(sentBytes);
    EXPECT_EQ(bufferSize, *sentBytes);

    // Should have data available
    ASSERT_TRUE(mTcpIpSocket2.isAvailable().has_value());
    EXPECT_TRUE(*(mTcpIpSocket2.isAvailable()));
    mTcpIpSocket2.clearReceiveBuffer();
    // All available data should be discarded
    ASSERT_TRUE(mTcpIpSocket2.isAvailable().has_value());
    EXPECT_FALSE(*(mTcpIpSocket2.isAvailable()));
}

TEST_F(TcpConnectionTest, canNotTransmitZeroLengthData)
{
    outpost::Slice<uint8_t> sendData(outpost::Slice<uint8_t>::empty());

    // Send message from 1 to 2
    const auto sentBytes = mTcpIpSocket1.send(sendData);
    ASSERT_TRUE(sentBytes);
    EXPECT_EQ(0, *sentBytes);
    ASSERT_TRUE(mTcpIpSocket2.isAvailable().has_value());
    EXPECT_FALSE(*(mTcpServer.isAvailable()));
}

TEST_F(TcpConnectionTest, shouldTransmitInTwoFrames)
{
    constexpr size_t bufferSize = 256;
    std::array<uint8_t, bufferSize> sendBuffer;
    std::generate(sendBuffer.begin(), sendBuffer.end(), []() {
        static int i = 10;
        return ++i;
    });
    outpost::Slice<uint8_t> sendData(sendBuffer);
    std::array<uint8_t, bufferSize> receiveBuffer{{0}};
    outpost::Slice<uint8_t> receiveData(receiveBuffer);

    // Send message from 1 to 2
    auto sentBytes = mTcpIpSocket1.send(sendData.first(10));
    ASSERT_TRUE(sentBytes);
    EXPECT_EQ(10, *sentBytes);
    // EXPECT_EQ(10U, *(mTcpIpSocket2.getNumberOfBytesAvailable())); // deprecated
    sentBytes = mTcpIpSocket1.send(sendData.skipFirst(10));
    ASSERT_TRUE(sentBytes);
    EXPECT_EQ(bufferSize - 10, *sentBytes);
    // EXPECT_EQ(256U, *(mTcpIpSocket2.getNumberOfBytesAvailable())); // deprecated
    outpost::rtos::Thread::sleep(outpost::time::Milliseconds(1));  // OS needs to do stuff
    const auto av = mTcpIpSocket2.isAvailable();
    ASSERT_TRUE(av.has_value());
    EXPECT_TRUE(*av);

    auto recvBytes = mTcpIpSocket2.receive(receiveData);
    ASSERT_TRUE(recvBytes);

    //  this tests are disabled, as it is not a datagram transport. The TCP stack may combine those.
    // EXPECT_EQ(10, recvBytes->getNumberOfElements());
    // recvBytes = mTcpIpSocket2.receive(receiveData, outpost::time::Seconds(1));
    // ASSERT_TRUE(recvBytes);
    // EXPECT_EQ(bufferSize-10, recvBytes->getNumberOfElements());
}

}  // namespace tcp_tests
