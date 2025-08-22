/*
 * Copyright (c) 2020, 2023, Jan-Gerd Mess
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "data_block_sender.h"

#include <outpost/container/reference_queue.h>

namespace outpost
{
namespace compression
{
OneTimeQueueSender::OneTimeQueueSender(container::ReferenceQueueBase<DataBlock>& queue) :
    mOutputQueue(queue)
{
}

bool
OneTimeQueueSender::send(DataBlock& b)
{
    return mOutputQueue.send(b) == outpost::utils::OperationResult::success;
}

}  // namespace compression
}  // namespace outpost
