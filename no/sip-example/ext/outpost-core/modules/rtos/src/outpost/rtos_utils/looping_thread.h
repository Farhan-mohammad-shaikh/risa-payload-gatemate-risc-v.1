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

#ifndef OUTPOST_RTOS_UTILS_LOOPING_THREAD_H
#define OUTPOST_RTOS_UTILS_LOOPING_THREAD_H

#include <outpost/rtos/thread.h>

#include <utility>

namespace outpost::rtos_utils
{

struct LoopingThread : public outpost::rtos::Thread
{
    // forward all parameters to original rtos::Thread
    template <typename... Args>
    explicit LoopingThread(Args&&... args) : Thread{std::forward<Args>(args)...}
    {
    }

protected:
    // The function that will be called by outpost::rtos::Thread
    void
    run() final override;

    // The step function that will be called forever
    virtual void
    step() = 0;
};

}  // namespace outpost::rtos_utils

#endif  // OUTPOST_RTOS_UTILS_LOOPING_THREAD_H
