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

#ifndef OUTPOST_POSIX_MUTEX_H
#define OUTPOST_POSIX_MUTEX_H

#include <pthread.h>

#include <outpost/time/duration.h>

namespace outpost
{
namespace posix
{
/**
 * Mutex
 *
 * \author    Fabian Greif
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

    inline ~Mutex()
    {
        pthread_mutex_destroy(&mMutex);
    }

    /**
     * Acquire the mutex.
     *
     * This function may block if the mutex is currently held by an
     * other thread.
     *
     * \returns    \c true if the mutex could be acquired.
     */
    inline bool
    acquire()
    {
        return (pthread_mutex_lock(&mMutex) == 0);
    }

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
     * Acquire the mutex. Not required for POSIX.
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
     */
    inline void
    release()
    {
        pthread_mutex_unlock(&mMutex);
    }

    /**
     * Release the mutex. Not required for POSIX.
     *
     * \param hasWokenThread Set to true iff a higher priority thread was woken by the method.
     *        Thread::yield() should be called before exiting the ISR.
     *
     * This function will never block.
     */
    inline void
    releaseFromISR(bool& hasWokenThread)
    {
        hasWokenThread = false;
        release();
    }

private:
    pthread_mutex_t mMutex;
};

}  // namespace posix
}  // namespace outpost

#endif
