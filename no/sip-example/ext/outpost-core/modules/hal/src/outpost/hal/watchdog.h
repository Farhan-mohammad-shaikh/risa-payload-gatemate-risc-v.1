/*
 * Copyright (c) 2021, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/time/duration.h>

#ifndef OUTPOST_HAL_WATCHDOG_H_
#define OUTPOST_HAL_WATCHDOG_H_

namespace outpost
{
namespace hal
{
/**
 * Interface for an (hardware) watchdog
 */
class Watchdog
{
public:
    Watchdog() = default;
    virtual ~Watchdog() = default;

    /*
     * Resets the timer of the watchdog.
     *
     * @return true if reset was successful, false otherwise
     */
    virtual bool
    reset() = 0;

    /**
     * Returns the interval within which a reset must occur.
     * If reset is not called within this interval since the last
     * call to reset watchdog will trigger.
     *
     * @return the interval in which a reset must occur
     */
    virtual outpost::time::Duration
    getInterval() = 0;
};
}  // namespace hal
}  // namespace outpost

#endif
