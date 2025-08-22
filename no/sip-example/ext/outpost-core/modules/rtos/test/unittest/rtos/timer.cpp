/*
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "outpost/rtos/timer.h"

#include "unittest/rtos/timer_mock.h"

using outpost::rtos::Timer;

// ---------------------------------------------------------------------------
Timer::Timer(outpost::Callable* object, Function function, const char* name) :
    mImplementation(nullptr)
{
    auto* timer = new unittest::rtos::TimerImplementation(this, object, function, name);
    mImplementation = timer;
}

Timer::~Timer()
{
    auto* timer = reinterpret_cast<unittest::rtos::TimerImplementation*>(mImplementation);
    delete timer;
    mImplementation = nullptr;
}

// ---------------------------------------------------------------------------
void
Timer::start(time::Duration duration)
{
    auto* timer = reinterpret_cast<unittest::rtos::TimerImplementation*>(mImplementation);
    return timer->start(duration);
}

void
Timer::reset()
{
    auto* timer = reinterpret_cast<unittest::rtos::TimerImplementation*>(mImplementation);
    return timer->reset();
}

void
Timer::cancel()
{
    auto* timer = reinterpret_cast<unittest::rtos::TimerImplementation*>(mImplementation);
    return timer->cancel();
}

bool
Timer::isRunning()
{
    auto* timer = reinterpret_cast<unittest::rtos::TimerImplementation*>(mImplementation);
    return timer->isRunning();
}

void
Timer::startTimerDaemonThread(uint8_t priority, size_t stack, CpuMask affinity)
{
    auto& allTimers = unittest::rtos::TimerImplementation::allInstances;
    assert(allTimers.size() > 0U);

    auto* timer = allTimers.first();
    return timer->startTimerDaemonThread(priority, stack, affinity);
}
