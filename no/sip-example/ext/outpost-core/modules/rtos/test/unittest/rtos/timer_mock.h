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

#ifndef UNITTEST_RTOS_TIMER_MOCK_H
#define UNITTEST_RTOS_TIMER_MOCK_H

#include <outpost/posix/timer_raw.h>
#include <outpost/rtos/thread.h>
#include <outpost/rtos/timer.h>
#include <outpost/time.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "unittest/rtos/mock.h"

namespace unittest
{
namespace rtos
{

class TimerImplementation;

// ---------------------------------------------------------------------------
/**
 * \class TimerInterface
 */
class TimerInterface
{
public:
    explicit TimerInterface() = default;

    TimerInterface(const TimerInterface&) = delete;

    TimerInterface&
    operator=(const TimerInterface&) = delete;

    virtual ~TimerInterface() = default;

    virtual void
    start(outpost::time::Duration duration) = 0;

    virtual void
    reset() = 0;

    virtual void
    cancel() = 0;

    virtual bool
    isRunning() = 0;

    virtual void
    startTimerDaemonThread(uint8_t priority, size_t stack, outpost::rtos::CpuMask affinity) = 0;

private:
};

// ---------------------------------------------------------------------------
/**
 * \class TimerMock
 */
class TimerMock : public TimerInterface, public Mock<TimerImplementation>
{
public:
    explicit TimerMock(MockMode mode = defaultMode) : Mock<TimerImplementation>(mode)
    {
    }

    MOCK_METHOD(void, start, (outpost::time::Duration), (override));
    MOCK_METHOD(void, reset, (), (override));
    MOCK_METHOD(void, cancel, (), (override));
    MOCK_METHOD(bool, isRunning, (), (override));
    MOCK_METHOD(void,
                startTimerDaemonThread,
                (uint8_t, size_t, outpost::rtos::CpuMask),
                (override));

private:
};

// ---------------------------------------------------------------------------
/**
 * \class TimerImplementation
 */
class TimerImplementation : public Mockable<TimerMock, outpost::posix::TimerRaw>
{
public:
    explicit TimerImplementation(outpost::rtos::Timer* timer,
                                 outpost::Callable* object,
                                 outpost::rtos::Timer::Function function,
                                 const char* name);

    TimerImplementation(const TimerImplementation&) = delete;

    TimerImplementation&
    operator=(const TimerImplementation&) = delete;

    ~TimerImplementation();

    void
    start(outpost::time::Duration duration);

    void
    reset();

    void
    cancel();

    bool
    isRunning();

    void
    startTimerDaemonThread(uint8_t priority, size_t stack, outpost::rtos::CpuMask affinity);

    /**
     * Manually trigger the execution of the timer.
     */
    void
    executeTimer() const;

    /**
     * Get the name passed to the timer instance via the constructor.
     *
     * \return the name of the timer
     */
    const char*
    getName() const;

private:
    outpost::posix::TimerRaw mTimer;

    outpost::rtos::Timer* const mTimerPointer;
    outpost::Callable* const mObject;
    outpost::rtos::Timer::Function const mFunction;
    const char* mName;
};

}  // namespace rtos
}  // namespace unittest

#endif  // UNITTEST_RTOS_TIMER_MOCK_H
