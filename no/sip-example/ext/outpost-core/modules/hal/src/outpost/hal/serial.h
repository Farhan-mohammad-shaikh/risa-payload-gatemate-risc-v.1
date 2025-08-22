/*
 * Copyright (c) 2014-2018, Fabian Greif
 * Copyright (c) 2014, Norbert Toth
 * Copyright (c) 2015, 2017, Jan Sommer
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2023, Jan-Gerd Mess
 * Copyright (c) 2023, Cold, Erin Cynthia
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_HAL_SERIAL_H
#define OUTPOST_HAL_SERIAL_H

#include <outpost/base/slice.h>
#include <outpost/time/duration.h>
#include <outpost/utils/error_code.h>
#include <outpost/utils/expected.h>

#include <stdint.h>

#include <cstddef>

namespace outpost
{
namespace hal
{

class SerialError : public outpost::ErrorCode
{
public:
    // Unknown error
    static SerialError
    error(const int16_t err = -1)
    {
        return SerialError(err);
    }

    // Trying again might work
    static SerialError
    temporary()
    {
        return SerialError(-2);
    }

    // Don't try again, the fault is persistent
    static SerialError
    persistent()
    {
        return SerialError(-3);
    }

    // Input parameters are invalid
    static SerialError
    inputInvalid()
    {
        return SerialError(-4);
    }

protected:
    explicit SerialError(int16_t code) : outpost::ErrorCode(code)
    {
    }
};

class SerialRx
{
public:
    using MaybeSize = outpost::Expected<size_t, SerialError>;

    virtual ~SerialRx() = default;

    /**
     * Check if data is available in the input buffers.
     *
     * \retval true   Data is available and can be read via read(...).
     * \retval false  No data available.
     */
    virtual bool
    isAvailable() = 0;

    /**
     * Check how many bytes are available in the input buffers.
     *
     * \return  Number of bytes in the buffer
     */
    virtual size_t
    getNumberOfBytesAvailable() = 0;

    /**
     * Read a block of bytes.
     *
     * Waits until the timeout occurs to read the given number of
     * bytes. May return earlier if enough bytes are available. The
     * exact number of bytes read will be returned, it will be up to
     * \p data.getNumberOfElements() bytes but can also be any lower value.
     *
     * \param data
     *      Buffer to write the received data to.
     * \param timeout
     *      Will return if call has exceeded this time, default is
     *      blocking call.
     * \return
     *      Either the Number of bytes which could be read
     *      (maximal \p data.getNumberOfElements()), or an
     *      outpost::ErrorCode indicating the type of failure
     */
    virtual MaybeSize
    read(const outpost::Slice<uint8_t>& data,
         const outpost::time::Duration& timeout = outpost::time::Duration::myriad()) = 0;

    /**
     * Flush receive buffers.
     *
     * Data remaining in the receive buffer is discarded. Afterwards all
     * internal buffers are empty.
     */
    virtual void
    flushReceiver() = 0;
};

class SerialTx
{
public:
    using MaybeSize = outpost::Expected<size_t, SerialError>;

    virtual ~SerialTx() = default;

    /**
     * Write a block of bytes with timeout.
     *
     * \param data
     *      Buffer containing the data to send.
     * \param timeout
     *      Will return if call has exceeded this time, default is
     *      blocking call.
     * \return
     *      Number of bytes which could be sent
     *      (maximal \p data.getNumberOfElements()), or an
     *      outpost::ErrorCode indicating the type of failure
     */
    virtual MaybeSize
    write(const outpost::Slice<const uint8_t>& data,
          const outpost::time::Duration& timeout = outpost::time::Duration::myriad()) = 0;

    /**
     * Sends eventually buffered data. Afterwards all internal buffers
     * are empty.
     */
    virtual void
    flushTransmitter() = 0;
};

/**
 * Serial Interface.
 *
 * \author  Fabian Greif
 */
class Serial : public SerialRx, public SerialTx
{
public:
    using SerialError = outpost::hal::SerialError;
    using MaybeSize = SerialRx::MaybeSize;
    static_assert(std::is_same_v<SerialRx::MaybeSize, SerialTx::MaybeSize>);

    virtual ~Serial() = 0;

    /**
     * Close the UART device
     */
    virtual void
    close() = 0;
};

}  // namespace hal
}  // namespace outpost

#endif
