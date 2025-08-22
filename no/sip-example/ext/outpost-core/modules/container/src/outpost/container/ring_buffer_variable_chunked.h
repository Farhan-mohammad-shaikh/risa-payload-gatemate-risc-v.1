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

#ifndef OUTPOST_CONTAINER_RING_BUFFER_VARIABLE_CHUNKED_H
#define OUTPOST_CONTAINER_RING_BUFFER_VARIABLE_CHUNKED_H

#include <outpost/base/slice.h>
#include <outpost/base/testing_assert.h>
#include <outpost/container/ring_buffer.h>
#include <outpost/container/ring_buffer_allocators/array_ring_buffer.h>
#include <outpost/storage/serialize.h>
#include <outpost/utils/expected.h>
#include <outpost/utils/limits.h>

#include <stdint.h>
#include <string.h>

namespace outpost
{
namespace container
{

// ---------------------------------------------------------------------------
/**
 * \class VariableChunkedRingBufferRaw
 */
template <typename Allocator>
class VariableChunkedRingBufferRaw
{
public:
    enum class OperationalResult
    {
        success,
        bufferTooSmall
    };

    template <typename... Args>
    constexpr explicit VariableChunkedRingBufferRaw(Args&&... args);

    VariableChunkedRingBufferRaw(const VariableChunkedRingBufferRaw&) = delete;

    VariableChunkedRingBufferRaw&
    operator=(const VariableChunkedRingBufferRaw&) = delete;

    ~VariableChunkedRingBufferRaw() = default;

    /**
     * Checks if Buffer is empty.
     *
     * \retval true If Buffer is empty
     * \retval false If Buffer is not empty
     */
    inline constexpr bool
    isEmpty() const;

    /**
     * Returns the size of Buffer in bytes.
     *
     * \return
     *      the size of the buffer
     */
    inline constexpr size_t
    capacity() const;

    /**
     * Returns the number of chunks stored in the ring buffer.
     *
     * \return
     *      the number of chunks in the ring buffer.
     */
    constexpr inline size_t
    getNumberOfChunks() const;

    /**
     * Quick way of resetting the ring buffer.
     */
    inline constexpr void
    reset();

protected:
    RingBuffer<Allocator, uint8_t> mRingBuffer;
    size_t mNumberOfChunks;
};

// ---------------------------------------------------------------------------
/**
 * \class VariableChunkedRingBuffer
 *
 * \tparam SizeType
 */
template <typename SizeType = uint16_t, typename Allocator = ReferencedArrayAllocator<uint8_t>>
class VariableChunkedRingBuffer : public VariableChunkedRingBufferRaw<Allocator>
{
public:
    using OperationalResult = typename VariableChunkedRingBufferRaw<Allocator>::OperationalResult;

    static_assert(std::is_integral<SizeType>::value && std::is_unsigned<SizeType>::value,
                  "SizeType must be an unsigned integral type!");

    static constexpr size_t headerSize = sizeof(SizeType);

    explicit VariableChunkedRingBuffer(outpost::Slice<uint8_t> storage);

    VariableChunkedRingBuffer(const VariableChunkedRingBuffer&) = delete;

    VariableChunkedRingBuffer&
    operator=(const VariableChunkedRingBuffer&) = delete;

    ~VariableChunkedRingBuffer() = default;

    /**
     * Push a chunk to the ring buffer
     *
     * If the data with the header exceeds the capacity of the buffer
     * or the maximum value of \c SizeType the input chunk ( \c data) will not
     * be added to the ring buffer. This will be visible throught the return
     * value. If not enough free bytes are available, the oldest chunks will
     * be removed to insert the new data.
     *
     * \param data
     *      the data to push.
     *
     * \return the number of user bytes written (excluding the header).
     */
    size_t
    pushChunk(const outpost::Slice<const uint8_t>& data);

    /**
     * Peek a chunk from the ring buffer
     *
     * The data will not be removed. If the chunk is larger than the given
     * output buffer, no data will be read.
     *
     * \param data
     *      the output buffer
     *
     * \return A slice to the output buffer with the copied data. If \c data
     *      is not large enough to hold the complete chunk an \c bufferTooSmall
     *      will be returned.
     */
    outpost::Expected<outpost::Slice<uint8_t>, OperationalResult>
    peekChunkInto(const outpost::Slice<uint8_t>& data) const;

    /**
     * Pop a chunk from the ring buffer
     *
     * The data will be removed. If the chunk is larger than the given
     * output buffer, no data will be read.
     *
     * \param data
     *      the output buffer
     *
     * \return A slice to the output buffer with the copied data. If \c data
     *      is not large enough to hold the complete chunk an \c bufferTooSmall
     *      will be returned.
     */
    outpost::Expected<outpost::Slice<uint8_t>, OperationalResult>
    popChunkInto(const outpost::Slice<uint8_t>& data);

    /**
     * Drop the next chunk which would be read by \c popChunkInto or \c peekChunkInto
     */
    inline constexpr void
    discardChunk();

    /**
     * Returns the number of free bytes which can be used by the user. I.e.:
     * The user can push a chunk of the size \c getFreeUserBytes() with
     * guarantee success.
     *
     * \return
     *      the number of free bytes
     */
    inline constexpr size_t
    getFreeUserBytes() const;

    /**
     * Returns the number of bytes used by the user. I.e.: the headers are
     * excluded.
     *
     * \return
     *      the number of bytes used by the user.
     */
    constexpr inline size_t
    getAvailableBytes() const;

private:
    std::optional<outpost::Slice<uint8_t>>
    peekInto(const outpost::Slice<uint8_t>& data) const;

    void
    storeSize(SizeType size);

    size_t
    readSize() const;
};

// ---------------------------------------------------------------------------
/**
 * Storage class for the buffer.
 *
 * \tparam N
 *      size of the buffer in bytes
 * \tparam SizeType
 *      Integral type used to store the chunk size
 */
template <size_t N, typename SizeType>
class VariableChunkedRingBufferStorage : private ArrayStorage<N, uint8_t>,
                                         public VariableChunkedRingBuffer<SizeType>
{
public:
    inline VariableChunkedRingBufferStorage() :
        ArrayStorage<N, uint8_t>(),
        VariableChunkedRingBuffer<SizeType>(ArrayStorage<N, uint8_t>::getBuffer())
    {
    }
};

}  // namespace container
}  // namespace outpost

#include "outpost/container/ring_buffer_internal/ring_buffer_variable_chunked_impl.h"

#endif  // OUTPOST_CONTAINER_RING_BUFFER_VARIABLE_CHUNKED_H
