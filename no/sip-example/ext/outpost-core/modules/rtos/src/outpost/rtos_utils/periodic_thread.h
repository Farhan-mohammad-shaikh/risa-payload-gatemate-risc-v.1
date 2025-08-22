/*
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Pascal Pieper
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Reinking, Janosch
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_UTILS_PERIODIC_THREAD_H
#define OUTPOST_RTOS_UTILS_PERIODIC_THREAD_H

#include <outpost/rtos/periodic_task_manager.h>
#include <outpost/rtos/thread.h>

#include <utility>

namespace outpost::rtos_utils
{

struct PeriodicThread : public outpost::rtos::Thread
{
    // forward all additional parameters to original rtos::Thread
    template <typename... Args>
    explicit PeriodicThread(const time::Duration& period, Args&&... args) :
        Thread{std::forward<Args>(args)...}, mPeriod{period}
    {
    }

protected:
    // The function that will be called by outpost::rtos::Thread
    void
    run() final override;

    // The step function that will be called forever, in the defined period
    virtual void
    step() = 0;

private:
    const time::Duration mPeriod;
};

}  // namespace outpost::rtos_utils

#endif  // OUTPOST_RTOS_UTILS_PERIODIC_THREAD_H
