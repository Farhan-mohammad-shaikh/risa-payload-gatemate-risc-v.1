/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2023, Hannah Kirstein
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_NONE_THREAD_H
#define OUTPOST_RTOS_NONE_THREAD_H

#include <outpost/parameter/rtos.h>
#include <outpost/time/duration.h>

#include <stdint.h>

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
     *         Thread priority. RTEMS supports priorities between 1..255.
     *         Lower values represent a higher priority, 1 is the
     *         highest and 255 the lowest priority.
     * \param stack
     *         Stack size in bytes. If the stack is smaller than the
     *         default stack size it is replaced with the default size.
     * \param name
     *         Name of the thread. Length is limited to four characters
     *         due to RTEMS internal handling of the string.
     * \param affinity
     *         Not used for the 'none' implementation.
     *
     * \see    rtos::FailureHandler::fatal()
     */
    Thread(uint8_t priority,
           size_t stack = defaultStackSize,
           const char* name = nullptr,
           FloatingPointSupport floatingPointSupport = noFloatingPoint,
           CpuMask affinity = inheritFromCaller);

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
     *         Thread priority. RTEMS supports priorities between 1..255.
     *         Lower values correspond to higher priorities, e.g. 1 is
     *         the highest priority, 255 the lowest.
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
     * Give up the processor but remain in ready state.
     */
    static inline void
    yield()
    {
    }

    /**
     * Suspend the current thread for the given time.
     *
     * \param timeout
     *         Time to sleep.
     */
    static void
    sleep(::outpost::time::Duration duration);

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
};

}  // namespace rtos
}  // namespace outpost

#endif
