/*
 * Copyright (c) 2014-2017, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_TIME_PRINTER_H
#define OUTPOST_TIME_PRINTER_H

#include <outpost/time/date.h>
#include <outpost/time/duration.h>

#include <ostream>

namespace outpost
{
namespace time
{
::std::ostream&
operator<<(::std::ostream& os, const outpost::time::Duration& duration);

::std::ostream&
operator<<(::std::ostream& os, const outpost::time::Seconds& duration);

::std::ostream&
operator<<(::std::ostream& os, const outpost::time::SpacecraftElapsedTime& time);

::std::ostream&
operator<<(::std::ostream& os, const outpost::time::Date& date);

}  // namespace time
}  // namespace outpost

#endif
