/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2022, Jan-Gerd Mess
 * Copyright (c) 2023, Hannah Kirstein
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "timer_raw.h"

#include "internal/time.h"

#include <outpost/rtos/failure_handler.h>

#include <string.h>

using outpost::posix::TimerRaw;

// ---------------------------------------------------------------------------
TimerRaw::TimerRaw(outpost::rtos::Timer* timer,
                   outpost::Callable* object,
                   Function function,
                   const char* name) :
    mTimer(timer), mObject(object), mFunction(function), mTid()
{
    this->createTimer(name);
}

TimerRaw::~TimerRaw()
{
    timer_delete(mTid);
}

void
TimerRaw::start(time::Duration duration)
{
    // initial expiration
    timespec relative = rtos::toRelativeTime(duration);

    itimerspec time;
    time.it_interval.tv_sec = 0;
    time.it_interval.tv_nsec = 0;

    time.it_value.tv_sec = relative.tv_sec;
    time.it_value.tv_nsec = relative.tv_nsec;

    memcpy(&mInterval, &time, sizeof(itimerspec));

    if (timer_settime(mTid, 0, &time, NULL) != 0)
    {
        // Could not set the timer valuell
        rtos::FailureHandler::fatal(rtos::FailureCode::genericRuntimeError(rtos::Resource::timer));
    }
}

void
TimerRaw::reset()
{
    if (timer_settime(mTid, 0, &mInterval, NULL) != 0)
    {
        // Could not set the timer value
        rtos::FailureHandler::fatal(rtos::FailureCode::genericRuntimeError(rtos::Resource::timer));
    }
}

void
TimerRaw::cancel()
{
    struct itimerspec time;

    memset(&time, 0, sizeof(time));

    if (timer_settime(mTid, 0, &time, NULL) != 0)
    {
        // Could not set the timer value
        rtos::FailureHandler::fatal(rtos::FailureCode::genericRuntimeError(rtos::Resource::timer));
    }
}

bool
TimerRaw::isRunning()
{
    itimerspec value;

    if (timer_gettime(mTid, &value) != 0)
    {
        rtos::FailureHandler::fatal(rtos::FailureCode::genericRuntimeError(rtos::Resource::timer));
    }

    bool running = (value.it_value.tv_sec != 0) || (value.it_value.tv_nsec != 0);
    return running;
}

void
TimerRaw::startTimerDaemonThread(uint8_t /*priority*/, size_t /*stack*/, rtos::CpuMask /*affinity*/)
{
    // do nothing here for POSIX, only used for RTEMS
}

void
TimerRaw::createTimer(const char* /*name*/)
{
    sigevent event;
    memset(&event, 0, sizeof(event));

    // Set the sigevent structure to cause the signal to be
    // delivered by creating a new thread.
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = TimerRaw::invokeTimer;
    event.sigev_notify_attributes = NULL;
    event.sigev_value.sival_ptr = this;

    if (timer_create(CLOCK_MONOTONIC, &event, &mTid) != 0)
    {
        // Could not allocate a new timer
        rtos::FailureHandler::fatal(
                rtos::FailureCode::resourceAllocationFailed(rtos::Resource::timer));
    }

    // Disable timer for now
    this->cancel();
}

void
TimerRaw::invokeTimer(union sigval parameter)
{
    TimerRaw* timer = reinterpret_cast<TimerRaw*>(parameter.sival_ptr);
    (timer->mObject->*(timer->mFunction))(timer->mTimer);
}
