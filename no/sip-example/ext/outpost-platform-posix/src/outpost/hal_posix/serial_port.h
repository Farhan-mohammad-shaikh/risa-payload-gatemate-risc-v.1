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

#ifndef OUTPOST_POSIX_SERIAL_PORT_H
#define OUTPOST_POSIX_SERIAL_PORT_H

#include <serial/serial.h>

#include <outpost/hal/serial.h>

namespace outpost
{
namespace posix
{
/**
 * Wrapper for POSIX serial port devices.
 *
 * Allows to communicate over serial port devices. The exact device is only
 * selected when opening the connection, therefore it is possible to connect
 * to a different device during run-time.
 *
 * Uses the serial library from William Woodall as backend (originally from
 * https://github.com/wjwwood/serial).
 *
 * \author  Fabian Greif
 */
class SerialPort : public hal::Serial
{
public:
    enum class Parity
    {
        none,
        odd,
        even
    };

    SerialPort();

    // see hal::Serial
    virtual ~SerialPort();

    /**
     * Open a device.
     *
     * \param   deviceName
     *      E.g. `/dev/ttyS0`, `/dev/ttyUSB0`, ...
     *
     * \param   baudrate
     *      Desired baudrate (e.g. `115200` for 115.2kBaud).
     *
     * \retval  success    Device was opened and is ready to send/receive data.
     * \retval  error      Open failed and device can not be used
     *                     (ErrorCode describing the error)
     */
    outpost::ErrorCode
    open(std::string deviceName, uint32_t baudrate, Parity parity = Parity::none);

    /**
     * Check whether the device has been opened.
     */
    bool
    isOpen() const;

    // see hal::Serial
    virtual void
    close() override;

    // see hal::Serial
    virtual bool
    isAvailable() override;

    // see hal::Serial
    virtual size_t
    getNumberOfBytesAvailable() override;

    // see hal::Serial
    virtual MaybeSize
    read(const Slice<uint8_t>& data,
         const outpost::time::Duration& timeout = time::Duration::myriad()) override;

    // see hal::Serial
    virtual MaybeSize
    write(const Slice<const uint8_t>& data,
          const time::Duration& timeout = time::Duration::myriad()) override;

    // see hal::Serial
    virtual void
    flushReceiver() override;

    // see hal::Serial
    virtual void
    flushTransmitter() override;

private:
    void
    updateTimeout();

    serial::Serial mSerialPort;
    outpost::time::Duration mPreviousReadTimeout;
    outpost::time::Duration mPreviousWriteTimeout;
};

}  // namespace posix
}  // namespace outpost

#endif  // OUTPOST_HAL_UDP_SOCKET_H
