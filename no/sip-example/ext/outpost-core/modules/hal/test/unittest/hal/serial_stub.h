/*
 * Copyright (c) 2015-2017, Fabian Greif
 * Copyright (c) 2021, Felix Passenberg
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

#ifndef UNITTEST_HAL_SERIAL_STUB_H
#define UNITTEST_HAL_SERIAL_STUB_H

#include <outpost/hal/serial.h>

#include <vector>

namespace unittest
{
namespace hal
{
/**
 * Serial interface stub.
 *
 * Provides direct access to the send- and receive queue.
 *
 * \author  Fabian Greif
 */
class SerialStub : public outpost::hal::Serial
{
public:
    SerialStub();

    virtual ~SerialStub();

    virtual void
    close() override;

    virtual bool
    isAvailable() override;

    virtual size_t
    getNumberOfBytesAvailable() override;

    virtual MaybeSize
    read(const outpost::Slice<uint8_t>& data,
         const outpost::time::Duration& timeout = outpost::time::Duration::myriad()) override;

    virtual MaybeSize
    write(const outpost::Slice<const uint8_t>& data,
          const outpost::time::Duration& timeout = outpost::time::Duration::myriad()) override;

    /// Does nothing in this implementation
    virtual void
    flushReceiver() override;

    /// Does nothing in this implementation
    virtual void
    flushTransmitter() override;

    /// Data which should be accessible through the read function
    std::vector<uint8_t> mDataToReceive;

    /// Data which has be stored through the write function
    std::vector<uint8_t> mDataToTransmit;
};

}  // namespace hal
}  // namespace unittest

#endif
