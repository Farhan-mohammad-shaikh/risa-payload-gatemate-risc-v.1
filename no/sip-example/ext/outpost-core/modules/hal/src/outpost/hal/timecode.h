/*
 * Copyright (c) 2019, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_HAL_TIMECODE_H_
#define OUTPOST_HAL_TIMECODE_H_

#include <stdint.h>

namespace outpost
{
namespace hal
{
struct TimeCode
{
    // data contained in the corresponding lower bits.
    uint8_t mValue;    // 6-bits
    uint8_t mControl;  // 2-bits
};

}  // namespace hal
}  // namespace outpost

#endif /* MODULES_HAL_SRC_OUTPOST_HAL_TIMECODE_H_ */
