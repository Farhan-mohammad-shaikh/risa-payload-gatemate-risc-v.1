/*
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_BEST_UINT_FOR_H
#define OUTPOST_UTILS_BEST_UINT_FOR_H

#include <outpost/utils/limits.h>

#include <stddef.h>
#include <stdint.h>

namespace outpost
{
namespace container
{
namespace internal
{

// ---------------------------------------------------------------------------
/**
 * Mapping from a group (numeric value) to the integer type which is the
 * smallest to represent the group. \see \c ValueToGroup.
 */
template <size_t group>
struct GroupToType
{
};

template <>
struct GroupToType<0>
{
    using Type = uint8_t;
};

template <>
struct GroupToType<1>
{
    using Type = uint16_t;
};

template <>
struct GroupToType<2>
{
    using Type = uint32_t;
};

template <>
struct GroupToType<3>
{
    using Type = uint64_t;
};

// ---------------------------------------------------------------------------
/**
 * This struct is used to calculated the group of a given \c chunkSize.
 * \see \c BestUIntFor.
 */
template <size_t chunkSize>
struct ValueToGroup
{
    static constexpr size_t group = (chunkSize > outpost::Limits<uint8_t>::max)
                                    + (chunkSize > outpost::Limits<uint16_t>::max)
                                    + (chunkSize > outpost::Limits<uint32_t>::max);
};

}  // namespace internal

// ---------------------------------------------------------------------------
/**
 * Evaluates the smallest type which can hold the chunk size. This allows
 * the user to set the maximum chunk size and get automatically the best
 * type to hold the size of a chunk.
 *
 * \tparam upperLimit
 *      max value of the integer
 */
template <size_t upperLimit>
using BestUIntFor = typename internal::GroupToType<internal::ValueToGroup<upperLimit>::group>::Type;

}  // namespace container
}  // namespace outpost

#endif  // OUTPOST_UTILS_BEST_UINT_FOR_H
