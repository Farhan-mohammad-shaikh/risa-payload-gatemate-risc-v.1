/*
 * Copyright (c) 2017-2018, Fabian Greif
 * Copyright (c) 2021, Jan-Gerd Mess
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "serial_port.h"

using outpost::posix::SerialPort;

SerialPort::SerialPort() :
    mSerialPort(),
    mPreviousReadTimeout(outpost::time::Duration::zero()),
    mPreviousWriteTimeout(outpost::time::Duration::zero())
{
}

SerialPort::~SerialPort()
{
    // Close an eventually open port.
    mSerialPort.close();
}

outpost::ErrorCode
SerialPort::open(std::string deviceName, uint32_t baudrate, Parity parity)
{
    mSerialPort.close();
    mSerialPort.setPort(deviceName);
    mSerialPort.setBaudrate(baudrate);

    serial::parity_t deviceParity;
    if (parity == Parity::odd)
    {
        deviceParity = serial::parity_odd;
    }
    else if (parity == Parity::even)
    {
        deviceParity = serial::parity_even;
    }
    else
    {
        // Other parity options are not supported
        deviceParity = serial::parity_none;
    }
    mSerialPort.setParity(deviceParity);

    updateTimeout();

    try
    {
        mSerialPort.open();
        return outpost::ErrorCode::success();
    }
    catch (serial::SerialException&)
    {
        return outpost::ErrorCode::error();
    }
}

bool
SerialPort::isOpen() const
{
    return mSerialPort.isOpen();
}

void
SerialPort::close()
{
    mSerialPort.close();
}

bool
SerialPort::isAvailable()
{
    return (mSerialPort.available() > 0);
}

size_t
SerialPort::getNumberOfBytesAvailable()
{
    return mSerialPort.available();
}

outpost::hal::Serial::MaybeSize
SerialPort::read(const outpost::Slice<uint8_t>& data, const outpost::time::Duration& timeout)
{
    if (timeout != mPreviousReadTimeout)
    {
        mPreviousReadTimeout = timeout;
        updateTimeout();
    }

    size_t bytesRead = 0;
    try
    {
        bytesRead = mSerialPort.read(&data[0], data.getNumberOfElements());
    }
    catch (serial::PortNotOpenedException&)
    {
        return unexpected(SerialError::persistent());
    }
    catch (serial::IOException&)
    {
        return unexpected(SerialError::temporary());
    }
    catch (serial::SerialException&)
    {
        return unexpected(SerialError::error());
    }

    return bytesRead;
}

outpost::hal::Serial::MaybeSize
SerialPort::write(const outpost::Slice<const uint8_t>& data, const outpost::time::Duration& timeout)
{
    if (timeout != mPreviousWriteTimeout)
    {
        mPreviousWriteTimeout = timeout;
        updateTimeout();
    }

    size_t bytesWritten = mSerialPort.write(&data[0], data.getNumberOfElements());
    return bytesWritten;
}

void
SerialPort::flushReceiver()
{
    mSerialPort.flushInput();
}

void
SerialPort::flushTransmitter()
{
    mSerialPort.flushOutput();
}

void
SerialPort::updateTimeout()
{
    auto deviceTimeout = serial::Timeout(serial::Timeout::max(),
                                         mPreviousReadTimeout.milliseconds(),
                                         0,
                                         mPreviousWriteTimeout.milliseconds(),
                                         0);
    mSerialPort.setTimeout(deviceTimeout);
}
