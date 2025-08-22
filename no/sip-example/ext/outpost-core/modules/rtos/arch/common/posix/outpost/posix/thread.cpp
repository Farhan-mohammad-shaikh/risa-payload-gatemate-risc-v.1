/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 * Copyright (c) 2019, Jan Malburg
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

#include "thread.h"

#include <iostream>

// for the access to gettid
#include <sys/syscall.h>
#include <unistd.h>

// for nanosleep()
#include "internal/time.h"

#include <outpost/rtos/failure_handler.h>

#include <time.h>

using outpost::posix::Thread;

void*
Thread::wrapper(void* object)
{
    Thread* thread = reinterpret_cast<Thread*>(object);

    thread->mTid = Thread::getCurrentThreadIdentifier();
    thread->run();

    // Returning from a thread is a fatal error, nothing more to
    // do here than call the fatal error handler.
    rtos::FailureHandler::fatal(rtos::FailureCode::returnFromThread());

    pthread_exit(0);
    return NULL;
}

Thread::Thread(uint8_t,
               size_t,
               const char* name,
               FloatingPointSupport /*floatingPointSupport*/,
               rtos::CpuMask /*affinity*/) :
    mIsRunning(false), mPthreadId(), mTid(), mName()
{
    if (name != 0)
    {
        mName = std::string(name);
    }
}

Thread::~Thread()
{
    if (mIsRunning)
    {
        pthread_cancel(mPthreadId);
        pthread_join(mPthreadId, NULL);
    }
}

Thread::Identifier
Thread::getIdentifier() const
{
    return 0;
}

Thread::Identifier
Thread::getCurrentThreadIdentifier()
{
#ifdef SYS_gettid
    pid_t tid = syscall(SYS_gettid);
    return tid;
#else
#error "SYS_gettid unavailable on this system"
#endif
}

void
Thread::start()
{
    mIsRunning = true;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    int ret = pthread_create(&mPthreadId, &attr, &Thread::wrapper, reinterpret_cast<void*>(this));
    if (ret != 0)
    {
        rtos::FailureHandler::fatal(
                rtos::FailureCode::resourceAllocationFailed(rtos::Resource::thread));
    }

    if (!mName.empty())
    {
        int result = pthread_setname_np(mPthreadId, mName.c_str());
        if (result != 0)
        {
            std::cerr << "Failed to set thread name: '" << mName << "': " << result << std::endl;
        }
    }

    pthread_attr_destroy(&attr);
}

void
Thread::setPriority(uint8_t priority)
{
    // No priority defined for POSIX!
    (void) priority;
}

uint8_t
Thread::getPriority() const
{
    return 0;
}

void
Thread::yield()
{
    // On Linux, this call always succeeds
    sched_yield();
}

void
Thread::sleep(::outpost::time::Duration timeout)
{
    const timespec deadline = rtos::toAbsoluteTime(CLOCK_MONOTONIC, timeout);
    rtos::sleepUntilAbsoluteTime(CLOCK_MONOTONIC, deadline);
}
