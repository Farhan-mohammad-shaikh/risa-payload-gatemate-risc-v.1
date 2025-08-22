/*
 * Copyright (c) 2013-2018, Fabian Greif
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023, Jan-Gerd Mess
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_OPERATION_RESULT_H
#define OUTPOST_UTILS_OPERATION_RESULT_H

namespace outpost
{
namespace utils
{
enum class OperationResult
{
    success,
    timeout,
    invalid,
    outOfBounds,
    inUse,
    genericError
};

}  // namespace utils
}  // namespace outpost

#endif  // OUTPOST_UTILS_OPERATION_RESULT_H
