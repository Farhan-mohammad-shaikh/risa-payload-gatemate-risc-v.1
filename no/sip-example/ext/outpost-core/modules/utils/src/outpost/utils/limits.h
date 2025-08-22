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

#ifndef OUTPOST_UTILS_LIMITS_H
#define OUTPOST_UTILS_LIMITS_H

#include <stdint.h>

namespace outpost
{
template <typename T>
struct Limits
{
};

template <>
struct Limits<int8_t>
{
    static const int8_t min = -128;
    static const int8_t max = 127;
};

template <>
struct Limits<uint8_t>
{
    static const uint8_t min = 0;
    static const uint8_t max = 255;
};

template <>
struct Limits<int16_t>
{
    static const int16_t min = -32768;
    static const int16_t max = 32767;
};

template <>
struct Limits<uint16_t>
{
    static const uint16_t min = 0;
    static const uint16_t max = 65535;
};

template <>
struct Limits<int32_t>
{
    static const int32_t min = -2147483647L - 1;
    static const int32_t max = 2147483647;
};

template <>
struct Limits<uint32_t>
{
    static const uint32_t min = 0;
    static const uint32_t max = 4294967295UL;
};

template <>
struct Limits<int64_t>
{
    static const int64_t min = -9223372036854775807LL - 1LL;
    static const int64_t max = 9223372036854775807LL;
};

template <>
struct Limits<uint64_t>
{
    static const uint64_t min = 0;
    static const uint64_t max = 18446744073709551615ULL;
};

}  // namespace outpost

#endif
