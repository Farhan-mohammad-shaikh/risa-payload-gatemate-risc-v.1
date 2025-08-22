/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_PERIODIC_TASK_MANAGER_H
#define OUTPOST_RTOS_PERIODIC_TASK_MANAGER_H

#include <outpost/rtos/mutex.h>
#include <outpost/time/duration.h>

#include <time.h>

namespace outpost
{
namespace rtos
{
/**
 * \class PeriodicTaskManager
 *
 * Helper class for Rate-Monotonic Scheduling (RMS).
 */
class PeriodicTaskManager
{
public:
    struct Status
    {
        enum Type
        {
            /// Period has not been started
            idle,

            /// Period is currently running
            running,

            /// Period has expired
            timeout
        };
    };

    PeriodicTaskManager();

    PeriodicTaskManager(const PeriodicTaskManager& other) = delete;

    PeriodicTaskManager&
    operator=(const PeriodicTaskManager& other) = delete;

    ~PeriodicTaskManager();

    /**
     * Start next period.
     *
     * If the PeriodicTaskManager is running, the calling thread will
     * be blocked for the remainder of the outstanding period and,
     * upon completion of that period, the period will be reinitialized
     * with the specified period.
     *
     * If the PeriodicTaskManager is not currently running and has
     * not expired, it is initiated with a length of period ticks and
     * the calling task returns immediately.
     *
     * If the PeriodicTaskManager has expired before the thread invokes
     * the \c nextPeriod method, the period will be initiated with a
     * length of *period* and the calling task returns immediately with
     * a timeout error status.
     *
     * \param  period
     *     Length of the next period. Can be different from the
     *     previous one.
     *
     * \retval    Status::running
     *     Period is currently running.
     * \retval  Status::timeout
     *     Last period was missed, this may require some different
     *     handling from the user.
     */
    Status::Type
    nextPeriod(time::Duration period);

    /**
     * Check the status of the current period.
     *
     * \retval  Status::idle
     *     Period has not been started.
     * \retval    Status::running
     *     Period is currently running.
     * \retval  Status::timeout
     *     Last period was missed, this may require some different
     *     handling from the user.
     */
    Status::Type
    status();

    /**
     * Period measurement is stopped.
     *
     * Can be restarted with the invocation of \c nextPeriod.
     */
    void
    cancel();

private:
    void* mImplementation;
};

}  // namespace rtos
}  // namespace outpost

#endif
