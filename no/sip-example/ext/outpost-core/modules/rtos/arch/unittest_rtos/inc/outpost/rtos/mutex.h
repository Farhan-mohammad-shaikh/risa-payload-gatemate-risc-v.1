/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 * Copyright (c) 2015, Jan Sommer
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

#ifndef OUTPOST_RTOS_MUTEX_H
#define OUTPOST_RTOS_MUTEX_H

#include <outpost/time/duration.h>

namespace outpost
{
namespace rtos
{
/**
 * \class Mutex
 */
class Mutex
{
public:
    Mutex();

    // disable copy constructor
    Mutex(const Mutex& other) = delete;

    // disable assignment operator
    Mutex&
    operator=(const Mutex& other) = delete;

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
     * Acquire the mutex.
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
    acquire(::outpost::time::Duration timeout);

    /**
     * Acquire the mutex.
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
     * Release the mutex.
     */
    void
    release();

    /**
     * Release the mutex.
     *
     * \param hasWokenThread Set to true iff a higher priority thread was woken by the method.
     *        Thread::yield() should be called before exiting the ISR.
     *
     * This function will never block.
     */
    void
    releaseFromISR(bool& hasWokenThread);

private:
    void* mImplementation;
};

}  // namespace rtos
}  // namespace outpost

#endif
