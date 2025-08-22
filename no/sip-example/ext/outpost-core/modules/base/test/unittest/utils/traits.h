/*
 * Copyright (c) 2013-2018, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_TRAITS_H
#define OUTPOST_TRAITS_H

#include <stdint.h>

namespace outpost
{
/**
 * Arithmetic Traits.
 *
 * Traits give types more information e.g. the number of bytes needed
 * to represent the given type.
 *
 * \author    Fabian Greif
 */
template <typename T>
struct Traits
{
};

template <>
struct Traits<uint8_t>
{
    typedef int8_t SignedType;
    typedef uint8_t UnsignedType;

    static const int width = 1;
};

template <>
struct Traits<int8_t>
{
    typedef int8_t SignedType;
    typedef uint8_t UnsignedType;

    static const int width = 1;
};

template <>
struct Traits<uint16_t>
{
    typedef int16_t SignedType;
    typedef uint16_t UnsignedType;

    static const int width = 2;
};

template <>
struct Traits<int16_t>
{
    typedef int16_t SignedType;
    typedef uint16_t UnsignedType;

    static const int width = 2;
};

template <>
struct Traits<uint32_t>
{
    typedef int32_t SignedType;
    typedef uint32_t UnsignedType;

    static const int width = 4;
};

template <>
struct Traits<int32_t>
{
    typedef int32_t SignedType;
    typedef uint32_t UnsignedType;

    static const int width = 4;
};

template <>
struct Traits<uint64_t>
{
    typedef int64_t SignedType;
    typedef uint64_t UnsignedType;

    static const int width = 8;
};

template <>
struct Traits<int64_t>
{
    typedef int64_t SignedType;
    typedef uint64_t UnsignedType;

    static const int width = 8;
};
}  // namespace outpost

#endif
