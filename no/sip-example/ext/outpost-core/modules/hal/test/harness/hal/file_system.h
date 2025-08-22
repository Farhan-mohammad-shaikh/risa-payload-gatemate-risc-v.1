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

#ifndef HARNESS_HAL_FILE_SYSTEM_H
#define HARNESS_HAL_FILE_SYSTEM_H

#include <outpost/hal/file_system.h>
#include <outpost/utils/expected.h>

#include <gtest/gtest.h>

#include <cassert>
#include <ostream>

namespace outpost
{
namespace hal
{

void
PrintTo(const outpost::hal::FileSystem::Result& result, std::ostream* os);

namespace filesystem
{

void
PrintTo(const outpost::hal::filesystem::Permission& permission, std::ostream* os);

}  // namespace filesystem
}  // namespace hal
}  // namespace outpost

#endif  // HARNESS_HAL_FILE_SYSTEM_H
