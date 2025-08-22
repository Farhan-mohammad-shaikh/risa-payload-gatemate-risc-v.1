/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2015, Jan Sommer
 * Copyright (c) 2018, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_RTEMS_PERIODIC_TASK_MANAGER_H
#define OUTPOST_RTOS_RTEMS_PERIODIC_TASK_MANAGER_H

#include "rtems/interval.h"

#include <outpost/rtos/failure_handler.h>

namespace outpost
{
namespace rtos
{
/**
 * Helper class for Rate-Monotonic Scheduling (RMS).
 *
 * \author  Fabian Greif
 * \ingroup rtos
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

    /**
     * Create a new periodic task manager.
     *
     * The periodic task manager has to be created in the thread which will
     * be used later to call the nextPeriod function.
     */
    PeriodicTaskManager();

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
     * \warning
     *      The nextPeriod function must only be called by the thread
     *      that create the PeriodicTaskManager object.
     *
     * \param  period
     *      Length of the next period. Can be different from the
     *      previous one.
     *
     * \retval    Status::running
     *      Period is currently running.
     * \retval  Status::timeout
     *      Last period was missed, this may require some different
     *      handling from the user.
     */
    inline Status::Type
    nextPeriod(time::Duration period)
    {
        rtems_status_code result = rtems_rate_monotonic_period(mId, rtems::getInterval(period));
        if (result == RTEMS_TIMEOUT)
        {
            return Status::timeout;
        }
        else
        {
            // RTEMS_NOT_OWNER_OF_RESOURCE (called from wrong thread)
            // not especially handled has no corresponding status flag exists.
            return Status::running;
        }
    }

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
    inline Status::Type
    status()
    {
        rtems_status_code result = rtems_rate_monotonic_period(mId, RTEMS_PERIOD_STATUS);
        if (result == RTEMS_TIMEOUT)
        {
            return Status::timeout;
        }
        else if (result == RTEMS_NOT_DEFINED)
        {
            return Status::idle;
        }
        else
        {
            return Status::running;
        }
    }

    /**
     * Period measurement is stopped.
     *
     * Can be restarted with the invocation of \c nextPeriod.
     */
    inline void
    cancel()
    {
        rtems_rate_monotonic_cancel(mId);
    }

private:
    rtems_id mId;
};

}  // namespace rtos
}  // namespace outpost

#endif
