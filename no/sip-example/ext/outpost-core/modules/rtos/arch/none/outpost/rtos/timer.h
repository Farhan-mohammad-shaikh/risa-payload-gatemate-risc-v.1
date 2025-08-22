/*
 * Copyright (c) 2013-2019, Fabian Greif
 * Copyright (c) 2022, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_NONE_TIMER_H
#define OUTPOST_RTOS_NONE_TIMER_H

#include <outpost/base/callable.h>
#include <outpost/time/duration.h>

#include <stddef.h>

namespace outpost
{
namespace rtos
{
/**
 * Software timer.
 *
 * The timer callback functions are called in the context of the
 * timer server thread.
 *
 * The Timer Server thread is responsible for executing the timer
 * service routines associated with all task-based timers.
 *
 * The Timer Server is designed to remain blocked until a task-based
 * timer fires. This reduces the execution overhead of the Timer Server.
 *
 * \author    Fabian Greif
 * \ingroup    rtos
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
    Timer(T* object, void (T::*function)(Timer* timer), const char* name = "TIM-");

    /**
     * Delete the timer.
     *
     * If the timer is running, it is automatically canceled. All it's
     * allocated resources are reclaimed and can be used for another
     * timer.
     */
    ~Timer() = default;

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

private:
    void
    createTimer(const char* name);

    /// Object and member function to call when the timer expires.
    Callable* const mObject;
    Function const mFunction;
};

// ----------------------------------------------------------------------------
// Implementation of the template constructor
template <typename T>
Timer::Timer(T* object, void (T::*function)(Timer* timer), const char* name) :
    mObject(reinterpret_cast<Callable*>(object)), mFunction(reinterpret_cast<Function>(function))
{
    this->createTimer(name);
}

}  // namespace rtos
}  // namespace outpost

#endif
