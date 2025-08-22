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

#include "outpost/posix/timer_raw.h"

using outpost::rtos::Timer;

// ---------------------------------------------------------------------------
Timer::Timer(outpost::Callable* object, Function function, const char* name) :
    mImplementation(nullptr)
{
    auto* timer = new outpost::posix::TimerRaw(this, object, function, name);
    mImplementation = timer;
}

Timer::~Timer()
{
    auto* timer = reinterpret_cast<outpost::posix::TimerRaw*>(mImplementation);
    delete timer;
    mImplementation = nullptr;
}

// ---------------------------------------------------------------------------
void
Timer::start(time::Duration duration)
{
    auto* timer = reinterpret_cast<outpost::posix::TimerRaw*>(mImplementation);
    return timer->start(duration);
}

void
Timer::reset()
{
    auto* timer = reinterpret_cast<outpost::posix::TimerRaw*>(mImplementation);
    return timer->reset();
}

void
Timer::cancel()
{
    auto* timer = reinterpret_cast<outpost::posix::TimerRaw*>(mImplementation);
    return timer->cancel();
}

bool
Timer::isRunning()
{
    auto* timer = reinterpret_cast<outpost::posix::TimerRaw*>(mImplementation);
    return timer->isRunning();
}

void
Timer::startTimerDaemonThread(uint8_t priority, size_t stack, CpuMask affinity)
{
    return outpost::posix::TimerRaw::startTimerDaemonThread(priority, stack, affinity);
}
