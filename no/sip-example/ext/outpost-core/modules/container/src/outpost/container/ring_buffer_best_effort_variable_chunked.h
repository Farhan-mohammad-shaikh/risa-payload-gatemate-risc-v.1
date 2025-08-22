/*
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_CONTAINER_RING_BUFFER_BEST_EFFORT_VARIABLE_CHUNKED_H
#define OUTPOST_CONTAINER_RING_BUFFER_BEST_EFFORT_VARIABLE_CHUNKED_H

#include "outpost/container/best_uint_for.h"
#include "outpost/container/ring_buffer_variable_chunked.h"

namespace outpost
{
namespace container
{

// ---------------------------------------------------------------------------
/**
 * Storage class for the buffer.
 *
 * This Storage class assures that there is at least enough space for
 * \c numberOfChunks when the maximum size of a chunk is \c maxChunkSize.
 * This does not mean that is it forbidden to add larger objects. But in this
 * case there is no guarantee that at least \c numberOfChunks
 * will fit into the buffer.
 *
 * \tparam numberOfChunks
 *      minimial number of chunks.
 *
 * \tparam maxChunkSize
 *      maximum size of a chunk
 *
 * \tparam SizeType
 *      size of the buffer in bytes
 */
template <size_t numberOfChunks, size_t maxChunkSize, typename SizeType = BestUIntFor<maxChunkSize>>
class BestEffortVariableChunkedRingBufferStorage
    : public VariableChunkedRingBufferStorage<
              (VariableChunkedRingBuffer<SizeType>::headerSize + maxChunkSize) * numberOfChunks,
              SizeType>
{
public:
    using Type = SizeType;

    static_assert(outpost::Limits<SizeType>::max >= maxChunkSize,
                  "SizeType must hold at least the size of a chunk!");

private:
};

}  // namespace container
}  // namespace outpost

#endif  // OUTPOST_CONTAINER_RING_BUFFER_BEST_EFFORT_VARIABLE_CHUNKED_H
