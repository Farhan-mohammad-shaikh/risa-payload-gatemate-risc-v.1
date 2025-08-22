/*
 * Copyright (c) 2013-2019, Fabian Greif
 * Copyright (c) 2015-2016, 2018, Jan Sommer
 * Copyright (c) 2021, Sommer, Jan
 * Copyright (c) 2022, Adrian Roeser
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023, Hannah Kirstein
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_RTEMS_THREAD_H
#define OUTPOST_RTOS_RTEMS_THREAD_H

#include "rtems/interval.h"

#include <rtems.h>

#include <outpost/parameter/rtos.h>

#include <stdint.h>

#include <array>
#include <cstddef>

namespace outpost
{
namespace rtos
{
/**
 * Wrapper class for the Thread function of the Operating System.
 *
 * Encapsulates the RTEMS C thread functions and provides C++-style
 * access to them.
 *
 * The run()-method of a derived class is invoked in the newly created
 * thread context. The derived class can also hold data members
 * associated with the specific thread.
 *
 * This class uses the standard RTEMS tasks (not the POSIX threads).
 * Make sure to adapt CONFIGURE_MAXIMUM_TASKS to the number of
 * thread created. If more threads than available are created the
 * FailureHandler::fatal() handler is called.
 *
 * \author    Fabian Greif
 * \ingroup    rtos
 */
class Thread
{
public:
    /// Unique identifier to identify a thread.
    typedef uint32_t Identifier;

    enum FloatingPointSupport
    {
        noFloatingPoint,
        floatingPoint
    };

    /**
     * Use the default value for the stack size.
     *
     * The default value is depending on the project settings.
     */
    static constexpr size_t defaultStackSize = 0;

    /**
     * Create a new thread.
     *
     * Make sure to set CONFIGURE_MAXIMUM_TASKS to the maximum number
     * of executed tasks. Otherwise this constructor will fail to
     * allocate the necessary thread id and call the fatal error
     * handler.
     *
     * \param priority
     *         Thread priority. A higher value represent a higher priority.
     * \param stack
     *         Stack size in bytes. If the stack is smaller than the
     *         default stack size it is replaced with the default size.
     * \param name
     *         Name of the thread. Length is limited to four characters
     *         due to RTEMS internal handling of the string.
     * \param floatingPointSupport
     *      Enable usage of floating point operations for this thread. If
     *      hardware supported floating point is used, each thread which
     *      uses floating point must enable this!
     *
     * \param affinity
     *      CPU mask whose set bits represent CPUs on which the thread should be able to run e.g.
     * thread affinity to those CPUs. Affinities assigned explicitly or affinity inheritance from
     * calling thread is triggered by passing inheritAffinity.
     *
     * \see    rtos::FailureHandler::fatal()
     */
    Thread(uint8_t priority,
           size_t stack = defaultStackSize,
           const char* name = nullptr,
           FloatingPointSupport floatingPointSupport = floatingPoint,
           CpuMask affinity = inheritFromCaller);

    // disable copy constructor
    Thread(const Thread& other) = delete;

    // disable assignment operator
    Thread&
    operator=(const Thread& other) = delete;

    /**
     * Destructor.
     *
     * Frees the resources used by this thread to be used by another
     * thread.
     */
    virtual ~Thread();

    /**
     * Start the execution of the thread.
     *
     * This may preempt the current thread if the thread to be executed
     * has a higher priority.
     */
    void
    start();

    /**
     * Get a unique identifier for this thread.
     *
     * \return  Unique identifier.
     */
    Identifier
    getIdentifier() const;

    /**
     * Get the unique identifier for the currently executed thread.
     *
     * \return  Unique identifier.
     */
    static Identifier
    getCurrentThreadIdentifier();

    /**
     * Set a new priority for the thread.
     *
     * The calling thread may be preempted if it lowers its own
     * priority or raises another thread's priority.
     *
     * From the RTEMS user manual:
     *
     *     If the task specified by id is currently holding any binary
     *     semaphores which use the priority inheritance algorithm,
     *     then the task’s priority cannot be lowered immediately. If
     *     the task’s priority were lowered immediately, then priority
     *     inversion results. The requested lowering of the task’s
     *     priority will occur when the task has released all priority
     *     inheritance binary semaphores. The task’s priority can be
     *     increased regardless of the task’s use of priority
     *     inheritance binary semaphores.
     *
     * \param priority
     *         Thread priority. Higher values represent a higher priority.
     */
    void
    setPriority(uint8_t priority);

    /**
     * Read the priority.
     *
     * \return    Priority of the thread
     */
    uint8_t
    getPriority() const;

    /**
     * Set a new affinity for the thread.
     *
     * \param affinity
     *      CPU mask whose set bits represent CPUs on which the thread should be able to run e.g.
     * thread affinity to those CPUs. Affinities assigned explicitly or affinity inheritance from
     * calling thread is triggered by passing inheritAffinity.
     */
    void
    setAffinity(CpuMask affinity = inheritFromCaller);

    /**
     * Read the thread's affinity as CPU mask.
     * Should never return 0 since every Thread is assigned to at least one CPU.
     *
     * \return     CPU mask with bit number of assigned CPU set to 1
     */
    CpuMask
    getAffinity();

    /**
     * Give up the processor but remain in ready state.
     */
    static inline void
    yield()
    {
        rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
    }

    /**
     * Suspend the current thread for the given time.
     *
     * \param timeout
     *         Time to sleep.
     */
    static inline void
    sleep(::outpost::time::Duration duration)
    {
        if (outpost::time::Duration::myriad() <= duration)
        {
            // getInterval returns 0 as a placeholder for myriad, which
            // fits for most rtems API functions, but not rtems_task_wake_after
            rtems_task_wake_after(std::numeric_limits<rtems_interval>::max());
        }
        else if (outpost::time::Duration::zero() >= duration)
        {
            rtems_task_wake_after(0U);
        }
        else
        {
            rtems_task_wake_after(rtems::getInterval(duration));
        }
    }

    /*
     * RTEMS supports priorities between 1..255. Lower values represent a higher
     * priority, 1 is the highest and 255 the lowest priority.
     *
     * These RTEMS are priorities are mapped to 0..255 priority map with 0
     * representing the lowest priority and 255 the highest. Because RTEMS
     * has only 255 steps, both 0 and 1 represent the same priority.
     */
    static uint8_t
    toRtemsPriority(uint8_t priority);

protected:
    /**
     * Working method of the thread.
     *
     * This method is called after the thread is started. It may never
     * return (endless loop). On a return the fatal error
     * handler will be called.
     */
    virtual void
    run() = 0;

private:
    static void
    wrapper(rtems_task_argument object);

    /**
     * Foward the affinity defined by "mCpuset" to rtems task and thereby set the actual thread/task
     * affinity.
     *
     * If mCpuset is empty e.g. no affinity is assigned, the affinity of the thread in whose
     * context this method is executed is inherited. This enables affinity inheritance when creating
     * and starting new threads.
     */
    void
    applyAffinity();

    /**
     * Define affinity.
     *
     * Sets mCpuset. Does not change the actual thread's/task's affinity until applyAffinity() is
     * called.
     *
     * \param affinity
     *      CPU mask whose set bits represent CPUs on which the thread should be able to run e.g.
     * thread affinity to those CPUs. Affinities assigned explicitly or affinity inheritance from
     * calling thread is triggered by passing inheritAffinity.
     *
     */
    void
    setCpuset(CpuMask affinity);

    rtems_id mTid;

    cpu_set_t mCpuset;  // may only be used by setCpuset and applyAffinity
};

}  // namespace rtos
}  // namespace outpost

#endif
