/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 * Copyright (c) 2022, Tobias Pfeffer
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

#ifndef OUTPOST_RTOS_FREERTOS_MUTEX_HPP
#define OUTPOST_RTOS_FREERTOS_MUTEX_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

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
     */
    Mutex();

    // disable copy constructor
    Mutex(const Mutex& other) = delete;

    // disable assignment operator
    Mutex&
    operator=(const Mutex& other) = delete;

    /**
     * Destroy the mutex and free it's resources.
     */
    ~Mutex();

    /**
     * Acquire the mutex. Not to be called from with ISRs.
     *
     * This function may block if the mutex is currently held by an
     * other thread.
     *
     * \returns    \c true if the mutex could be acquired.
     */
    bool
    acquire();

    /**
     * Acquire the mutex. Not to be called from with ISRs.
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
     * Acquire the mutex. Only to be used from within an ISR.
     *
     *
     * \param hasWokenThread Set to true iff a higher priority thread was woken by the method.
     *        Thread::yield() should be called before exiting the ISR.
     *
     * \returns    \c true if the mutex could be acquired.
     */
    bool
    acquireFromISR(bool& hasWokenThread);

    /**
     * Release the mutex. Not to be called from with ISRs.
     *
     * This function will never block.
     */
    void
    release();

    /**
     * Release the mutex. Only to be used from within an ISR.
     *
     * \param hasWokenThread Set to true iff a higher priority thread was woken by the method.
     *        Thread::yield() should be called before exiting the ISR.
     *
     * This function will never block.
     */
    void
    releaseFromISR(bool& hasWokenThread);

private:
    QueueDefinition* mHandle;
};

}  // namespace rtos
}  // namespace outpost

#endif
