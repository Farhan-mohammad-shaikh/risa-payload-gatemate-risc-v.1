/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_MUTEX_GUARD_HPP
#define OUTPOST_RTOS_MUTEX_GUARD_HPP

#include <outpost/rtos/mutex.h>

namespace outpost
{
namespace rtos
{
/**
 * Implements a RAII-style locking.
 *
 * Locks the Mutex when created and unlocks it on destruction.
 *
 * Example:
 *
 *     Mutex mutex;
 *     ...
 *     {
 *         MutexGuard lock(mutex);
 *         // Mutex is locked on creation of lock
 *
 *         ... do something ...
 *
 *         // End of scope for lock => Mutex is unlocked.
 *     }
 *
 * \author    Fabian Greif
 * \ingroup    rtos
 */
template <typename T>
class Guard
{
public:
    /**
     * Acquire the mutex.
     *
     * \param m
     *      The mutex to acquire.
     */
    explicit inline Guard(T& m) : mLock(m)
    {
        mLock.acquire();
    }

    // Disable copy constructor
    Guard(const Guard&) = delete;

    // Disable assignment operator
    Guard&
    operator=(const Guard& other) = delete;

    /**
     * Release the mutex.
     */
    inline ~Guard()
    {
        mLock.release();
    }

private:
    T& mLock;
};

/**
 * Lock for Mutexes.
 */
typedef Guard<Mutex> MutexGuard;

}  // namespace rtos
}  // namespace outpost

#endif
