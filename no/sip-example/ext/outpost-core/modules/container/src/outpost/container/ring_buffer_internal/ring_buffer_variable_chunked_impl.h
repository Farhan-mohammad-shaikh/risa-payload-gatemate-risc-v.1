/*
 * Copyright (c) 2024, Janosch Reinking
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

#ifndef OUTPOST_CONTAINER_RING_BUFFER_VARIABLE_CHUNKED_IMPL_H
#define OUTPOST_CONTAINER_RING_BUFFER_VARIABLE_CHUNKED_IMPL_H

#include <outpost/container/ring_buffer_variable_chunked.h>

namespace outpost
{
namespace container
{

// ---------------------------------------------------------------------------
template <typename Allocator>
template <typename... Args>
constexpr VariableChunkedRingBufferRaw<Allocator>::VariableChunkedRingBufferRaw(Args&&... args) :
    mRingBuffer(std::forward<Args>(args)...), mNumberOfChunks(0U)
{
}

template <typename Allocator>
constexpr bool
VariableChunkedRingBufferRaw<Allocator>::isEmpty() const
{
    return mRingBuffer.isEmpty();
}

template <typename Allocator>
constexpr size_t
VariableChunkedRingBufferRaw<Allocator>::capacity() const
{
    return mRingBuffer.capacity();
}

template <typename Allocator>
constexpr size_t
VariableChunkedRingBufferRaw<Allocator>::getNumberOfChunks() const
{
    return mNumberOfChunks;
}

template <typename Allocator>
constexpr void
VariableChunkedRingBufferRaw<Allocator>::reset()
{
    mNumberOfChunks = 0U;
    mRingBuffer.reset();
}

// ---------------------------------------------------------------------------
template <typename SizeType, typename Allocator>
constexpr size_t VariableChunkedRingBuffer<SizeType, Allocator>::headerSize;

// ---------------------------------------------------------------------------
template <typename SizeType, typename Allocator>
VariableChunkedRingBuffer<SizeType, Allocator>::VariableChunkedRingBuffer(
        outpost::Slice<uint8_t> storage) :
    VariableChunkedRingBufferRaw<Allocator>(storage)
{
    OUTPOST_ASSERT(storage.getNumberOfElements() >= headerSize,
                   "Must be possible to write at least one header!");
}

// ---------------------------------------------------------------------------
template <typename SizeType, typename Allocator>
size_t
VariableChunkedRingBuffer<SizeType, Allocator>::pushChunk(const outpost::Slice<const uint8_t>& data)
{
    const size_t length = data.getNumberOfElements();

    if (length > outpost::Limits<SizeType>::max || length > this->capacity() - headerSize)
    {
        return 0U;
    }

    // pop message until enough space is available or there are no
    // messages left to pop
    while (length + headerSize > this->mRingBuffer.getFreeElements()
           && this->getNumberOfChunks() > 0U)
    {
        discardChunk();
    }

    const size_t numberOfBytesToWrite = length;

    storeSize(numberOfBytesToWrite);
    const auto result = this->mRingBuffer.append(data.first(numberOfBytesToWrite));

    (void) result;
    OUTPOST_ASSERT(result == decltype(result)::success,
                   "Can not fail since the available space was checked before");

    this->mNumberOfChunks++;
    return numberOfBytesToWrite;
}

template <typename SizeType, typename Allocator>
outpost::Expected<outpost::Slice<uint8_t>,
                  typename VariableChunkedRingBufferRaw<Allocator>::OperationalResult>
VariableChunkedRingBuffer<SizeType, Allocator>::peekChunkInto(
        const outpost::Slice<uint8_t>& data) const
{
    const auto maybeChunk = peekInto(data);
    if (maybeChunk.has_value())
    {
        return *maybeChunk;
    }
    else
    {
        return outpost::unexpected(OperationalResult::bufferTooSmall);
    }
}

template <typename SizeType, typename Allocator>
outpost::Expected<outpost::Slice<uint8_t>,
                  typename VariableChunkedRingBufferRaw<Allocator>::OperationalResult>
VariableChunkedRingBuffer<SizeType, Allocator>::popChunkInto(const outpost::Slice<uint8_t>& data)
{
    const auto maybeChunk = peekInto(data);
    if (maybeChunk)
    {
        OUTPOST_ASSERT(this->mNumberOfChunks > 0, "Chunk counter disagrees with content");
        const size_t chunkRawSize = headerSize + maybeChunk->getNumberOfElements();
        this->mRingBuffer.discardElements(chunkRawSize);
        this->mNumberOfChunks--;
        return *maybeChunk;
    }
    else
    {
        return outpost::unexpected(OperationalResult::bufferTooSmall);
    }
}

template <typename SizeType, typename Allocator>
constexpr void
VariableChunkedRingBuffer<SizeType, Allocator>::discardChunk()
{
    if (this->getNumberOfChunks() > 0U)
    {
        const size_t length = readSize();
        const auto chunkRawSize = sizeof(SizeType) + length;
        this->mRingBuffer.discardElements(chunkRawSize);
        this->mNumberOfChunks--;
    }
}

template <typename SizeType, typename Allocator>
constexpr size_t
VariableChunkedRingBuffer<SizeType, Allocator>::getFreeUserBytes() const
{
    const auto freeBytes = this->mRingBuffer.getFreeElements();
    if (freeBytes > headerSize)
    {
        return freeBytes - headerSize;
    }
    else
    {
        return 0;
    }
}

template <typename SizeType, typename Allocator>
constexpr size_t
VariableChunkedRingBuffer<SizeType, Allocator>::getAvailableBytes() const
{
    return this->mRingBuffer.getAvailableElements() - (headerSize * this->mNumberOfChunks);
}

// ---------------------------------------------------------------------------
template <typename SizeType, typename Allocator>
std::optional<outpost::Slice<uint8_t>>
VariableChunkedRingBuffer<SizeType, Allocator>::peekInto(const outpost::Slice<uint8_t>& data) const
{
    if (this->getNumberOfChunks() == 0)
    {
        // No chunk available
        return std::nullopt;
    }

    const size_t length = readSize();

    if (length <= data.getNumberOfElements())
    {
        const size_t numberOfBytesToRead = length;
        const auto result = this->mRingBuffer.peekInto(data.first(numberOfBytesToRead), headerSize);

        (void) result;
        OUTPOST_ASSERT(result == decltype(result)::success,
                       "Can not fail since enough bytes are available");

        return data.first(numberOfBytesToRead);
    }
    else
    {
        // The chunk would not fit
        return std::nullopt;
    }
}

template <typename SizeType, typename Allocator>
void
VariableChunkedRingBuffer<SizeType, Allocator>::storeSize(SizeType size)
{
    std::array<uint8_t, headerSize> headerBuffer;
    outpost::Serialize serialize(outpost::asSlice(headerBuffer));
    serialize.store<SizeType>(size);
    const auto result = this->mRingBuffer.append(outpost::asSlice(headerBuffer));

    (void) result;
    OUTPOST_ASSERT(result == decltype(result)::success,
                   "Can not fail since available space was checked before");
}

template <typename SizeType, typename Allocator>
size_t
VariableChunkedRingBuffer<SizeType, Allocator>::readSize() const
{
    std::array<uint8_t, headerSize> headerBuffer;
    const auto result = this->mRingBuffer.peekInto(outpost::asSlice(headerBuffer));

    (void) result;
    OUTPOST_ASSERT(result == decltype(result)::success,
                   "Can not fail since enough bytes are available");

    outpost::Deserialize deserialize(outpost::asSlice(headerBuffer));
    const auto size = deserialize.read<SizeType>();
    return size;
}

}  // namespace container
}  // namespace outpost

#endif  // OUTPOST_CONTAINER_RING_BUFFER_VARIABLE_CHUNKED_IMPL_H
