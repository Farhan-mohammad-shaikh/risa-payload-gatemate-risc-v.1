/*
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

/**
 * \author Fabian Greif, Pascal Pieper
 *
 * \brief FIFO ring buffer data structure.
 */

#ifndef OUTPOST_UTILS_RING_BUFFER_BYTE_ARRAY_H
#define OUTPOST_UTILS_RING_BUFFER_BYTE_ARRAY_H

#include "array_ring_buffer.h"

#include <outpost/container/ring_buffer_chunked.h>

#include <type_traits>

namespace outpost
{
namespace container
{

// Chunked Ringbuffer with Reference to some storage
template <size_t chunkPayloadSize>
using ChunkedRingBufferArray =
        ChunkedRingBuffer<ReferencedArrayAllocator<uint8_t>, chunkPayloadSize>;

template <size_t chunkPayloadSize, size_t numberOfChunks>
class ChunkedRingBufferArrayStorage
    : private ArrayStorage<ChunkedRingBufferArray<chunkPayloadSize>::chunkTotalSize
                                   * numberOfChunks,
                           uint8_t>,
      public ChunkedRingBufferArray<chunkPayloadSize>
{
    static constexpr auto neededBytesTotal =
            ChunkedRingBufferArray<chunkPayloadSize>::chunkTotalSize * numberOfChunks;

public:
    inline ChunkedRingBufferArrayStorage() :
        ArrayStorage<neededBytesTotal, uint8_t>(),
        ChunkedRingBufferArray<chunkPayloadSize>(
                ArrayStorage<neededBytesTotal, uint8_t>::getBuffer())
    {
    }
};

}  // namespace container
}  // namespace outpost

#endif
