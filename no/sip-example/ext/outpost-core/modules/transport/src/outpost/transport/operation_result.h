/*
 * Copyright (c) 2013-2018, Fabian Greif
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023, Jan-Gerd Mess
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_TRANSPORT_OPERATION_RESULT_H_
#define OUTPOST_TRANSPORT_OPERATION_RESULT_H_

namespace outpost
{
namespace transport
{
enum class OperationResult
{
    success,

    // frame_coder
    notComplete,
    encodingError,
    decodingError,
    bufferError,
    // frame_transport
    timeout,
    streamStopped,

};
}
}  // namespace outpost

#endif  // OUTPOST_TRANSPORT_OPERATION_RESULT_H_
