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

#ifndef OUTPOST_RTOS_TIMER_H
#define OUTPOST_RTOS_TIMER_H

#include <outpost/base/callable.h>
#include <outpost/parameter/rtos.h>
#include <outpost/time/duration.h>

namespace outpost
{
namespace rtos
{
/**
 * \class Timer
 */
class Timer
{
public:
    /**
     * Type of the timer handler function.
     *
     * \param timer
     *         Timer which caused the function to be called. Can be used
     *         to restart the timer.
     */
    typedef void (Callable::*Function)(Timer* timer);

    template <typename T>
    struct TimerFunction
    {
        typedef void (T::*type)(Timer* timer);
    };

    /**
     * Create a timer.
     *
     * \param object
     *         Instance to with the function to be called belongs. Must
     *         be sub-class of outpost::rtos::Callable.
     * \param function
     *         Member function of \p object to call when the timer
     *         expires.
     * \param name
     *         Name of the timer. Maximum length is four characters. Longer
     *         names will be truncated.
     *
     * \see    outpost::Callable
     */
    template <typename T>
    Timer(T* object, typename TimerFunction<T>::type function, const char* name = "TIM-");

    // disable copy-constructor and assignment operator
    Timer(const Timer& other) = delete;

    Timer&
    operator=(const Timer& other) = delete;

    /**
     * Delete the timer.
     *
     * If the timer is running, it is automatically canceled. All it's
     * allocated resources are reclaimed and can be used for another
     * timer.
     */
    ~Timer();

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
     */
    static void
    startTimerDaemonThread(uint8_t priority,
                           size_t stack = 0,
                           CpuMask affinity = inheritFromCaller);

private:
    Timer(outpost::Callable* object, Function function, const char* name);

    void* mImplementation;
};

// ----------------------------------------------------------------------------
// Implementation of the template constructor
template <typename T>
Timer::Timer(T* object, typename TimerFunction<T>::type function, const char* name) :
    Timer(reinterpret_cast<Callable*>(object), reinterpret_cast<Function>(function), name)
{
}

}  // namespace rtos
}  // namespace outpost

#endif
