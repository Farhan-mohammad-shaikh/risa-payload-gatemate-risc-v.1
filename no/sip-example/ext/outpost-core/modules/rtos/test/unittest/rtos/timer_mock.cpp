/*
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

#include "unittest/rtos/timer_mock.h"

#include <functional>

using namespace outpost::posix;
using namespace unittest::rtos;

// ---------------------------------------------------------------------------
TimerImplementation::TimerImplementation(outpost::rtos::Timer* timer,
                                         outpost::Callable* object,
                                         outpost::rtos::Timer::Function function,
                                         const char* name) :
    Mockable<TimerMock, outpost::posix::TimerRaw>(this, reinterpret_cast<intptr_t>(timer), &mTimer),
    mTimer(timer, object, function, name),
    mTimerPointer(timer),
    mObject(object),
    mFunction(function),
    mName(name)
{
}

TimerImplementation::~TimerImplementation()
{
}

// ---------------------------------------------------------------------------
void
TimerImplementation::start(outpost::time::Duration duration)
{
    std::function<void()> mockFunction = std::bind(&TimerMock::start, mMock, duration);
    std::function<void()> realFunction = std::bind(&TimerRaw::start, &mTimer, duration);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

void
TimerImplementation::reset()
{
    std::function<void(void)> mockFunction = std::bind(&TimerMock::reset, mMock);
    std::function<void(void)> realFunction = std::bind(&TimerRaw::reset, &mTimer);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

void
TimerImplementation::cancel()
{
    std::function<void(void)> mockFunction = std::bind(&TimerMock::cancel, mMock);
    std::function<void(void)> realFunction = std::bind(&TimerRaw::cancel, &mTimer);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

bool
TimerImplementation::isRunning()
{
    std::function<bool(void)> mockFunction = std::bind(&TimerMock::isRunning, mMock);
    std::function<bool(void)> realFunction = std::bind(&TimerRaw::isRunning, &mTimer);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

void
TimerImplementation::startTimerDaemonThread(uint8_t priority,
                                            size_t stack,
                                            outpost::rtos::CpuMask affinity)
{
    std::function<void()> mockFunction =
            std::bind(&TimerMock::startTimerDaemonThread, mMock, priority, stack, affinity);
    std::function<void()> realFunction =
            std::bind(&TimerRaw::startTimerDaemonThread, priority, stack, affinity);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

// ---------------------------------------------------------------------------
void
TimerImplementation::executeTimer() const
{
    (mObject->*(mFunction))(mTimerPointer);
}

const char*
TimerImplementation::getName() const
{
    return mName;
}
