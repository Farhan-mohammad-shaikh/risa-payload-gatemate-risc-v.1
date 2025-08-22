/*
 * Copyright (c) 2013-2017, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_POW_H
#define OUTPOST_POW_H

namespace outpost
{
/**
 * \author  Fabian Greif
 */
template <int N>
struct PowerOfTwo
{
    static const int base = 2;

    enum
    {
        value = base * PowerOfTwo<N - 1>::value
    };
};

template <>
struct PowerOfTwo<1>
{
    enum
    {
        value = 2
    };
};

template <>
struct PowerOfTwo<0>
{
    enum
    {
        value = 1
    };
};

template <>
struct PowerOfTwo<-1>
{
    enum
    {
        value = 0
    };
};

/**
 * \author  Fabian Greif
 */
template <int Base, int Exponent>
struct Pow
{
    enum
    {
        value = Base * Pow<Base, Exponent - 1>::value
    };
};

template <int Base>
struct Pow<Base, 1>
{
    enum
    {
        value = Base
    };
};

template <int Base>
struct Pow<Base, 0>
{
    enum
    {
        value = 1
    };
};

template <int Base>
struct Pow<Base, -1>
{
    enum
    {
        value = 0
    };
};

}  // namespace outpost

#endif
