/*
 * Copyright (c) 2013-2018, Fabian Greif
 * Copyright (c) 2014, Benjamin Weps
 * Copyright (c) 2015, Annika Ofenloch
 * Copyright (c) 2018, Jan-Gerd Mess
 * Copyright (c) 2018, Jan Malburg
 * Copyright (c) 2021, pfef_to
 * Copyright (c) 2024, Passenberg, Felix Constantin
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "time_epoch.h"

namespace outpost
{
namespace time
{

std::atomic<Duration> TimeEpochConverter<SpacecraftElapsedTimeEpoch, GpsEpoch>::offsetToGpsTime =
        Duration::zero();

constexpr Duration TimeEpochConverter<J2000Epoch, UnixEpoch>::offsetUnixtoJ2000;

void
setOffsetBetweenScetAndGps(const SpacecraftElapsedTime& scet, const GpsTime& gps)
{
    TimeEpochConverter<SpacecraftElapsedTimeEpoch, GpsEpoch>::setOffset(scet, gps);
}

}  // namespace time
}  // namespace outpost
