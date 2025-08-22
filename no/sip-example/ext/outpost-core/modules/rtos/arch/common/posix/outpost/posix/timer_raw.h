/*
 * Copyright (c) 2013-2019, Fabian Greif
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

#ifndef OUTPOST_POSIX_TIMER_RAW_H
#define OUTPOST_POSIX_TIMER_RAW_H

#include <outpost/base/callable.h>
#include <outpost/parameter/rtos.h>
#include <outpost/time/duration.h>

#include <signal.h>
#include <time.h>

namespace outpost
{

namespace rtos
{

class Timer;

}

namespace posix
{
/**
 * Software timer.
 *
 * \author    Fabian Greif
 * \ingroup    posix
 */
class TimerRaw
{
public:
    /**
     * Type of the timer handler function.
     *
     * \param timer
     *         Timer which caused the function to be called. Can be used
     *         to restart the timer.
     */
    typedef void (Callable::*Function)(outpost::rtos::Timer* timer);

    TimerRaw(outpost::rtos::Timer* timer,
             outpost::Callable* object,
             Function function,
             const char* name);

    TimerRaw(const TimerRaw& other) = delete;

    TimerRaw&
    operator=(const TimerRaw& other) = delete;

    /**
     * Delete the timer.
     *
     * If the timer is running, it is automatically canceled. All it's
     * allocated resources are reclaimed and can be used for another
     * timer.
     */
    ~TimerRaw();

    /**
     * Start the timer.
     *
     * If the timer is running it is automatically reset before being
     * initiated.
     *
     * \param duration
     *         Runtime duration.
     */
    void
    start(time::Duration duration);

    /**
     * Reset the timer interval to it's original value when it is
     * currently running.
     */
    void
    reset();

    /**
     * Abort operation.
     *
     * The timer will not fire until the next invocation of reset() or
     * start().
     */
    void
    cancel();

    /**
     * Check whether the timer is currently running.
     *
     * \retval  true    Timer is running
     * \retval  false   Timer has not been started or was stopped.
     */
    bool
    isRunning();

    /**
     * Start the timer daemon.
     *
     * Not used for the POSIX implementation.
     */
    static void
    startTimerDaemonThread(uint8_t priority,
                           size_t stack = 0,
                           rtos::CpuMask affinity = rtos::inheritFromCaller);

private:
    void
    createTimer(const char* name);

    /**
     * Helper function to map the void-pointer to a
     * member function pointer and call the member function.
     */
    static void
    invokeTimer(union sigval parameter);

    /// Object and member function to call when the timer expires.
    rtos::Timer* mTimer;
    Callable* const mObject;
    Function const mFunction;

    timer_t mTid;
    itimerspec mInterval;
};

}  // namespace posix
}  // namespace outpost

#endif
