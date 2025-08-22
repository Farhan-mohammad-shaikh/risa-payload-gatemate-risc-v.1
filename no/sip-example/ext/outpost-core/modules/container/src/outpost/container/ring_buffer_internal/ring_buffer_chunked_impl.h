/*
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Pascal Pieper
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
 * \file ring_buffer_chunked_impl.h
 * \author Fabian Greif, Pascal Pieper
 *
 * \brief Implementation details of the chunked ring buffer
 */

#ifndef OUTPOST_UTILS_RING_BUFFER_INTERNAL_RING_BUFFER_CHUNKED_IMPL_H
#define OUTPOST_UTILS_RING_BUFFER_INTERNAL_RING_BUFFER_CHUNKED_IMPL_H

#include "../ring_buffer_chunked.h"

#include <outpost/base/testing_assert.h>
#include <outpost/container/ring_buffer.h>
#include <outpost/storage/serialize.h>

namespace outpost
{
namespace container
{

template <class Allocator, size_t chunkPayloadSize>
template <typename... Args>
ChunkedRingBuffer<Allocator, chunkPayloadSize>::ChunkedRingBuffer(Args&&... args) :
    RingBuffer<Allocator, uint8_t>(std::forward<Args>(args)...)
{
}

template <class Allocator, size_t chunkPayloadSize>
constexpr size_t
ChunkedRingBuffer<Allocator, chunkPayloadSize>::getChunkSize()
{
    return chunkPayloadSize;
}

template <class Allocator, size_t chunkPayloadSize>
constexpr size_t
ChunkedRingBuffer<Allocator, chunkPayloadSize>::getFreeSlots() const
{
    return (RingBuffer<Allocator, uint8_t>::capacity() / chunkTotalSize) - getUsedSlots();
}

template <class Allocator, size_t chunkPayloadSize>
constexpr size_t
ChunkedRingBuffer<Allocator, chunkPayloadSize>::getUsedSlots() const
{
    return RingBuffer<Allocator, uint8_t>::getAvailableElements() / chunkTotalSize;
}

template <class Allocator, size_t chunkPayloadSize>
bool
ChunkedRingBuffer<Allocator, chunkPayloadSize>::append(outpost::Slice<const uint8_t> data,
                                                       uint8_t flags,
                                                       bool zeroOut)
{
    // Key assumption: Element either fits completely or no single byte at all
    OUTPOST_ASSERT((RingBuffer<Allocator, uint8_t>::capacity()) % chunkTotalSize == 0,
                   "Allocator needs to be divisible by chunk size.");

    if (data.getNumberOfElements() > chunkPayloadSize)
    {
        return false;
    }

    ChunkHeader header = {};  // zero out
    header.flags = flags;
    header.length = data.getNumberOfElements();
    const auto& headerBuf = serializeHeader(header);

    if (RingBuffer<Allocator, uint8_t>::append(outpost::asSlice(headerBuf))
        != RingBuffer<Allocator, uint8_t>::OperationResult::success)
    {
        return false;
    }

    OUTPOST_ASSERT((RingBuffer<Allocator, uint8_t>::getFreeElements())
                           >= data.getNumberOfElements(),
                   "Can't append data even though the initial append was successful");
    RingBuffer<Allocator, uint8_t>::append(data);

    // Append dummy data to remain in static element size
    if (data.getNumberOfElements() < chunkPayloadSize)
    {
        size_t remainingBytes = chunkPayloadSize - data.getNumberOfElements();

        if (!zeroOut)
        {
            // faster version without actually writing bytes
            OUTPOST_ASSERT((RingBuffer<Allocator, uint8_t>::getFreeElements()) >= remainingBytes,
                           "Can't append data even though the initial append was "
                           "successful");
            RingBuffer<Allocator, uint8_t>::appendPaddingElements(remainingBytes);
        }
        else
        {
            // we have to fill it with zeros
            constexpr uint8_t dummyData[64] = {};  // upper limit on stack usage
            const auto dummyDataSlice = outpost::asSlice(dummyData);
            while (remainingBytes > 0)
            {
                const auto bytesToAppend = remainingBytes > dummyDataSlice.getNumberOfElements()
                                                   ? dummyDataSlice.getNumberOfElements()
                                                   : remainingBytes;
                OUTPOST_ASSERT((RingBuffer<Allocator, uint8_t>::getFreeElements()) >= bytesToAppend,
                               "Can't append data even though the append was successful");
                RingBuffer<Allocator, uint8_t>::append(dummyDataSlice.first(bytesToAppend));
                remainingBytes -= bytesToAppend;
            }
        }
    }
    OUTPOST_ASSERT((RingBuffer<Allocator, uint8_t>::getFreeElements()) % chunkTotalSize == 0,
                   "Bug: Append did not stay in chunkSize");
    return true;
}

template <class Allocator, size_t chunkPayloadSize>
constexpr bool
ChunkedRingBuffer<Allocator, chunkPayloadSize>::isEmpty() const
{
    return RingBuffer<Allocator, uint8_t>::isEmpty();
}

template <class Allocator, size_t chunkPayloadSize>
bool
ChunkedRingBuffer<Allocator, chunkPayloadSize>::setFlagsToHead(const uint8_t& flags)
{
    const auto maybeElement = RingBuffer<Allocator, uint8_t>::peek(ChunkHeader::getStorageSize());
    if (maybeElement.getNumberOfElements() != ChunkHeader::getStorageSize())
    {
        return false;
    }
    auto header = deserializeHeader(maybeElement);
    header.flags = flags;
    // TODO: Check lifetime of temporary after inputting it into slice
    serializeHeaderInto(maybeElement, header);
    return true;
}

template <class Allocator, size_t chunkPayloadSize>
bool
ChunkedRingBuffer<Allocator, chunkPayloadSize>::pop()
{
    OUTPOST_ASSERT(this->getAvailableContinuousElements() >= chunkTotalSize
                           || this->getAvailableContinuousElements() == 0,
                   "Assertion either all or nothing did not hold");
    return RingBuffer<Allocator, uint8_t>::pop(chunkTotalSize).getNumberOfElements()
           == chunkTotalSize;
}

template <class Allocator, size_t chunkPayloadSize>
outpost::Slice</* const */ uint8_t>
ChunkedRingBuffer<Allocator, chunkPayloadSize>::peek(const size_t& index) /* const */
{
    const auto r = RingBuffer<Allocator, uint8_t>::peek(chunkTotalSize, index * chunkTotalSize);
    if (r.getNumberOfElements() > 0)
    {
        OUTPOST_ASSERT(r.getNumberOfElements() == chunkTotalSize,
                       "Assertion of \"either all or nothing\" did not hold");
        const auto header = deserializeHeader(r.first(ChunkHeader::getStorageSize()));
        return r.subSlice(ChunkHeader::getStorageSize(), header.length);
    }
    return outpost::Slice</* const */ uint8_t>::empty();
}

template <class Allocator, size_t chunkPayloadSize>
uint8_t
ChunkedRingBuffer<Allocator, chunkPayloadSize>::peekFlags(const size_t& index) /* const */
{
    const auto r = RingBuffer<Allocator, uint8_t>::peek(ChunkHeader::getStorageSize(),
                                                        index * chunkTotalSize);
    if (r.getNumberOfElements() > 0)
    {
        OUTPOST_ASSERT(r.getNumberOfElements() == ChunkHeader::getStorageSize(),
                       "Assertion of \"either all or nothing\" did not hold");
        return deserializeHeader(r).flags;
    }
    return 0;
}

template <class Allocator, size_t chunkPayloadSize>
constexpr void
ChunkedRingBuffer<Allocator, chunkPayloadSize>::reset()
{
    RingBuffer<Allocator, uint8_t>::reset();
}

template <class Allocator, size_t chunkPayloadSize>
void
ChunkedRingBuffer<Allocator, chunkPayloadSize>::resetElementsFrom(const size_t& count)
{
    RingBuffer<Allocator, uint8_t>::resetAfter(count * chunkTotalSize);
}

template <class Allocator, size_t chunkPayloadSize>
void
ChunkedRingBuffer<Allocator, chunkPayloadSize>::serializeHeaderInto(outpost::Slice<uint8_t> buf,
                                                                    const ChunkHeader& element)
{
    OUTPOST_ASSERT(buf.getNumberOfElements() >= ChunkHeader::getStorageSize(),
                   "given buffer is too small for serialization");
    Serialize s(buf);
    s.store(element.length);
    s.store(element.flags);
    OUTPOST_ASSERT(s.getPosition() == ChunkHeader::getStorageSize(),
                   "Serialize did not use the expected number of bytes");
}

template <class Allocator, size_t chunkPayloadSize>
constexpr const std::array<uint8_t, ChunkedRingBufferBase::ChunkHeader::getStorageSize()>
ChunkedRingBuffer<Allocator, chunkPayloadSize>::serializeHeader(const ChunkHeader& element)
{
    std::array<uint8_t, ChunkHeader::getStorageSize()> buf{};
    serializeHeaderInto(outpost::asSlice(buf), element);
    return buf;
}

template <class Allocator, size_t chunkPayloadSize>
ChunkedRingBufferBase::ChunkHeader
ChunkedRingBuffer<Allocator, chunkPayloadSize>::deserializeHeader(
        const outpost::Slice<const uint8_t>& buf)
{
    OUTPOST_ASSERT(buf.getNumberOfElements() >= ChunkHeader::getStorageSize(),
                   "given buffer is too small for deserialization");
    ChunkHeader ret;
    Deserialize d(buf);
    ret.length = d.read<decltype(ChunkHeader::length)>();
    ret.flags = d.read<decltype(ChunkHeader::flags)>();
    return ret;
}

}  // namespace container
}  // namespace outpost

#endif  // OUTPOST_UTILS_RING_BUFFER_INTERNAL_RING_BUFFER_CHUNKED_IMPL_H
