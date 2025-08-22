/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 * Copyright (c) 2022, Jan-Gerd Mess
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_NONE_MUTEX_HPP
#define OUTPOST_RTOS_NONE_MUTEX_HPP

#include <outpost/time/duration.h>

#include <stdint.h>

namespace outpost
{
namespace rtos
{
/**
 * Mutex class.
 *
 * Mutexes can be used to enforce mutual exclusion for a critical
 * section in user code.
 *
 * Implemented using RTEMS binary semaphores with priority pool locking
 * and priority inheritance.
 *
 * \author    Fabian Greif
 * \ingroup    rtos
 */
class Mutex
{
public:
    /**
     * Create a new Mutex.
     *
     * The mutex is released after creation.
     *
     * RTEMS has only one pool for mutexes and semaphores. Make sure
     * the maximum number of allocated mutexes and semaphores combined
     * is smaller than the number given via \c CONFIGURE_MAXIMUM_SEMAPHORES.
     */
    Mutex();

    // disable copy constructor
    Mutex(const Mutex& other) = delete;

    // disable assignment operator
    Mutex&
    operator=(const Mutex& other) = delete;

    /**
     * Destroy the mutex and free it's resources.
     *
     * The resources are added back to the resource pool created by
     * CONFIGURE_MAXIMUM_SEMAPHORES.
     */
    ~Mutex();

    /**
     * Acquire the mutex.
     *
     * This function may block if the mutex is currently held by an
     * other thread.
     *
     * \returns    \c true if the mutex could be acquired.
     */
    bool
    acquire();

    /**
     * Acquire the mutex. Not to be used from within ISRs.
     *
     * Same as acquire() but blocks only for \p timeout milliseconds.
     *
     * \param    timeout
     *         Timeout in milliseconds.
     *
     * \return    \c true if the mutex could be acquired, \c false in
     *             case of an error or timeout.
     */
    bool
    acquire(time::Duration timeout);

    /**
     * Acquire the mutex. Only to be used from within ISRs.
     *
     *
     * \param hasWokenThread Set to true iff a higher priority thread was woken by the method.
     *        Thread::yield() should be called before exiting the ISR.
     *
     * \returns    \c true if the mutex could be acquired.
     */
    inline bool
    acquireFromISR(bool& hasWokenThread)
    {
        hasWokenThread = false;
        return acquire();
    }

    /**
     * Release the mutex.
     *
     * This function will never block.
     */
    inline void
    release()
    {
    }

    /**
     * Release the mutex. Not needed for bare-metal.
     *
     * \param hasWokenThread Set to true iff a higher priority thread was woken by the method.
     *        Thread::yield() should be called before exiting the ISR.
     *
     * This function will never block.
     */
    void
    releaseFromISR(bool& hasWokenThread)
    {
        hasWokenThread = false;
    }
};

}  // namespace rtos
}  // namespace outpost

#endif
