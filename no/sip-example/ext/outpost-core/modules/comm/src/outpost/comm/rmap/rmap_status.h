/*
 * Copyright (c) 2017, Muhammad Bassam
 * Copyright (c) 2023, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_COMM_RMAP_STATUS_H_
#define OUTPOST_COMM_RMAP_STATUS_H_

#include <stdint.h>

// #define DEBUG_EN
#ifdef DEBUG_EN
#include <stdio.h>
#define console_out printf
#else
#define console_out(...)
#endif

namespace outpost
{
namespace comm
{
class RmapReplyStatus
{
public:
    enum ErrorStatusCodes
    {
        commandExecutedSuccessfully = 0,
        generalErrorCode = 1,
        unusedRmapPacketType = 2,
        invalidKey = 3,
        invalidDataCrc = 4,
        earlyEOP = 5,
        tooMuchData = 6,
        eep = 7,
        verifyBufferOverrun = 9,
        rmapCommandNotImplemented = 10,
        rmwDataLengthError = 11,
        invalidTargetLogicalAddress = 12,
        unknown = 255
    };

    static void
    replyStatus(uint8_t status);
};
}  // namespace comm
}  // namespace outpost

#endif /* MODULES_HAL_PROTOCOL_RMAP_RMAP_STATUS_H_ */
