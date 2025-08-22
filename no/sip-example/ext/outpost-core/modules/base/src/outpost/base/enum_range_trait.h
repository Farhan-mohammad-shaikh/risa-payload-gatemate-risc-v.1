/*
 * Copyright (c) 2019, Jan-Gerd Mess
 * Copyright (c) 2019, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_BASE_ENUM_TRAIT_H_
#define OUTPOST_BASE_ENUM_TRAIT_H_

namespace outpost
{
/**
 * Trait class for enums that require a range check.
 *
 * \author  Jan-Gerd Mess
 */
template <typename T>
struct EnumRangeTrait
{
    /**
     * Return the minimum integer value allowed for the underlying enum.
     */
    static constexpr T
    min();

    /**
     * Return the maximum integer value allowed for the underlying enum.
     */
    static constexpr T
    max();

    /**
     * Check the given enum value for validity.
     */
    static constexpr bool
    isValid(const T& t);
};

/**
 * Trivial check for enum validity.
 * \author  Jan-Gerd Mess
 */
template <typename T>
inline constexpr bool
isValidEnum(const T& t)
{
    return t >= EnumRangeTrait<T>::min() && t <= EnumRangeTrait<T>::max();
}

}  // namespace outpost

#endif
