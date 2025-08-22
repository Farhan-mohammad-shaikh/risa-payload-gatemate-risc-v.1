/*
 * Copyright (c) 2023-2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_HAL_GPIO_INDEX_H
#define OUTPOST_HAL_GPIO_INDEX_H

#include <stddef.h>
#include <stdint.h>

namespace outpost
{
namespace hal
{

using IndexType = size_t;

/**
 * \class Index
 *
 * Struct for compile time checked index access.
 *
 * \tparam N
 *      the index
 */
template <IndexType N>
struct Index
{
};

}  // namespace hal
}  // namespace outpost

#endif  // OUTPOST_HAL_GPIO_INDEX_H
