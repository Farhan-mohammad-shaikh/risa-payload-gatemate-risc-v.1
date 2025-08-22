/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2023, Hannah Kirstein
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Pascal Pieper
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_THREAD_H
#define OUTPOST_RTOS_THREAD_H

#include <outpost/parameter/rtos.h>
#include <outpost/time/duration.h>

namespace outpost
{
namespace rtos
{

class Thread;

}  // namespace rtos
}  // namespace outpost

namespace unittest
{
namespace rtos
{

void
runWrapper(outpost::rtos::Thread&);

}  // namespace rtos
}  // namespace unittest

namespace outpost
{
namespace rtos
{
/**
 * \class Thread
 *
 * Wrapper class for the Thread function of the Operating System.
 *
 * The run()-method of a derived class is invoked in the newly created
 * thread context. The derived class can also hold data members
 * associated with the specific thread.
 */
class Thread
{
public:
    friend void
    unittest::rtos::runWrapper(outpost::rtos::Thread&);

    /// Unique identifier to identify a thread.
    typedef uint32_t Identifier;

    enum FloatingPointSupport
    {
        noFloatingPoint,
        floatingPoint
    };

    /**
     * Initial return value of getIdentifier() before the
     * thread have been started and an associated thread id.
     */
    static const Identifier invalidIdentifier = 0xFFFFFFFF;

    /**
     * Use the default value for the stack size.
     *
     * The default value is depending on the project settings.
     */
    static constexpr size_t defaultStackSize = 0;

    /**
     * Create a new thread.
     *
     * \param priority
     *         Priority of the thread
     * \param stack
     *         Stack size of the thread
     * \param name
     *         Name of the thread. Must not be longer than 16 characters.
     * \param affinity
     *         Cpu affinity
     *
     * \see    rtos::FailureHandler::fatal()
     */
    explicit Thread(uint8_t priority,
                    size_t stack = defaultStackSize,
                    const char* name = 0,
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
     * Only valid after the thread has be started.
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
     * Set priority
     */
    void
    setPriority(uint8_t priority);

    /**
     * Get priority
     */
    uint8_t
    getPriority() const;

    /**
     * Give up the processor but remain in ready state.
     */
    static void
    yield();

    /**
     * Suspend the current thread for the given time.
     *
     * \param timeout
     *         Time to sleep in milliseconds.
     */
    static void
    sleep(::outpost::time::Duration timeout);

private:
    /**
     * Working method of the thread.
     *
     * This method is called after the thread is started. It may never
     * return (endless loop). On a return the fatal error
     * handler will be called.
     */
    virtual void
    run() = 0;

    void* mImplementation;
};

}  // namespace rtos
}  // namespace outpost

#endif
