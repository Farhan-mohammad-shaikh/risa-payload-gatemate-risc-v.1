/*
 * Copyright (c) 2023, Hannah Kirstein
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_PARAMETER_RTOS_H_
#define OUTPOST_RTOS_PARAMETER_RTOS_H_

#include <limits>
#include <stddef.h>
#include <stdint.h>

#include <type_traits>

namespace outpost
{
namespace rtos
{
/**
 * Number of CPUs.
 */
static constexpr uint8_t cpuCount = 2;

/**
 * CPU Mask used for thread affinity assignment.
 * Bit count of CpuMask must be at least cpuCount.
 */
typedef uint8_t CpuMask;
static_assert(sizeof(CpuMask) * 8 >= cpuCount, "CpuMask too small for cpuCount!");

/**
 * Used for affinity assignment to inherit affinity of calling thread.
 */
static constexpr CpuMask inheritFromCaller = 0;

/**
 * Used for affinity assignment to specify the index of a CPU.
 */
enum class CpuAffinity : uint8_t;
static_assert(std::numeric_limits<std::underlying_type<CpuAffinity>::type>::max() >= cpuCount,
              "CpuAffinity type cannot handle number of CPUs requested");

/**
 * operator to use CPU affinities as flags to create a CPU mask.
 */
constexpr inline CpuMask
operator|(CpuAffinity a, CpuAffinity b)
{
    return (1 << (static_cast<CpuMask>(a))) | (1 << (static_cast<CpuMask>(b)));
}

/**
 * Operator to add a CPU affinity to a CPU mask.
 */
constexpr inline CpuMask
operator|(CpuMask a, CpuAffinity b)
{
    return a | (1 << (static_cast<CpuMask>(b)));
}

/**
 * Operator to add a CPU affinity to a CPU mask.
 */
constexpr inline CpuMask
operator|(CpuAffinity a, CpuMask b)
{
    return (1 << (static_cast<CpuMask>(a))) | b;
}

/**
 * Convert a CPU affinity to a CPU mask.
 */
constexpr inline CpuMask
toCpuMask(CpuAffinity a)
{
    return 1 << static_cast<CpuMask>(a);
}

}  // namespace rtos
}  // namespace outpost

#endif /* OUTPOST_RTOS_PARAMETER_RTOS_H_ */
