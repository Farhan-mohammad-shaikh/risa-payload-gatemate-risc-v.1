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

#ifndef HARNESS_UTILS_EXPECTED_H
#define HARNESS_UTILS_EXPECTED_H

#include <outpost/utils/expected.h>

#include <gtest/gtest.h>

namespace outpost
{

template <typename ValueType, typename ErrorType>
void
PrintTo(const outpost::Expected<ValueType, ErrorType>& value, std::ostream* os)
{
    if (value.has_value())
    {
        ::testing::internal::UniversalPrint(*value, os);
        *os << " (value)";
    }
    else
    {
        ::testing::internal::UniversalPrint(value.error(), os);
        *os << " (error code)";
    }
}

}  // namespace outpost

#endif  // HARNESS_UTILS_EXPECTED_H
