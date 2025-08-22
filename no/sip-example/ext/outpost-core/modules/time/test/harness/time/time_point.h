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

#ifndef HARNESS_TIME_TIME_POINT_H
#define HARNESS_TIME_TIME_POINT_H

#include "harness/time/duration.h"

#include <outpost/time/time_point.h>

#include <cassert>
#include <ctime>
#include <ostream>

/**
 * \note If a test source file (*_test.cpp) includes this header, all other
 *      source file of the same executable should also include this header.
 *      Otherwise there might be a problem with ODR. This is caused by the
 *      fact that gtest uses a template function to print out objects of any
 *      type. The function calls `ToString` if it is available, or it
 *      will call a generic function which prints the bytes of the object.
 *      If one source file includes the header, the compiler will generate the
 *      function which uses `ToString`. And if there is another source file
 *      which does not include the header, it will generate the same function
 *      (signature) but with a different definition (which uses the default
 *      printer function). Therefore, there will be two definition of the
 *      same function. During linking the compiler will choose one of them. If
 *      the generic function is selected, the test will still print durations
 *      byte-wise.
 */

namespace outpost
{
namespace time
{

template <typename Epoch>
void
PrintTo(TimePoint<Epoch> timePoint, std::ostream* os)
{
    PrintTo(timePoint.timeSinceEpoch(), os);
}

template <>
void
PrintTo(SpacecraftElapsedTime timePoint, std::ostream* os);

template <>
void
PrintTo(UnixTime timePoint, std::ostream* os);

}  // namespace time
}  // namespace outpost

#endif  // HARNESS_TIME_TIME_POINT_H
