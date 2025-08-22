/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2014, Benjamin Weps
 * Copyright (c) 2015, Annika Ofenloch
 * Copyright (c) 2024, Passenberg, Felix Constantin
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_TIME_TIME_EPOCH_H
#define OUTPOST_TIME_TIME_EPOCH_H

#include "time_point.h"

namespace outpost
{
namespace time
{
/**
 * International Atomic Time (TAI).
 *
 * Time since 1958-01-01T00:00:00Z, no leap seconds
 */
class TaiEpoch
{
};

/**
 * Time since 1980-01-06T00:00:00Z, no leap seconds
 */
class GpsEpoch
{
};

/**
 * Time since 1970-01-01T00:00:00Z, ignores leap seconds.
 */
class UnixEpoch
{
};

/**
 * Time since 1999-08-22T00:00:00Z with leap seconds
 */
class GalileoSystemTimeEpoch
{
};

/**
 * Time since 2000-01-01T11:58:55Z
 */
class J2000Epoch
{
};

/**
 * Time since start of OBC (SCET).
 *
 * Strictly monotonous counter.
 */
class SpacecraftElapsedTimeEpoch
{
};

typedef TimePoint<SpacecraftElapsedTimeEpoch> SpacecraftElapsedTime;
typedef TimePoint<GpsEpoch> GpsTime;
typedef TimePoint<TaiEpoch> AtomicTime;
typedef TimePoint<UnixEpoch> UnixTime;
typedef TimePoint<J2000Epoch> J2000Time;

// forward declaration
template <typename Epoch>
class TimePoint;

template <typename From, typename To>
class TimeEpochConverter
{
public:
    static constexpr TimePoint<To>
    convert(const TimePoint<From>& from);
};

/**
 * Set offset between SCET and GPS time.
 *
 * Convenience function for
 * TimeEpochConverter<SpacecraftElapsedTimeEpoch, GpsEpoch>::setOffset(...)
 */
void
setOffsetBetweenScetAndGps(const SpacecraftElapsedTime& scet, const GpsTime& gps);

}  // namespace time
}  // namespace outpost

#include "time_epoch_impl.h"

#endif
