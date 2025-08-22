/*
 * Copyright (c) 2016-2017, Fabian Greif
 * Copyright (c) 2022, Passenberg, Felix Constantin
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Tepe, Alexander
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_TIME_TIMEOUT_H
#define OUTPOST_TIME_TIMEOUT_H

#include <outpost/time/clock.h>
#include <outpost/time/duration.h>

namespace outpost
{
namespace time
{
/**
 * Polling based timeout class.
 *
 * Allows to track that a certain amount of time has passed.
 *
 * \author  Fabian Greif
 */
class Timeout
{
public:
    enum class State
    {
        stopped,
        armed,
        expired
    };

    /**
     * \brief Create a stopped timeout.
     *
     * The timeout can later be started by calling the `restart` method.
     */
    explicit constexpr Timeout(const outpost::time::Clock& clock);

    /**
     * \brief Create a new timeout and start it immediately.
     */
    // constexpr // can not be constexpr under gcc because of virtual clock::now() (don't ask why
    // getState is constexpr!)
    inline Timeout(const outpost::time::Clock& clock, const outpost::time::Duration& time);

    /**
     * \brief restart (or start) timeout
     *
     * \param time time offset to count towards, cannot be negative
     */
    // constexpr // can not be constexpr under gcc because of virtual clock::now() (don't ask why
    // getState is constexpr!)
    inline void
    restart(const outpost::time::Duration& time);

    /**
     * \brief Updates a running Timeouts EndTime.
     *
     * Checks if a Timeout expired before updating the EndTime
     * A stopped timer is not restarted
     *
     * \param time time offset to cound towards, cannot be negative
     */
    constexpr void
    changeDuration(const outpost::time::Duration& time);

    /**
     * \brief Stop the timeout.
     *
     * brief set the internal state to `stopped`
     *
     * A stopped timeout will never expire.
     */
    constexpr void
    stop();

    /**
     * \brief get the current state
     *
     * Check if timeout is currently running and state is "armed",
     * does not update the internal state
     *
     * \return current state of timeout
     */
    constexpr State
    getState() const;

    /**
     * \brief Check if timeout is in "stopped" state
     *
     * \retval true, if state is "stopped"
     * \retval false, else
     */
    constexpr bool
    isStopped() const;

    /**
     * \brief Checks, if timeout is in "expired" state
     *
     * \retval true, if state is "expired"
     * \retval false, else
     */
    constexpr bool
    isExpired() const;

    /**
     * Checks if a timeout is expired,
     * also updates the parameter with the remaining time:
     *      if timeout has expired -> remaining = zero()
     *      else: remaining -> duration until expiration
     *
     * \param[out] remaining will be updated with the remaining time, if timeout has not expired
     */
    // not constexpr because now() is called
    inline bool
    isExpired(outpost::time::Duration& remaining) const;

    /**
     * \brief checks if state is "armed"
     *
     * \retval true, if state is "armed"
     * \retval false, else
     */
    constexpr bool
    isArmed() const;

    /**
     * \brief Get remaining time until timeout expires
     *
     * if the time has already run out, `Duration::zero()` will be returned
     *
     * \retval duration until timeout runs out, if time remains
     * \retval 0, else
     */
    // again not constexpr because clock::now() is called.
    inline outpost::time::Duration
    getRemainingTime() const;

protected:
    /**
     * \brief checks state and updates if necessary
     *
     * Checks if clock has run out nominally and sets the state to `expired` if so
     */
    constexpr void
    updateState();

private:
    outpost::time::SpacecraftElapsedTime mStartTime;
    outpost::time::SpacecraftElapsedTime mEndTime;
    State mState;
    const outpost::time::Clock& mClock;
};

}  // namespace time
}  // namespace outpost

#include "timeout_impl.h"

#endif
