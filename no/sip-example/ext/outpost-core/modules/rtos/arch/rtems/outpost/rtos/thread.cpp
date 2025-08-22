/*
 * Copyright (c) 2013-2018, Fabian Greif
 * Copyright (c) 2015, 2018, Jan Sommer
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

#include "thread.h"

#include <outpost/rtos/failure_handler.h>

#include <stdio.h>

#include <type_traits>

using outpost::rtos::Thread;

// ----------------------------------------------------------------------------
void
Thread::wrapper(rtems_task_argument object)
{
    Thread* thread = reinterpret_cast<Thread*>(object);
    thread->run();

    // Returning from a RTEMS thread is a fatal error, nothing more to
    // do here than call the fatal error handler.
    rtos::FailureHandler::fatal(rtos::FailureCode::returnFromThread());
}

uint8_t
Thread::toRtemsPriority(uint8_t priority)
{
    if (priority == 0)
    {
        return 255;
    }

    return (256 - static_cast<int16_t>(priority));
}

static uint8_t
fromRtemsPriority(uint8_t rtemsPriority)
{
    // a RTEMS priority of 0 is invalid and not checked here.
    return (256 - static_cast<int16_t>(rtemsPriority));
}

// ----------------------------------------------------------------------------
Thread::Thread(uint8_t priority,
               size_t stack,
               const char* name,
               FloatingPointSupport floatingPointSupport,
               CpuMask affinity)
{
#ifdef RTEMS_SMP
    setCpuset(affinity);
#else
    (void) affinity;
#endif

    rtems_name taskName = 0;
    if (name == 0)
    {
        // taskName = 0 is not allowed.
        taskName = rtems_build_name('T', 'H', 'D', '-');
    }
    else
    {
        for (uint_fast8_t i = 0; i < 4; ++i)
        {
            taskName <<= 8;
            if (name != 0)
            {
                taskName |= *name++;
            }
        }
    }

    rtems_task_priority rtemsPriority = toRtemsPriority(priority);

    uint32_t attributes = RTEMS_DEFAULT_ATTRIBUTES;
    if (floatingPointSupport == floatingPoint)
    {
        attributes |= RTEMS_FLOATING_POINT;
    }

    rtems_status_code status = rtems_task_create(taskName,
                                                 rtemsPriority,
                                                 stack,
                                                 RTEMS_DEFAULT_MODES | RTEMS_TIMESLICE,
                                                 attributes,
                                                 &mTid);

    if (status != RTEMS_SUCCESSFUL)
    {
        FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::thread));
    }
}

Thread::~Thread()
{
    rtems_task_delete(mTid);
}

// ----------------------------------------------------------------------------
Thread::Identifier
Thread::getIdentifier() const
{
    return mTid;
}

Thread::Identifier
Thread::getCurrentThreadIdentifier()
{
    rtems_id current;
    if (rtems_task_ident(RTEMS_SELF, OBJECTS_SEARCH_LOCAL_NODE, &current) != RTEMS_SUCCESSFUL)
    {
        return 0;
    }

    return current;
}

// ----------------------------------------------------------------------------
void
Thread::start()
{
#ifdef RTEMS_SMP
    applyAffinity();
#endif

    rtems_task_start(mTid, wrapper, reinterpret_cast<rtems_task_argument>(this));
}

// ----------------------------------------------------------------------------
void
Thread::setPriority(uint8_t priority)
{
    rtems_task_priority old;
    rtems_task_set_priority(mTid, toRtemsPriority(priority), &old);
}

uint8_t
Thread::getPriority() const
{
    rtems_task_priority priority;
    rtems_task_set_priority(mTid, RTEMS_CURRENT_PRIORITY, &priority);

    return fromRtemsPriority(priority);
}

void
Thread::setCpuset(CpuMask affinity)
{
#ifdef RTEMS_SMP
    // remove all CPUs from cpuset
    CPU_ZERO(&mCpuset);

    for (std::underlying_type<CpuAffinity>::type i = 0; i < cpuCount; i++)
    {
        if (affinity & (1 << i))
        {
            CPU_SET(i, &mCpuset);
        }
    }
#else
    (void) affinity;
#endif
    return;
}

void
Thread::applyAffinity()
{
#ifdef RTEMS_SMP

    // In case no explicit cpu affinity is assigned, the calling tasks affinity is inherited.
    // This changes the default task/thread affinity defined by rtems from "all cpu's" to an
    // affinity inheritance to reduce the need of affinity assignments for task started in the
    // context of other tasks and enables system wide task affinity control by setting the init
    // tasks affinity (not done via thread class api but explicitly in init task via rtems function
    // calls).
    if (CPU_COUNT(&mCpuset) == 0)
    {
        rtems_id pTid = rtems_task_self();
        rtems_task_get_affinity(pTid, sizeof(mCpuset), &mCpuset);
    }

    rtems_task_set_affinity(mTid, sizeof(mCpuset), &mCpuset);
#endif
    return;
}

void
Thread::setAffinity(CpuMask affinity)
{
#ifdef RTEMS_SMP
    setCpuset(affinity);
    applyAffinity();
#else
    (void) affinity;
#endif
    return;
}

outpost::rtos::CpuMask
Thread::getAffinity()
{
    CpuMask cpuMask = 0;

#ifdef RTEMS_SMP
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);

    rtems_task_get_affinity(mTid, sizeof(cpuset), &cpuset);

    for (std::underlying_type<CpuAffinity>::type i = 0; i < cpuCount; i++)
    {
        cpuMask |= CPU_ISSET(i, &cpuset) << i;
    }
#endif
    return cpuMask;
}
