/*
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Pascal Pieper
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_HAL_PARAMETER_FILE_SYSTEM_H_
#define OUTPOST_HAL_PARAMETER_FILE_SYSTEM_H_

#include <stddef.h>

namespace outpost
{
namespace hal
{
namespace filesystem
{
static constexpr size_t maxPathLength = 128;
}  // namespace filesystem

}  // namespace hal

}  // namespace outpost

#endif  // OUTPOST_HAL_PARAMETER_FILE_SYSTEM_H_
