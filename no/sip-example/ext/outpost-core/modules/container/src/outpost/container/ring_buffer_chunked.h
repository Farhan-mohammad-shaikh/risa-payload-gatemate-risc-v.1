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
 * \file
 * \author Fabian Greif, Pascal Pieper
 *
 * \brief FIFO ring buffer data structure.
 */

#ifndef OUTPOST_UTILS_RING_BUFFER_CHUNKED_H
#define OUTPOST_UTILS_RING_BUFFER_CHUNKED_H

#include <outpost/base/slice.h>
#include <outpost/container/ring_buffer.h>
#include <outpost/storage/serialize.h>

#include <stddef.h>
#include <stdint.h>

#include <type_traits>

namespace outpost
{
namespace container
{

/**
 * Chunked Ring-Buffer with constant-sized slots.
 * Uses the concept of "Allocator"s that provide some kind of storage.
 * This is used for normal, array based, RingBuffers; but also for
 * persistent RingBuffers on, e.g. file systems or other memories.
 *
 * \param chunkPayloadSize
 *      Size of a single element.
 * \param totalNumberOfElements
 *      Number of elements to store
 *
 * \ingroup
 *      utils
 * \author
 *      Pascal Pieper
 *      Fabian Greif
 */

/**
 * Common, untemplated types for the \c ChunkedRingBuffer .
 */
class ChunkedRingBufferBase
{
protected:
    struct ChunkHeader
    {
        size_t length = 0;
        uint8_t flags = 0;

        static constexpr size_t
        getStorageSize()
        {
            return Serialize::getTypeSize<decltype(length)>()
                   + Serialize::getTypeSize<decltype(flags)>();
        }
    };
};

template <class Allocator, size_t chunkPayloadSize>
class ChunkedRingBuffer : ChunkedRingBufferBase, RingBuffer<Allocator, uint8_t>
{
public:
    static constexpr size_t chunkTotalSize = ChunkHeader::getStorageSize() + chunkPayloadSize;

    /**
     * \param[in] args
     *      will be forwarded to \c Allocator .
     */
    template <typename... Args>
    explicit inline ChunkedRingBuffer(Args&&... args);

    ChunkedRingBuffer(const ChunkedRingBuffer& o) = delete;

    ChunkedRingBuffer&
    operator=(const ChunkedRingBuffer& o) = delete;

    /**
     * Get the size of the usable bytes in a single chunk (or Slot)
     *
     * \return
     *      Size of a single buffer in bytes
     */
    inline static constexpr size_t
    getChunkSize();

    /**
     * Get the number of currently inactive (free) slots
     *
     * \return
     *      number of free chunks
     */
    inline constexpr size_t
    getFreeSlots() const;

    /**
     * Get the number of currently used Slots.
     *
     * \return number of taken count in the buffer
     */
    inline constexpr size_t
    getUsedSlots() const;

    /**
     * Write element to the buffer.
     *
     * \param[in] e new element of type T
     *
     * \return
     *  \c false on overflow (element not added; buffer full)
     *  \c true  on success  (element was added)
     */
    inline bool
    append(outpost::Slice<const uint8_t> data, uint8_t flags = 0, bool zeroOut = false);

    /**
     * Check if the buffer is empty.
     *
     * \return
        \c true  if the buffer is empty.
     *  \c false if the buffer contains at least one element.
     */
    inline constexpr bool
    isEmpty() const;

    /**
     * Updates \c flags of head element
     */
    inline bool
    setFlagsToHead(const uint8_t& flags);

    /**
     * Pops an element without actually returning it.
     *
     * \retval false on underflow (no element read; buffer empty)
     * \retval true  on success
     */
    inline bool
    pop();

    /**
     * Provides the means to access a specific element.
     *
     * - Head: index = 0
     * - Tail: index = getUsedcount()
     *
     * \param[in] index
     *      offset of the element to return
     */
    inline outpost::Slice</* const */ uint8_t>
    peek(const size_t& index = 0) /* const */;

    /**
     * Just like peek, but for Flag.
     */
    inline uint8_t
    peekFlags(const size_t& index = 0) /* const */;

    /**
     * Quick way of resetting the FIFO.
     */
    inline constexpr void
    reset();

    /**
     * Reset all elements from the given number onwards.
     *
     * E.g. resetElementsFrom(0) would reset the complete buffer,
     * resetElementsFrom(getUsedSlots()) does nothing.
     *
     * \param count
     *      Number of elements to skip before reseting elements.
     */
    inline void
    resetElementsFrom(const size_t& count);

private:
    static void
    serializeHeaderInto(outpost::Slice<uint8_t> buf, const ChunkHeader& element);

    static constexpr const std::array<uint8_t, ChunkHeader::getStorageSize()>
    serializeHeader(const ChunkHeader& element);

    static ChunkHeader
    deserializeHeader(const outpost::Slice<const uint8_t>& buf);
};

template <class Allocator, size_t chunkPayloadSize>
constexpr size_t ChunkedRingBuffer<Allocator, chunkPayloadSize>::chunkTotalSize;

}  // namespace container
}  // namespace outpost

#include "ring_buffer_internal/ring_buffer_chunked_impl.h"

#endif  // OUTPOST_UTILS_RING_BUFFER_CHUNKED_H
