/*
 * Copyright (c) 2015-2017, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "printer.h"

#include <iomanip>

::std::ostream&
outpost::time::operator<<(::std::ostream& os, const outpost::time::Duration& duration)
{
    return os << duration.microseconds() << " us";
}

::std::ostream&
outpost::time::operator<<(::std::ostream& os, const outpost::time::Seconds& duration)
{
    return os << duration.microseconds() << " us";
}

::std::ostream&
outpost::time::operator<<(::std::ostream& os, const outpost::time::SpacecraftElapsedTime& time)
{
    return os << time.timeSinceEpoch().microseconds() << " us";
}

::std::ostream&
outpost::time::operator<<(::std::ostream& os, const outpost::time::Date& date)
{
    os << static_cast<int>(date.year) << "-";
    os << std::setfill('0') << std::setw(2) << static_cast<int>(date.month) << "-";
    os << std::setfill('0') << std::setw(2) << static_cast<int>(date.day) << "T";
    os << std::setfill('0') << std::setw(2) << static_cast<int>(date.hour) << "-";
    os << std::setfill('0') << std::setw(2) << static_cast<int>(date.minute) << "-";
    os << std::setfill('0') << std::setw(2) << static_cast<int>(date.second);

    return os;
}
