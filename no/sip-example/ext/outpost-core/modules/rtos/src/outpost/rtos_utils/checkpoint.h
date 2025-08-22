/*
 * Copyright (c) 2016-2017, Fabian Greif
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2024, Reinking, Janosch
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_UTILS_CHECKPOINT_H
#define OUTPOST_RTOS_UTILS_CHECKPOINT_H

#include <outpost/rtos/mutex.h>
#include <outpost/rtos/mutex_guard.h>
#include <outpost/rtos/semaphore.h>

namespace outpost
{
namespace rtos_utils
{
/**
 * Checkpoint at which threads can be suspended or resumed.
 *
 * Directly suspending or resuming threads is very dangerous because it can
 * very easily lead to deadlocks. This checkpoint class provides an alternative
 * based on semaphores.
 *
 * Threads can be suspended at a checkpoint until they are needed again. The
 * use of this checkpoint class allows to properly release resources before
 * suspending the thread.
 *
 * \author  Fabian Greif
 */
class Checkpoint
{
public:
    enum State
    {
        /// Thread is blocked at a pass() call.
        suspended,

        /// Thread is currently running.
        running,

        /// Thread is running but will block on the next invocation of pass().
        suspending
    };

    explicit Checkpoint(State initialState = suspending);

    ~Checkpoint();

    /**
     * Resume a thread from a blocked checkpoint.
     *
     * Can be called multiple times but only resumes the checkpoint once.
     */
    void
    resume();

    /**
     * Suspend a thread at the next pass through the checkpoint.
     *
     * Can be called multiple times but only suspends the checkpoint once. The
     * thread is not suspended until getState() returns 'suspended'.
     */
    void
    suspend();

    /**
     * Check the current state of the checkpoint.
     */
    State
    getState() const;

    /**
     * Check if the thread will be suspended on the next invocation of pass().
     *
     * Can be used as an event flag for the thread using the checkpoint to
     * finish an operation and prepare for suspending.
     */
    bool
    shouldSuspend() const;

    /**
     * Pass the checkpoint.
     *
     * Blocks if the thread should be suspended or passes through directly
     * otherwise. Switches to the 'suspended' state if blocked.
     */
    void
    pass();

private:
    mutable outpost::rtos::Mutex mMutex;
    outpost::rtos::BinarySemaphore mSemaphore;
    State mState;
};

}  // namespace rtos_utils
}  // namespace outpost

#endif
