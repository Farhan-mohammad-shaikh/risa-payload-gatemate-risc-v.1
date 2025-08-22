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

#include "harness/time/time_point.h"

#include "harness/time/duration.h"

using namespace outpost::time;

// ---------------------------------------------------------------------------
template <>
void
outpost::time::PrintTo(SpacecraftElapsedTime timePoint, std::ostream* os)
{
    if (timePoint.timeSinceEpoch() >= outpost::time::Duration::zero())
    {
        PrintTo(timePoint.timeSinceEpoch(), os);
        *os << " after system start";
    }
    else
    {
        PrintTo(-timePoint.timeSinceEpoch(), os);
        *os << " before system start";
    }
}

template <>
void
outpost::time::PrintTo(UnixTime timePoint, std::ostream* os)
{
    // only positive time points can be printed
    if (timePoint.timeSinceEpoch() >= Duration::zero())
    {
        std::array<char, 64U> format;
        std::array<char, 64U> buffer;

        const Duration duration = timePoint.timeSinceEpoch();
        const TickType seconds = duration.seconds();

        struct timeval tv;
        tv.tv_sec = seconds;
        tv.tv_usec = (duration - Seconds(seconds)).microseconds();

        struct tm* tm = std::localtime(&tv.tv_sec);
        assert(tm != nullptr);

        // daylight saving time
        assert(tm->tm_isdst == 0);

        // remove offset to GMT
        tv.tv_sec = tv.tv_sec - tm->tm_gmtoff;
        tm = std::localtime(&tv.tv_sec);
        assert(tm != nullptr);

        {
            size_t ret = strftime(&format[0], format.size(), "%Y-%m-%d %H:%M:%S.%%06u", tm);
            assert(ret > 0U);
        }

        {
            int ret = snprintf(&buffer[0], sizeof(buffer), &format[0], tv.tv_usec);
            assert(ret > 0 && ret < static_cast<int>(sizeof(buffer)));
        }

        *os << &buffer[0];
    }
    else
    {
        outpost::time::PrintTo(-timePoint.timeSinceEpoch(), os);
        *os << " before UNIX epoch";
    }
}
