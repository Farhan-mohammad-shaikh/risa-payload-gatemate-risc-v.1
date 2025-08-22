/*
 * Copyright (c) 2017, 2019, Fabian Greif
 * Copyright (c) 2024, Reinking, Janosch
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_UTILS_LOCKING_POLICY_H
#define OUTPOST_RTOS_UTILS_LOCKING_POLICY_H

#include <outpost/rtos/mutex.h>

namespace outpost
{
namespace rtos_utils
{
namespace locking_policy
{
/**
 * No locking.
 */
class SingleThreaded
{
public:
    class Lock
    {
    public:
        explicit inline Lock(const SingleThreaded&)
        {
        }

        // Disable copy constructor and copy assignment operator
        Lock(const Lock&) = delete;

        Lock&
        operator=(const Lock&) = delete;

        inline ~Lock() = default;
    };
};

/**
 * Create a Mutex per object.
 */
class MutexLock
{
public:
    class Lock
    {
    public:
        explicit inline Lock(const MutexLock& parent) : mMutex(parent.mMutex)
        {
            mMutex.acquire();
        }

        // Disable copy constructor and copy assignment operator
        Lock(const Lock&) = delete;

        Lock&
        operator=(const Lock&) = delete;

        inline ~Lock()
        {
            mMutex.release();
        }

    private:
        outpost::rtos::Mutex& mMutex;
    };

private:
    friend class Lock;

    mutable outpost::rtos::Mutex mMutex;
};

/**
 * Use a Mutex shared between multiple objects.
 */
template <outpost::rtos::Mutex& mutex>
class SharedMutexLock
{
public:
    class Lock
    {
    public:
        explicit inline Lock(const SharedMutexLock&)
        {
            mutex.acquire();
        }

        // Disable copy constructor and copy assignment operator
        Lock(const Lock&) = delete;

        Lock&
        operator=(const Lock&) = delete;

        inline ~Lock()
        {
            mutex.release();
        }
    };

private:
    friend class Lock;
};

}  // namespace locking_policy
}  // namespace rtos_utils
}  // namespace outpost

#endif
