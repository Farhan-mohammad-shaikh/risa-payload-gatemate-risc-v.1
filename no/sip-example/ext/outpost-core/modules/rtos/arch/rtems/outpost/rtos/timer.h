/*
 * Copyright (c) 2013-2019, Fabian Greif
 * Copyright (c) 2021, Sommer, Jan
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2023, Hannah Kirstein
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_RTEMS_TIMER_H
#define OUTPOST_RTOS_RTEMS_TIMER_H

#include "mutex.h"

#include <rtems.h>

#include <outpost/base/callable.h>
#include <outpost/parameter/rtos.h>
#include <outpost/rtos/thread.h>
#include <outpost/time/duration.h>

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
 * \warning    You need to call Timer::startTimerDaemonThread() somewhere
 *             in your start-up code. Otherwise the timers won't work!
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
     * Example:
     * \code
     * class A : public outpost::rtos::Callable
     * {
     * public:
     *     void
     *     function(outpost::rtos::Timer*)
     *     {
     *     }
     * };
     *
     * A a;
     *
     * outpost::rtos::Timer timer(&a, &A::function);
     * \endcode
     *
     * Example 2:
     * \code
     * class B : public outpost::rtos::Callable
     * {
     * public:
     *     B() :
     *         timer(this, &B::function, "TIM2");
     *     {
     *     }
     *
     *     void
     *     function(outpost::rtos::Timer*)
     *     {
     *     }
     *
     * private:
     *     outpost::rtos::Timer timer;
     * };
     * \endcode
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
     *
     * This function initiates the Timer Server thread. This thread is
     * responsible for executing all timers.
     *
     * As it is a normal thread it was to be considered when
     * configuring CONFIGURE_MAXIMUM_TASKS.
     *
     * \param priority
     *         Thread priority.
     *         Supported are values between 1..255.
     *         Lower values represent a lower priority. To ensure that
     *         the handler-function is called at the exact time it is
     *         a good idea to give the timer daemon thread a high
     *         priority (high value).
     *         Note: The priority assigned here is converted to the
     *               internal priority representation of RTEMS.
     * \param stack
     *         Stack size in bytes. If the stack is smaller than the
     *         default stack size it is replaced with the default size.
     *
     * \param affinity
     *         CPU mask whose set bits represent CPUs on which the thread should be able to run e.g.
     * thread affinity to those CPUs. Affinities assigned explicitly or affinity inheritance from
     * calling thread is triggered by passing inheritAffinity.
     */
    static void
    startTimerDaemonThread(uint8_t priority,
                           size_t stack = 0,
                           CpuMask affinity = inheritFromCaller);

private:
    void
    createTimer(const char* name);

    /**
     * Helper function to map the void-pointer to a
     * member function pointer and call the member function.
     */
    static void
    invokeTimer(rtems_id id, void* timer);

    /// Object and member function to call when the timer expires.
    Callable* const mObject;
    Function const mFunction;

    Mutex mMutex;
    bool mRunning;

    rtems_id mTid;
};

// ----------------------------------------------------------------------------
// Implementation of the template constructor
template <typename T>
Timer::Timer(T* object, void (T::*function)(Timer* timer), const char* name) :
    mObject(reinterpret_cast<Callable*>(object)),
    mFunction(reinterpret_cast<Function>(function)),
    mRunning(false),
    mTid()
{
    this->createTimer(name);
}

}  // namespace rtos
}  // namespace outpost

#endif
