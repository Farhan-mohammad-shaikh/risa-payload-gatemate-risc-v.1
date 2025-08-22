/*
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Pascal Pieper
 * Copyright (c) 2024, Reinking, Janosch
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_UTILS_STOPPABLE_THREAD_H
#define OUTPOST_RTOS_UTILS_STOPPABLE_THREAD_H

#include <outpost/rtos/thread.h>

#include <atomic>

namespace outpost::rtos_utils
{

struct StoppableThread : public outpost::rtos::Thread
{
    // forward all parameters to original rtos::Thread
    template <typename... Args>
    explicit StoppableThread(Args... args) : Thread{std::forward<Args>(args)...}
    {
    }

    // command thread to stop
    void
    doStop();

protected:
    // The function that will be called by outpost::rtos::Thread
    void
    run() final override;

    // The step function that will be called until `doStop()` is invoked
    virtual void
    step() = 0;

private:
    std::atomic<bool> mStop = false;
};

}  // namespace outpost::rtos_utils

#endif  // OUTPOST_RTOS_UTILS_STOPPABLE_THREAD_H
