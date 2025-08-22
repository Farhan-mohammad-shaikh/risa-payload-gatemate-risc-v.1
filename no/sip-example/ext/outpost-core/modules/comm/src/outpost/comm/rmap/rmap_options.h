/*
 * Copyright (c) 2019, Jan Malburg
 * Copyright (c) 2022, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_COMM_RMAP_RMAP_OPTIONS_H_
#define OUTPOST_COMM_RMAP_RMAP_OPTIONS_H_

#include <stdint.h>

namespace outpost
{
namespace comm
{
struct RMapOptions
{
    constexpr RMapOptions() : mIncrementMode(true), mVerifyMode(true), mReplyMode(true){};
    constexpr RMapOptions(bool increment, bool verify, bool reply) :
        mIncrementMode(increment), mVerifyMode(verify), mReplyMode(reply){};

    bool mIncrementMode;
    bool mVerifyMode;
    bool mReplyMode;
};

}  // namespace comm
}  // namespace outpost

#endif /* MODULES_COMM_SRC_OUTPOST_COMM_RMAP_RMAP_OPTIONS_H_ */
