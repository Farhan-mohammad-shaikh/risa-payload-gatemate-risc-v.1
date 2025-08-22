/*
 * Copyright (c) 2021, Andre Nahrwold
 * Copyright (c) 2021, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef UNITTEST_HAL_SPI_STUB_H
#define UNITTEST_HAL_SPI_STUB_H

#include <outpost/hal/spi.h>

#include <vector>

using namespace outpost::hal;

namespace unittest
{
namespace hal
{
/**
 * Spi interface stub.
 *
 * Provides direct access to the send- and receive queue.
 *
 * \author  Andre Nahrwold
 */
class SpiStub : public Spi
{
public:
    /**
     * Constructor
     */
    SpiStub();

    /**
     * Destructor
     */
    ~SpiStub();

    void
    close() override;

    Spi::ReturnCode
    read(outpost::Slice<uint8_t> data, uint8_t cs = outpost::hal::Spi::noChipSelect) override;

    Spi::ReturnCode
    write(outpost::Slice<const uint8_t> data,
          uint8_t cs = outpost::hal::Spi::noChipSelect) override;

    Spi::ReturnCode
    transfer(outpost::Slice<const uint8_t> outData,
             outpost::Slice<uint8_t> inData,
             uint8_t cs = outpost::hal::Spi::noChipSelect) override;

    /// Data which should be accessible through the read + transfer function
    std::vector<uint8_t> mDataToRead;

    /// Data which has been stored through the write + transfer function
    std::vector<uint8_t> mDataToWrite;
};

}  // namespace hal
}  // namespace unittest

#endif
