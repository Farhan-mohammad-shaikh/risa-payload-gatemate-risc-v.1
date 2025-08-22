/*
 * Copyright (c) 2013-2018, Fabian Greif
 * Copyright (c) 2015, 2017-2018, Jan Sommer
 * Copyright (c) 2023, Hannah Kirstein
 * Copyright (c) 2023, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "timer.h"

#include "rtems/interval.h"

#include <outpost/rtos/failure_handler.h>
#include <outpost/rtos/mutex_guard.h>

// ----------------------------------------------------------------------------
outpost::rtos::Timer::~Timer()
{
    rtems_timer_delete(mTid);
}

// ----------------------------------------------------------------------------
void
outpost::rtos::Timer::start(time::Duration duration)
{
    MutexGuard lock(mMutex);
    mRunning = true;
    rtems_timer_server_fire_after(
            mTid, rtems::getInterval(duration), &Timer::invokeTimer, (void*) this);
}

void
outpost::rtos::Timer::reset()
{
    MutexGuard lock(mMutex);
    rtems_status_code result = rtems_timer_reset(mTid);
    mRunning = (result == RTEMS_SUCCESSFUL);
}

void
outpost::rtos::Timer::cancel()
{
    MutexGuard lock(mMutex);
    mRunning = false;
    rtems_timer_cancel(mTid);
}

bool
outpost::rtos::Timer::isRunning()
{
    MutexGuard lock(mMutex);
    return mRunning;
}

// ----------------------------------------------------------------------------
void
outpost::rtos::Timer::startTimerDaemonThread(uint8_t priority, size_t stack, CpuMask affinity)
{
    rtems_status_code result = rtems_timer_initiate_server(
            outpost::rtos::Thread::toRtemsPriority(priority), stack, RTEMS_DEFAULT_ATTRIBUTES);

    if (result != RTEMS_SUCCESSFUL)
    {
        FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::timer));
    }

#ifdef RTEMS_SMP

    rtems_id timerId;
    rtems_task_ident(rtems_build_name('T', 'I', 'M', 'E'), RTEMS_SEARCH_ALL_NODES, &timerId);

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);

    for (std::underlying_type<CpuAffinity>::type i = 0; i < cpuCount; i++)
    {
        if (affinity & (1 << i))
        {
            CPU_SET(i, &cpuset);
        }
    }

    // In case no explicit cpu affinity is assigned, the calling tasks affinity is inherited.
    if (CPU_COUNT(&cpuset) == 0)
    {
        rtems_id pTid = rtems_task_self();
        rtems_task_get_affinity(pTid, sizeof(cpuset), &cpuset);
    }

    rtems_task_set_affinity(timerId, sizeof(cpuset), &cpuset);
#else
    (void) affinity;
#endif
}

// ----------------------------------------------------------------------------
void
outpost::rtos::Timer::createTimer(const char* name)
{
    rtems_name taskName = 0;
    if (name == 0)
    {
        // taskName = 0 is not allowed.
        taskName = rtems_build_name('T', 'I', 'M', '-');
        ;
    }
    else
    {
        for (uint_fast8_t i = 0; i < 4; ++i)
        {
            if (name != 0)
            {
                taskName |= *name++;
            }
            taskName <<= 8;
        }
    }

    rtems_status_code result = rtems_timer_create(taskName, &mTid);
    if (result != RTEMS_SUCCESSFUL)
    {
        FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::timer));
    }
}

// ----------------------------------------------------------------------------
void
outpost::rtos::Timer::invokeTimer(rtems_id id, void* parameter)
{
    (void) id;  // not used here

    Timer* timer = reinterpret_cast<Timer*>(parameter);
    {
        MutexGuard lock(timer->mMutex);
        timer->mRunning = false;
    }
    (timer->mObject->*(timer->mFunction))(timer);
}
