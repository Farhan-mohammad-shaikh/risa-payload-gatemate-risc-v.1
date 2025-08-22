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

#ifndef OUTPOST_RTOS_FREERTOS_BARRIER_HPP
#define OUTPOST_RTOS_FREERTOS_BARRIER_HPP

// included as standard include for testing reason
#include <outpost/rtos/mutex.h>
#include <outpost/rtos/semaphore.h>

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
 * Implementation using Semaphore and mutex and no freertos specific part directly, hence, as such
 * is a general implementation.
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
    explicit Barrier(uint32_t numberOfThreads);

    // disable copy constructor
    Barrier(const Barrier& other) = delete;

    // disable assignment operator
    Barrier&
    operator=(const Barrier& other) = delete;

    /**
     * Destroy the Barrier and free it's resources.
     */
    ~Barrier() = default;

    /**
     * Waits till the set amount of threads are waiting.
     */
    void
    wait();

private:
    // explicit naming for testing reasons
    ::outpost::rtos::Mutex mCounterMutex;
    ::outpost::rtos::BinarySemaphore mWaitSemaphore;
    ::outpost::rtos::BinarySemaphore mReleaseSemaphore;

    uint32_t mMaxWaiting;
    uint32_t mCurrentlyWaiting;
};

}  // namespace rtos
}  // namespace outpost

#endif
