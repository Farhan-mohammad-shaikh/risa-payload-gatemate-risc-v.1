/*
 * Copyright (c) 2020, Moyano, Gabriel
 * Copyright (c) 2021, Jan-Gerd Mess
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2023-2024, Pieper, Pascal
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

#ifndef OUTPOST_TERMIOS_SERIAL_PORT_H
#define OUTPOST_TERMIOS_SERIAL_PORT_H

#include <outpost/hal/serial.h>
#include <outpost/time/clock.h>
#include <outpost/time/duration.h>
#include <outpost/utils/expected.h>

#include <cstdint>

namespace outpost
{
namespace posix
{
/**
 * Implementation of outpost::hal::Serial for posix based on termios
 */
class TermiosRawPort : public hal::Serial
{
public:
    enum class Parity
    {
        none,
        odd,
        even,
    };

public:
    explicit TermiosRawPort(outpost::time::Clock& clock);
    ~TermiosRawPort();
    TermiosRawPort(const TermiosRawPort&) = delete;

    /**
     * Open the desired serial port.
     *
     * \param deviceName (e.g. "/dev/gr_pci/apbuart0")
     * \param baudrate (e.g. 115200, 9600, etc.)
     * \param parity (e.g. Parity::odd, Parity::even. By default it's
     * Parity::none)
     *
     * \retval ErrorCode::success if the device was opened correctly
     * \retval Any error if some issue happend trying to open the device
     */
    outpost::ErrorCode
    open(const Slice<const char>& deviceName, uint32_t baudrate, Parity parity = Parity::none);

    /**
     * See hal::Serial
     */
    void
    close() override;

    /**
     * See hal::Serial
     */
    bool
    isAvailable() override;

    /**
     * See hal::Serial
     */
    size_t
    getNumberOfBytesAvailable() override;

    /**
     * See hal::Serial
     */
    MaybeSize
    read(const Slice<uint8_t>& data,
         const time::Duration& timeout = time::Duration::myriad()) override;

    /**
     * See hal::Serial
     */
    MaybeSize
    write(const Slice<const uint8_t>& data,
          const time::Duration& timeout = time::Duration::myriad()) override;

    /**
     * See hal::Serial
     */
    void
    flushReceiver() override;

    /*
     * See hal::Serial
     */
    void
    flushTransmitter() override;

protected:
    /**
     * Update the value of mReadTimeout and  mTerm.c_cc[VTIME]
     *
     * \param timeout new timeout duration
     */
    void
    updateReadTimeout(outpost::time::Duration timeout);

    bool mIsOpen;
    outpost::time::Duration mReadTimeout;
    outpost::time::Clock& mClock;
    int mFd;
};

}  // namespace posix
}  // namespace outpost

#endif  // OUTPOST_TERMIOS_SERIAL_PORT_H
