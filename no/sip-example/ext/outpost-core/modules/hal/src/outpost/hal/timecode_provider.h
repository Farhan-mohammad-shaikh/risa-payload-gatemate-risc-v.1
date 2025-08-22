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

#ifndef OUTPOST_HAL_TIMECODE_PROVIDER_H_
#define OUTPOST_HAL_TIMECODE_PROVIDER_H_

#include "timecode.h"

#include <outpost/rtos/queue.h>

namespace outpost
{
namespace hal
{
class TimeCodeProvider
{
public:
    TimeCodeProvider() = default;
    virtual ~TimeCodeProvider() = default;

    /**
     * Add a listener for timecode
     * @param queue the queue to add
     * @return false if queue == nullptr or all places for Listener are filled
     */
    virtual bool
    addTimeCodeListener(outpost::rtos::Queue<TimeCode>* queue) = 0;
};

}  // namespace hal
}  // namespace outpost

#endif /* MODULES_HAL_SRC_OUTPOST_HAL_TIMECODE_PROVIDER_H_ */
