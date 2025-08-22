/*
 * Copyright (c) 2020, Moyano, Gabriel
 * Copyright (c) 2021, Jan-Gerd Mess
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2023, Pieper, Pascal
 * Copyright (c) 2023, Mess, Jan-Gerd
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2024, Christof Efkemann
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <fcntl.h>
#include <pty.h>
#include <termios.h>
#include <unistd.h>

#include <outpost/hal_posix/termios_rawport.h>
#include <outpost/rtos/clock.h>
#include <outpost/rtos/thread.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>

using ::testing::_;
using ::testing::Return;

class TermiosPortTestFixture : public ::testing::Test
{
public:
    TermiosPortTestFixture() : mDataSlice(mBuffer), mTxStr("Hello from the other side")
    {
    }

    void
    SetUp()
    {
        int fd;
        openpty(&mPartnerFd, &fd, mPortPath, NULL, NULL);
    }

    void
    TearDown()
    {
        close(mPartnerFd);
    }

    outpost::rtos::SystemClock mSystemClock;
    char mPortPath[30];
    int mPartnerFd;
    std::array<uint8_t, 100> mBuffer;
    outpost::Slice<uint8_t> mDataSlice;
    std::string mTxStr;
};

TEST_F(TermiosPortTestFixture, shouldReturnFalseOpeningFalseDevice)
{
    const char devicePath[] = "/path/to/false/dev";
    outpost::posix::TermiosRawPort port(mSystemClock);

    EXPECT_FALSE(port.open(outpost::asSlice<>(devicePath), 115200));
}

TEST_F(TermiosPortTestFixture, shouldReturnFalseOpeningEmptyPath)
{
    auto devicePath = outpost::Slice<char>::empty();
    outpost::posix::TermiosRawPort port(mSystemClock);

    EXPECT_FALSE(port.open(outpost::asSlice<>(devicePath), 115200));
}

TEST_F(TermiosPortTestFixture, shouldsetCorrectBaudrates)
{
    struct TestingTermionRawPort : public outpost::posix::TermiosRawPort
    {
        explicit TestingTermionRawPort(outpost::time::Clock& clock) : TermiosRawPort(clock){};

        int
        getTerminalFiledescriptor()
        {
            return mFd;
        }
    };
    TestingTermionRawPort port(mSystemClock);
    struct termios term;

    EXPECT_TRUE(port.open(outpost::asSlice<>(mPortPath), 9600));
    ASSERT_GE(port.getTerminalFiledescriptor(), 0);

    ASSERT_EQ(tcgetattr(port.getTerminalFiledescriptor(), &term), 0);
    EXPECT_EQ(cfgetispeed(&term), static_cast<speed_t>(B9600));
    EXPECT_EQ(cfgetospeed(&term), static_cast<speed_t>(B9600));

    port.close();

    EXPECT_TRUE(port.open(outpost::asSlice<>(mPortPath), 115200));
    ASSERT_GE(port.getTerminalFiledescriptor(), 0);

    ASSERT_EQ(tcgetattr(port.getTerminalFiledescriptor(), &term), 0);
    EXPECT_EQ(cfgetispeed(&term), static_cast<speed_t>(B115200));
    EXPECT_EQ(cfgetospeed(&term), static_cast<speed_t>(B115200));

    port.close();

    EXPECT_TRUE(port.open(outpost::asSlice<>(mPortPath), 921600));
    ASSERT_GE(port.getTerminalFiledescriptor(), 0);

    ASSERT_EQ(tcgetattr(port.getTerminalFiledescriptor(), &term), 0);
    EXPECT_EQ(cfgetispeed(&term), static_cast<speed_t>(B921600));
    EXPECT_EQ(cfgetospeed(&term), static_cast<speed_t>(B921600));

    port.close();

    // non-standard baudrate
    EXPECT_TRUE(port.open(outpost::asSlice<>(mPortPath), 375000));
}

TEST_F(TermiosPortTestFixture, shouldReceiveDataCorrectly)
{
    outpost::posix::TermiosRawPort port(mSystemClock);

    ASSERT_TRUE(port.open(outpost::asSlice<>(mPortPath), 115200));

    // Send data
    write(mPartnerFd, mTxStr.c_str(), mTxStr.size());

    // Wait data
    outpost::time::Milliseconds timeToWait(100);
    outpost::rtos::Thread::sleep(timeToWait);

    // Amount of received bytes
    ASSERT_EQ(mTxStr.length(), port.getNumberOfBytesAvailable());

    // Data Reception
    const auto len = port.read(mDataSlice);
    ASSERT_TRUE(len);
    std::string rxStr(mBuffer.begin(), mBuffer.begin() + *len);
    EXPECT_EQ(0, rxStr.compare(mTxStr));
}

TEST_F(TermiosPortTestFixture, shouldReceiveDataButReturnNothing)
{
    outpost::posix::TermiosRawPort port(mSystemClock);

    ASSERT_TRUE(port.open(outpost::asSlice<>(mPortPath), 115200));

    // Send data
    write(mPartnerFd, mTxStr.c_str(), mTxStr.size());

    // Wait data
    outpost::time::Milliseconds timeToWait(100);
    outpost::rtos::Thread::sleep(timeToWait);

    // Amount of received bytes
    ASSERT_EQ(mTxStr.length(), port.getNumberOfBytesAvailable());

    // Data Reception
    auto dataSlice = outpost::Slice<uint8_t>::empty();
    const auto len = port.read(dataSlice);
    EXPECT_FALSE(len);
}

TEST_F(TermiosPortTestFixture, shouldFlushRxBuffer)
{
    outpost::posix::TermiosRawPort port(mSystemClock);

    ASSERT_TRUE(port.open(outpost::asSlice<>(mPortPath), 115200));

    // Send data
    write(mPartnerFd, mTxStr.c_str(), mTxStr.size());

    // Wait data
    outpost::time::Milliseconds timeToWait(100);
    outpost::rtos::Thread::sleep(timeToWait);

    // Amount of received bytes
    ASSERT_EQ(mTxStr.length(), port.getNumberOfBytesAvailable());

    // Flushing
    port.flushReceiver();
    EXPECT_EQ(static_cast<size_t>(0), port.getNumberOfBytesAvailable());
}

TEST_F(TermiosPortTestFixture, receptionWithTimeoutSuccess)
{
    outpost::time::Milliseconds rxTimeout(100);
    outpost::posix::TermiosRawPort port(mSystemClock);

    ASSERT_TRUE(port.open(outpost::asSlice<>(mPortPath), 115200));

    // Send data
    write(mPartnerFd, mTxStr.c_str(), mTxStr.size());

    // Data Reception
    const auto len = port.read(mDataSlice, rxTimeout);
    ASSERT_TRUE(len);
    std::string rxStr(mBuffer.begin(), mBuffer.begin() + *len);
    EXPECT_EQ(0, rxStr.compare(mTxStr));
}

TEST_F(TermiosPortTestFixture, receptionWithTimeoutFailure)
{
    outpost::time::Milliseconds rxTimeout(100);
    outpost::time::Milliseconds tolerance(10);
    outpost::posix::TermiosRawPort port(mSystemClock);

    ASSERT_TRUE(port.open(outpost::asSlice<>(mPortPath), 115200));

    // Data Reception
    auto t0 = mSystemClock.now();
    const auto ret = port.read(mDataSlice, rxTimeout);
    ASSERT_TRUE(ret);
    EXPECT_EQ(*ret, 0U);
    auto t1 = mSystemClock.now();
    auto txT = t1.timeSinceEpoch() - t0.timeSinceEpoch();
    EXPECT_GE(txT.milliseconds() + tolerance.milliseconds(), rxTimeout.milliseconds());
}

TEST_F(TermiosPortTestFixture, shouldTransmitDataCorrectly)
{
    outpost::posix::TermiosRawPort port(mSystemClock);

    ASSERT_TRUE(port.open(outpost::asSlice<>(mPortPath), 115200));

    std::copy(mTxStr.begin(), mTxStr.end(), mBuffer.data());
    outpost::Slice<uint8_t> dataSlice(mBuffer.begin(), mBuffer.begin() + mTxStr.size());
    const auto ret = port.write(dataSlice);
    ASSERT_TRUE(ret);
    EXPECT_EQ(*ret, dataSlice.getNumberOfElements());

    // Data Reception
    char rxBuffer[100];
    int len = read(mPartnerFd, rxBuffer, sizeof(rxBuffer) - 1);
    std::string rxStr(rxBuffer, len);
    EXPECT_EQ(0, rxStr.compare(mTxStr));
}

TEST_F(TermiosPortTestFixture, shouldTransmitLessDataThenDesiredBecauseTimeout)
{
    outpost::time::Microseconds txTimeout(10);
    outpost::posix::TermiosRawPort port(mSystemClock);

    ASSERT_TRUE(port.open(outpost::asSlice<>(mPortPath), 115200));

    std::copy(mTxStr.begin(), mTxStr.end(), mBuffer.data());
    outpost::Slice<uint8_t> dataSlice(mBuffer.begin(), mBuffer.begin() + mTxStr.size());
    port.write(dataSlice, txTimeout);

    // Data Reception
    char rxBuffer[100];
    int len = read(mPartnerFd, rxBuffer, sizeof(rxBuffer) - 1);
    std::string rxStr(rxBuffer, len);
    EXPECT_LT(rxStr.length(), mTxStr.length());
}

TEST_F(TermiosPortTestFixture, shouldTransmitNothing)
{
    outpost::posix::TermiosRawPort port(mSystemClock);

    ASSERT_TRUE(port.open(outpost::asSlice<>(mPortPath), 115200));

    auto dataSlice = outpost::Slice<uint8_t>::empty();
    const auto len = port.write(dataSlice);
    ASSERT_TRUE(len);
    EXPECT_EQ(*len, 0U);
}
