/*
 * Copyright (c) 2013-2017, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_TIME_H
#define OUTPOST_TIME_H

/**
 * \defgroup    time
 * \brief        Time management
 *
 * In the satellite there are different time representations. TM/TC use
 * CUC (CCSDS Unsegmented time Code) and UTC encoded as CDS (CCSDS Day
 * Segmented time code) while the satellite itself uses a local time
 * representation for calculations. This local time lacks for example a
 * leap-second correction but is very efficient for time calculations.
 *
 * The local time is simply the number of seconds from the start of the
 * satellite.
 *
 * outpost::TimeModel is responsible for converting between local Time and UTC,
 * while spp::Time is used to convert between CDS and UTC and CUC and
 * local time.
 */

#include "time/duration.h"
#include "time/time_epoch.h"
#include "time/time_point.h"

namespace outpost
{
/**
 * \ingroup    time
 */
namespace time
{
}
}  // namespace outpost

#endif
