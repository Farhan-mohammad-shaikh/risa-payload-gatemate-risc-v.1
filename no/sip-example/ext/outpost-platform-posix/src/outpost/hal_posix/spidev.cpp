/*
 * Copyright (c) 2023, Jan Sommer
 * Copyright (c) 2023, Meß, Jan-Gerd
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "spidev.h"

#include <fcntl.h>
#include <sys/ioctl.h>

using namespace outpost;
using namespace outpost::posix;

Spidev::Spidev()
{
    mIsOpen = false;
    mCsMax = 0;
}

Spidev::~Spidev()
{
    close();
}

Spidev::ReturnCode
Spidev::open(outpost::Slice<const char> deviceName, Cfg cfg)
{
    if (mIsOpen)
    {
        return ReturnCode::alreadyOpened;
    }

    if (deviceName.begin() == nullptr)
    {
        return ReturnCode::deviceNameInvalid;
    }

    mFd = ::open(deviceName.begin(), O_RDWR);
    if (mFd < 0)
    {
        return ReturnCode::couldNotOpen;
    }

    configureMessage(mMsg, cfg);

    // set the lsb setting
    if (ioctl(mFd, SPI_IOC_WR_LSB_FIRST, &cfg.lsbFirst) < 0)
    {
        return ReturnCode::couldNotSetLsbSetting;
    }

    // set the bits per word setting
    if (ioctl(mFd, SPI_IOC_WR_BITS_PER_WORD, &mMsg.bits_per_word) < 0)
    {
        return ReturnCode::couldNotSetBitsPerWordSetting;
    }

    // set the max speed setting
    if (ioctl(mFd, SPI_IOC_WR_MAX_SPEED_HZ, &cfg.speed) < 0)
    {
        return ReturnCode::couldNotSetMaxSpeedSetting;
    }

    // set the full 32-bit default mode
    if (ioctl(mFd, SPI_IOC_WR_MODE32, &mMsg.mode) < 0)
    {
        return ReturnCode::couldNotSetMode;
    }

    mCsMax = cfg.csMax;
    mIsOpen = true;

    return ReturnCode::success;
}

void
Spidev::close()
{
    if (mIsOpen && ::close(mFd) >= 0)
    {
        mIsOpen = false;
    }
}

hal::Spi::ReturnCode
Spidev::read(outpost::Slice<uint8_t> data, uint8_t cs)
{
    using Rc = hal::Spi::ReturnCode;

    if (!mIsOpen)
    {
        return Rc::notOpen;
    }

    if (data.begin() == nullptr)
    {
        return Rc::dataInvalid;
    }

    if (cs > mCsMax)
    {
        return Rc::chipSelectInvalid;
    }

    if (cs == hal::Spi::noChipSelect)
    {
        mMsg.cs = 0;
    }
    else
    {
        mMsg.cs = cs;
    }

    mMsg.rx_buf = static_cast<void*>(data.begin());
    mMsg.tx_buf = nullptr;
    mMsg.len = data.getNumberOfElements();

    if (ioctl(mFd, SPI_IOC_MESSAGE(1), &mMsg) < 0)
    {
        return Rc::dataInvalid;
    }

    return Rc::success;
}

hal::Spi::ReturnCode
Spidev::write(outpost::Slice<const uint8_t> data, uint8_t cs)
{
    using Rc = hal::Spi::ReturnCode;

    if (!mIsOpen)
    {
        return Rc::notOpen;
    }

    if (data.begin() == nullptr)
    {
        return Rc::dataInvalid;
    }

    if (cs > mCsMax)
    {
        return Rc::chipSelectInvalid;
    }

    if (cs == hal::Spi::noChipSelect)
    {
        mMsg.cs = 0;
    }
    else
    {
        mMsg.cs = cs;
    }

    mMsg.rx_buf = nullptr;
    mMsg.tx_buf = static_cast<const void*>(data.begin());
    mMsg.len = data.getNumberOfElements();

    if (ioctl(mFd, SPI_IOC_MESSAGE(1), &mMsg) < 0)
    {
        return Rc::dataInvalid;
    }

    return Rc::success;
}

hal::Spi::ReturnCode
Spidev::transfer(outpost::Slice<const uint8_t> outData, outpost::Slice<uint8_t> inData, uint8_t cs)
{
    using Rc = hal::Spi::ReturnCode;

    if (!mIsOpen)
    {
        return Rc::notOpen;
    }

    if (outData.begin() == nullptr || inData.begin() == nullptr)
    {
        return Rc::dataInvalid;
    }

    if (outData.getNumberOfElements() != inData.getNumberOfElements())
    {
        return Rc::inAndOutDataHaveDifferentSize;
    }

    if (cs > mCsMax)
    {
        return Rc::chipSelectInvalid;
    }

    if (cs == hal::Spi::noChipSelect)
    {
        mMsg.cs = 0;
    }
    else
    {
        mMsg.cs = cs;
    }

    mMsg.rx_buf = static_cast<void*>(inData.begin());
    mMsg.tx_buf = static_cast<const void*>(outData.begin());
    mMsg.len = outData.getNumberOfElements();

    if (ioctl(mFd, SPI_IOC_MESSAGE(1), &mMsg) < 0)
    {
        return Rc::dataInvalid;
    }

    return Rc::success;
}

void
Spidev::configureMessage(struct spi_ioc_transfer& msg, Spidev::Cfg cfg)
{
    msg.speed_hz = cfg.speed;
    msg.delay_usecs = cfg.delay;
    msg.bits_per_word = 8;
    msg.cs_change = cfg.csChange;
    msg.rx_nbits = 0;
    msg.tx_nbits = 0;

    msg.mode = 0x00000000;

    if (cfg.polarity == Spidev::ClockPolarity::clockIdleHigh)
    {
        msg.mode |= SPI_CPHA;
    }

    if (cfg.phase == Spidev::ClockPhase::sampleOnTrailingEdge)
    {
        msg.mode |= SPI_CPOL;
    }

    if (cfg.csBehaviour == Spidev::CSBehaviour::notUsed)
    {
        msg.mode |= SPI_NO_CS;
    }
    else if (cfg.csBehaviour == Spidev::CSBehaviour::highActive)
    {
        msg.mode |= SPI_CS_HIGH;
    }

    if (cfg.lsbFirst)
    {
        msg.mode |= SPI_LSB_FIRST;
    }

    if (cfg.loop)
    {
        msg.mode |= SPI_LOOP;
    }
}
