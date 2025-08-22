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

#ifndef OUTPOST_MODULES_BASE_TEST_UNITTEST_BOUNDED_STRING_HELPER_H
#define OUTPOST_MODULES_BASE_TEST_UNITTEST_BOUNDED_STRING_HELPER_H

#include <outpost/base/bounded_string.h>

#include <gtest/gtest.h>

#include <string>

std::string
to_string(const outpost::BoundedString& from);

template <>
std::string
testing::PrintToString(const outpost::BoundedString& from);

#endif  // OUTPOST_MODULES_BASE_TEST_UNITTEST_BOUNDED_STRING_HELPER_H
