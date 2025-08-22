/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2022, Jan-Gerd Mess
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_FREERTOS_SEMAPHORE_HPP
#define OUTPOST_RTOS_FREERTOS_SEMAPHORE_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <outpost/time/duration.h>

namespace outpost
{
namespace rtos
{
/**
 * Counting semaphores.
 *
 * A counting semaphore can be used to control access to a pool of two
 * or more resources. For example, access to three printers could be
 * administered by a semaphore created with an initial count of three.
 *
 * Thread synchronization may be achieved by creating a semaphore with an
 * initial count of zero. One thread waits for the arrival of another
 * thread by issuing a acquire() directive when it reaches
 * a synchronization point. The other thread performs a corresponding
 * release() operation when it reaches its synchronization point, thus
 * unblocking the pending thread.
 *
 * \author    Fabian Greif
 * \ingroup    rtos
 */
class Semaphore
{
public:
    /**
     * Create a Semaphore.
     *
     * \param count
     *         Initial value for the semaphore.
     */
    explicit Semaphore(uint32_t count);

    // disable copy constructor
    Semaphore(const Semaphore& other) = delete;

    // disable assignment operator
    Semaphore&
    operator=(const Semaphore& other) = delete;

    /**
     * Destroy the semaphore and release it's resources.
     *
     * Frees the resources for another semaphore or mutex to be
     * created.
     */
    ~Semaphore();

    /**
     * Decrement the count.
     *
     * Blocks if the count is currently zero until it is incremented
     * by another thread calling the release() method.
     */
    bool
    acquire();

    /**
     * Decrement the count. Not to be used from within ISRs.
     *
     * Same a acquire() but abort after \p timeout milliseconds.
     *
     * \param timeout
     *         Timeout in milliseconds resolution.
     *
     * \return    \c true if the semaphore could be successfully acquired,
     *             \c false in case of an error or timeout.
     */
    bool
    acquire(time::Duration timeout);

    /**
     * Decrement the count. Only to be used from within ISRs.
     *
     * \param taskWoken
     *     Is set to true if a higher priority task was woken by the call.
     *     In that case, a yield shout be called before exiting the ISR.
     *
     * \return True if the count is currently greater than zero and the semaphore could be obtained,
     *     false otherwise.
     */
    bool
    acquireFromISR(bool& hasWokenTask);

    /**
     * Increment the count. Not to be used from within ISRs.
     *
     * This function will never block, but may preempt if an other
     * thread waiting for this semaphore has a higher priority.
     */
    void
    release();

    /**
     * Increment the count. Only to be used from within ISRs.
     *
     * This function will never block, but may preempt if an other
     * thread waiting for this semaphore has a higher priority.
     *
     * \param taskWoken
     *     Is set to true if a higher priority task was woken by the call.
     *     In that case, a yield shout be called before exiting the ISR.
     */
    void
    releaseFromISR(bool& hasWokenTask);

private:
    QueueDefinition* mHandle;
};

/**
 * Binary semaphores.
 *
 * \author    Fabian Greif
 * \ingroup    rtos
 */
class BinarySemaphore
{
public:
    struct State
    {
        enum Type
        {
            acquired,
            released
        };
    };

    /**
     * Create a Semaphore initially released
     */
    BinarySemaphore();

    /**
     * Create a Semaphore.
     *
     * RTEMS has only one pool for mutexes and semaphores. Make sure
     * the maximum number of allocated mutexes and semaphores combined
     * is smaller than the number given via \c CONFIGURE_MAXIMUM_SEMAPHORES.
     *
     * \param initial
     *         Initial value for the semaphore.
     */
    explicit BinarySemaphore(State::Type initial);

    // disable copy constructor
    BinarySemaphore(const BinarySemaphore& other) = delete;

    // disable assignment operator
    BinarySemaphore&
    operator=(const BinarySemaphore& other) = delete;

    /**
     * Destroy the semaphore and release it's resources.
     *
     * Frees the resources for another semaphore or mutex to be
     * created.
     */
    ~BinarySemaphore();

    /**
     * Decrement the count. Not to be called from ISRs.
     *
     * Blocks if the count is currently zero until it is incremented
     * by another thread calling the release() method.
     */
    bool
    acquire();

    /**
     * Decrement the count. Not to be called from ISRs.
     *
     * Same a acquire() but abort after \p timeout milliseconds.
     *
     * \param timeout
     *         Timeout in milliseconds resolution.
     *
     * \return    \c true if the semaphore could be successfully acquired,
     *             \c false in case of an error or timeout.
     */
    bool
    acquire(time::Duration timeout);

    /**
     * Decrement the count. Only to be used from within ISRs.
     *
     * \param taskWoken
     *     Is set to true if a higher priority task was woken by the call.
     *     In that case, a yield shout be called before exiting the ISR.
     *
     * \return True if the count is currently greater than zero and the semaphore could be obtained,
     *     false otherwise.
     */
    bool
    acquireFromISR(bool& hasWokenTask);

    /**
     * Increment the count. Not to be called from ISRs.
     *
     * This function will never block, but may preempt if an other
     * thread waiting for this semaphore has a higher priority.
     */
    void
    release();

    /**
     * Increment the count. Only to be used from within ISRs.
     *
     * This function will never block, but may preempt if an other
     * thread waiting for this semaphore has a higher priority.
     *
     * \param taskWoken
     *     Is set to true if a higher priority task was woken by the call.
     *     In that case, a yield shout be called before exiting the ISR.
     */
    void
    releaseFromISR(bool& hasWokenTask);

private:
    QueueDefinition* mHandle;
};

}  // namespace rtos
}  // namespace outpost

#endif
