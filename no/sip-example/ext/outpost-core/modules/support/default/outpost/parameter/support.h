/*
 * Copyright (c) 2018, Annika Ofenloch
 * Copyright (c) 2018, Fabian Greif
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SUPPORT_PARAMETER_SUPPORT_H
#define OUTPOST_SUPPORT_PARAMETER_SUPPORT_H

#include <outpost/time/duration.h>

#include <stdint.h>

namespace outpost
{
namespace support
{
namespace parameter
{

/**
 * List of sources of heartbeat ticks.
 *
 * Needs to be adapted by the project to include all thread instances. Threads
 * Instantiated multiple times should each have their own entry.
 *
 * The enumerators should be automatically defined. Otherwise they must follow
 * the rules of the automatic generation (start at zero and always increment by
 * one with each value being unique). In case of gaps in the enumerator
 * identifiers the automatic allocation of resources for each heartbeat source
 * does **not** work.
 */
enum class HeartbeatSource : uint16_t
{
    default0,
    default1,

    /// Dummy entry to detect the number of heartbeat sources. The value
    /// is automatically set by the compiler.
    lastId
};

/**
 * Default heartbeat interval.
 *
 * Used as the polling interval for threads waiting on resources.
 *
 * \see outpost::support::Heartbeat for a more detailed description of the
 *      heartbeat timing.
 */
constexpr outpost::time::Duration defaultHeartbeatInterval = outpost::time::Seconds(10);

/**
 * Tolerance for timing jitter in the heartbeat generation.
 *
 * \see outpost::support::Heartbeat for a more detailed description of the
 *      heartbeat timing.
 */
constexpr outpost::time::Duration heartbeatTolerance = outpost::time::Seconds(1);

}  // namespace parameter
}  // namespace support
}  // namespace outpost

#endif
