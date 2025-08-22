/*
 * Copyright (c) 2023-2024, Felix Passenberg
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SIP_OPERATION_RESULT_H_
#define OUTPOST_SIP_OPERATION_RESULT_H_

namespace outpost
{
namespace sip
{
enum class OperationResult
{
    success,

    // read results
    lengthErrorTooSmall,
    lengthErrorEndOfFrame,
    crcError,
    // write results
    bufferError,
    notFinalized,
    // coordinator
    transmitError,
    responseError,
    workerIdError,
    responseTypeError,
    // transport codes
    transportError,
    // bufferError,
    timeout,
    serialStopped,

};
}
}  // namespace outpost

#endif  // OUTPOST_SIP_OPERATION_RESULT_H_
