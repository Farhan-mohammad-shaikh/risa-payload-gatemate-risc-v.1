/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_FREERTOS_TRAITS_H
#define OUTPOST_RTOS_FREERTOS_TRAITS_H

#include <stdint.h>

namespace outpost
{
namespace rtos
{
/**
 * Access the signed and unsigned types of the different types used
 * in FreeRTOS.
 *
 * \author  Fabian Greif
 */
template <typename T>
struct Traits
{
};

template <>
struct Traits<int16_t>
{
    typedef int16_t SignedType;
    typedef uint16_t UnsignedType;
};

template <>
struct Traits<uint16_t>
{
    typedef int16_t SignedType;
    typedef uint16_t UnsignedType;
};

template <>
struct Traits<int32_t>
{
    typedef int32_t SignedType;
    typedef uint32_t UnsignedType;
};

template <>
struct Traits<uint32_t>
{
    typedef int32_t SignedType;
    typedef uint32_t UnsignedType;
};

template <>
struct Traits<int64_t>
{
    typedef int64_t SignedType;
    typedef uint64_t UnsignedType;
};

template <>
struct Traits<uint64_t>
{
    typedef int64_t SignedType;
    typedef uint64_t UnsignedType;
};
}  // namespace rtos
}  // namespace outpost

#endif
