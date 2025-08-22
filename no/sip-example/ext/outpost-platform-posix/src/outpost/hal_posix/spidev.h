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

#ifdef __rtems__

#ifndef OUTPOST_POSIX_SPIDEV_H
#define OUTPOST_POSIX_SPIDEV_H

#include <linux/spi/spidev.h>

#include <outpost/hal/spi.h>

namespace outpost
{
namespace posix
{
/**
 * Implementation of outpost::hal::Spi for posix based on spidev
 */
class Spidev : public hal::Spi
{
public:
    enum class ClockPolarity
    {
        clockIdleLow,
        clockIdleHigh
    };

    enum class ClockPhase
    {
        sampleOnLeadingEdge,
        sampleOnTrailingEdge
    };

    enum class CSBehaviour
    {
        notUsed,
        lowActive,
        highActive
    };

    struct Cfg
    {
        ClockPolarity polarity;
        ClockPhase phase;
        CSBehaviour csBehaviour;
        bool lsbFirst;
        bool loop;
        /// The bit-rate of the device [Hz]
        uint32_t speed;
        /**
         * The delay after a transfer before the chip select status is changed
         * and the next transfer is triggered. [µs]
         */
        uint16_t delay;
        /**
         * The device is deselected after transfer ended and before a new
         * transfer is started.
         */
        bool csChange;
        /**
         * The highest chip select number
         *
         * "hal::Spi::noChipSelect" for no chip select
         *
         */
        uint8_t csMax;
    };

    enum class ReturnCode
    {
        success = 0,

        alreadyOpened,
        deviceNameInvalid,
        couldNotOpen,

        couldNotSetLsbSetting,
        couldNotSetBitsPerWordSetting,
        couldNotSetMaxSpeedSetting,
        couldNotSetMode
    };

public:
    static constexpr Cfg defaultCfg = {ClockPolarity::clockIdleLow,
                                       ClockPhase::sampleOnLeadingEdge,
                                       CSBehaviour::notUsed,
                                       false,
                                       false,
                                       1000000,  /// 1 MHz
                                       0,
                                       true,
                                       hal::Spi::noChipSelect};

public:
    explicit Spidev();

    ~Spidev();

    Spidev(const Spidev&) = delete;

    /**
     * Open the desired spi device.
     *
     * \param deviceName (e.g. "/dev/ttySPI_0")
     *
     * \return
     *      An implementation specific return code
     */
    ReturnCode
    open(outpost::Slice<const char> deviceName, const Cfg cfg = Spidev::defaultCfg);

    /**
     * See hal::Spi
     */
    void
    close() override;

    /**
     * See hal::Spi
     */
    hal::Spi::ReturnCode
    read(outpost::Slice<uint8_t> data, uint8_t cs = hal::Spi::noChipSelect) override;

    /**
     * See hal::Spi
     */
    hal::Spi::ReturnCode
    write(outpost::Slice<const uint8_t> data, uint8_t cs = hal::Spi::noChipSelect) override;

    /**
     * See hal::Spi
     */
    hal::Spi::ReturnCode
    transfer(outpost::Slice<const uint8_t> outData,
             outpost::Slice<uint8_t> inData,
             uint8_t cs = hal::Spi::noChipSelect) override;

private:
    static void
    configureMessage(struct spi_ioc_transfer& msg, Spidev::Cfg cfg = Spidev::defaultCfg);

    bool mIsOpen;

    int mFd;

    uint8_t mCsMax;

    struct spi_ioc_transfer mMsg;
};

}  // namespace posix
}  // namespace outpost

#endif  // OUTPOST_POSIX_SPIDEV_H

#endif  // __rtems__
