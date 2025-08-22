/*
 * Copyright (c) 2020-2021, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_RTEMS_BARRIER_HPP
#define OUTPOST_RTOS_RTEMS_BARRIER_HPP

#include <stdint.h>

namespace outpost
{
namespace rtos
{
/**
 * Barrier class.
 *
 * Barrier are used to ensure that a set of threads are at a defined position before continuing
 *
 * Stub implementation for none OS version, assuming no threading in this case -> no blocking
 *
 * \author    Jan Malburg
 * \ingroup    rtos
 */
class Barrier
{
public:
    /**
     * Create a new Barrier. Parameter here for same interface with versions for other OSs.
     */
    explicit Barrier(uint32_t){};

    // disable copy constructor
    Barrier(const Barrier& other) = delete;

    // disable assignment operator
    Barrier&
    operator=(const Barrier& other) = delete;

    /**
     * Destroy the Barrier and free it's resources.
     */
    ~Barrier(){};

    /**
     * Waits till the set amount of threads are waiting.
     */
    void
    wait(){};
};

}  // namespace rtos
}  // namespace outpost

#endif
