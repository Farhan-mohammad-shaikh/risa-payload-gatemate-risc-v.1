/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2019, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_LOG2_H_
#define OUTPOST_UTILS_LOG2_H_

#include <assert.h>
#include <stddef.h>

namespace outpost
{
constexpr size_t
Log2(size_t n)
{
    return ((n < 2) ? 0 : 1 + Log2(n / 2));
}

}  // namespace outpost

#endif /* OUTPOST_UTILS_LOG2_H_ */
