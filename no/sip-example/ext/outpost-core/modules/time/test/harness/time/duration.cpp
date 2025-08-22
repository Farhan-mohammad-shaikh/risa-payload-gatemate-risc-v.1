/*
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "duration.h"

#include <cmath>

// ---------------------------------------------------------------------------
void
outpost::time::PrintTo(outpost::time::Duration duration, std::ostream* os)
{
    // 10 Years
    static constexpr outpost::time::Duration epsilon = 10 * 365 * outpost::time::Hours(24);

    if (duration != outpost::time::Duration::maximum())
    {
        const TickType sign = (duration >= outpost::time::Duration::zero()) ? 1 : -1;
        const auto durationWithEpsilon = duration + (sign * epsilon);
        const auto myriads = durationWithEpsilon / outpost::time::Duration::myriad();
        duration = duration - (myriads * outpost::time::Duration::myriad());

        if (myriads != 0)
        {
            *os << myriads << " myriad";

            if (duration > outpost::time::Duration::zero())
            {
                *os << " + ";
            }
            else if (duration < outpost::time::Duration::zero())
            {
                *os << " - ";
                duration = -duration;
            }
            else  // outpost::time::Duration::zero()
            {
                // otherwise the outpost would be
                // `x myriad + zero` instead of `x myriad`
                return;
            }
        }
    }

    if (duration == outpost::time::Duration::zero())
    {
        *os << "zero";
    }
    else if (duration == outpost::time::Duration::maximum())
    {
        *os << "maximum/infinity";
    }
    else
    {
        const outpost::time::Milliseconds milliseconds(duration.milliseconds());
        const outpost::time::Seconds seconds(duration.seconds());
        const outpost::time::Minutes minutes(duration.minutes());
        const outpost::time::Hours hours(duration.hours());

        if (duration == hours)
        {
            // no milliseconds, microseconds, seconds or minutes
            *os << hours.hours() << "h";
        }
        else if (duration == minutes)
        {
            // no milliseconds, microseconds or seconds
            *os << minutes.minutes() << "min";
        }
        else if (duration == seconds)
        {
            // no milliseconds or microseconds
            *os << seconds.seconds() << "s";
        }
        else if (duration == milliseconds)
        {
            // no microseconds
            *os << milliseconds.milliseconds() << "ms";
        }
        else
        {
            // has microseconds
            *os << duration.microseconds() << "us";
        }
    }
}

void
outpost::time::PrintTo(outpost::time::Microseconds duration, std::ostream* os)
{
    PrintTo(static_cast<outpost::time::Duration>(duration), os);
}

void
outpost::time::PrintTo(outpost::time::Milliseconds duration, std::ostream* os)
{
    PrintTo(static_cast<outpost::time::Duration>(duration), os);
}

void
outpost::time::PrintTo(outpost::time::Seconds duration, std::ostream* os)
{
    PrintTo(static_cast<outpost::time::Duration>(duration), os);
}

void
outpost::time::PrintTo(outpost::time::Minutes duration, std::ostream* os)
{
    PrintTo(static_cast<outpost::time::Duration>(duration), os);
}

void
outpost::time::PrintTo(outpost::time::Hours duration, std::ostream* os)
{
    PrintTo(static_cast<outpost::time::Duration>(duration), os);
}
