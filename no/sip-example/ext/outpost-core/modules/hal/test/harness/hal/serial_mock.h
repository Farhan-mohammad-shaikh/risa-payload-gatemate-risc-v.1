/*
 * Copyright (c) 2024, Cold, Erin Cynthia
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef HARNESS_HAL_SERIAL_MOCK_H
#define HARNESS_HAL_SERIAL_MOCK_H

#include <outpost/hal/serial.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unittest/base/slice.h>

namespace harness
{
namespace hal
{

class SerialMock : public outpost::hal::Serial
{
public:
    MOCK_METHOD(bool, isAvailable, (), (override));
    MOCK_METHOD(size_t, getNumberOfBytesAvailable, (), (override));
    MOCK_METHOD(MaybeSize,
                read,
                (const outpost::Slice<uint8_t>&, const outpost::time::Duration&),
                (override));
    MOCK_METHOD(void, flushReceiver, (), (override));
    MOCK_METHOD(MaybeSize,
                write,
                (const outpost::Slice<const uint8_t>&, const outpost::time::Duration&),
                (override));
    MOCK_METHOD(void, flushTransmitter, (), (override));
    MOCK_METHOD(void, close, (), (override));

    // This lets us insert data that the mock reads
    static inline auto
    ReadFrom(outpost::Slice<const uint8_t> readData)
    {
        return ::testing::DoAll(unittest::base::SliceCopyFrom<0>(readData),
                                ::testing::Return(readData.getNumberOfElements()));
    }
};

}  // namespace hal
}  // namespace harness

#endif  // HARNESS_HAL_SERIAL_MOCK_H
