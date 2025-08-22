/*
 * Copyright (c) 2020-2021, Jan Malburg
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_POSIX_BARRIER_H
#define OUTPOST_POSIX_BARRIER_H

#include <pthread.h>

#include <stdint.h>

namespace outpost
{
namespace posix
{
/**
 * Barrier class.
 *
 * Barrier are used to ensure that a set of threads are at a defined position before continuing
 *
 * Implemented using POSIX barrier
 *
 * \author    Jan Malburg
 * \ingroup    rtos
 */
class Barrier
{
public:
    /**
     * Create a new Barrier.
     *
     *
     * @param numberOfThreads	number of threads that must wait on the barrier for them to continue
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
    ~Barrier();

    /**
     * Waits till the set amount of threads are waiting.
     */
    void
    wait();

private:
    pthread_barrier_t mBarrier;
};

}  // namespace posix
}  // namespace outpost

#endif
