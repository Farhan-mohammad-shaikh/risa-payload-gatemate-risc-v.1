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
 * \file ring_buffer.h
 * \author Pascal Pieper
 *
 * \brief FIFO element-oriented ring buffer data structure for storage.
 * Uses the concept of "Allocator"s that provide some kind of storage.
 * This is used for normal, array based, RingBuffers; but also for
 * persistent RingBuffers on, e.g. file systems or other memories.
 * Also used as a basis for the \c ChunkedRingBuffer that manages 'slots'.
 */

#ifndef OUTPOST_UTILS_RING_BUFFER_H
#define OUTPOST_UTILS_RING_BUFFER_H

#include "ring_buffer_internal/ring_buffer_detail.h"

#include <outpost/base/slice.h>
#include <outpost/utils/expected.h>

namespace outpost
{
namespace container
{

// ---------------------------------------------------------------------------
/**
 * Common, untemplated types for the \c RingBuffer .
 */
class RingBufferBase
{
public:
    enum class OperationResult
    {
        success = 0,
        notEnoughSpace,
        readWriteTooBig,
        readWriteOverBoundary,
    };
};

// ---------------------------------------------------------------------------
/**
 * \class RingBuffer
 *
 * Generic ring buffer implementation, so access functions reference
 * element offsets.
 *
 * For \c peek() and \c pop operations, both copy and no-copy variants exist.
 * E.g. \c peekInto() needs a target buffer to copy into, \c peek() directly
 * returns a reference to some contiguous addressable memory. The downside of
 * the no-copy variants is, however, that overflows in the underlying Allocator
 * storage are not possible to access in one operation. E.g. a read might
 * request x elements, but the first x/2 elements are at the end of the array, and
 * the remaining x/2 elements are at the start of the array (resulting in an
 * \c OperationResult::readWriteOverBoundary )
 *
 * It uses generic \c Allocator as the storage, which might in turn use
 * anything as backend. This includes simple arrays in RAM and extends to
 * powerloss-resilient serial-accessed memory with optional buffering.
 * The implementation of these Allocators are not virtual but through *static*
 * inheritance, so no overhead is implied.
 *
 * \tparam A
 *      (possibly) the Allocator. Only actually an Allocator after passing
 *      SFINAE-Check in \c ring_buffer::detail::ImplementsAllocator .
 * \tparam T
 *      The type of the elements. For byte level Allocators, this should be uint8_t.
 */
template <class A, class T>
class RingBuffer : public ring_buffer::MetadataWrapper<A>, public RingBufferBase
{
protected:
    using Allocator = typename ring_buffer::AllocatorStorage<A>::Allocator;
    using ring_buffer::AllocatorStorage<A>::getAllocator;
    using ring_buffer::MetadataWrapper<A>::setReadIndexAndElementsUsedAtomically;
    using ring_buffer::MetadataWrapper<A>::getNumberOfElementsUsed;
    using ring_buffer::MetadataWrapper<A>::setNumberOfElementsUsed;
    using ring_buffer::MetadataWrapper<A>::getReadIndex;
    using ring_buffer::MetadataWrapper<A>::setReadIndex;

public:
    /**
     * \param[in] args
     *      will be forwarded to \c Allocator .
     */
    template <typename... Args>
    constexpr explicit RingBuffer(Args&&... args);

    RingBuffer(const RingBuffer& o) = delete;

    RingBuffer&
    operator=(const RingBuffer& o) = delete;

    /**
     * Write element to the buffer.
     *
     * \param[in] from Elements to append
     *
     * \return
     *      \c OperationResultnotEnoughSpace on overflow (element not added;
     *          free space less than requested elements)
     *      \c OperationResult::success on success (element added)
     */
    inline OperationResult
    append(const outpost::Slice<const T>& from);

    /**
     * Pops an element from the ringbuffer by copying to a given buffer.
     *
     * \param to
     *      storage to pop the element into.
     *
     * \retval
     *      \c OperationResut::success on success
     *      \c OperationResut::empty (no element read; buffer empty)
     *      \c OperationResut::readWriteTooBig (no element read; requested
     *          read size is bigger than available elements)
     */
    inline OperationResult
    popInto(const outpost::Slice<T>& to);

    /**
     * Provides the means to access a specific range.
     *
     * - Head: offset = 0
     * - Tail: offset = capacity() - to.getNumberOfElements()
     *
     * \param[out] to
     *      buffer to write element into
     * \param[in] offset
     *      in elements from reading index
     *
     * \return
     *      \c OperationResut::empty (no element read; buffer empty)
     *      \c OperationResut::readWriteTooBig (no element read; requested read
     *          size is bigger than available elements)
     *      \c OperationResut::success on success
     */
    OperationResult
    peekInto(const outpost::Slice<T>& to, const size_t& offset = 0) const;

    /**
     * Provides direct (no-copy) access a specific range if the allocator
     * supports it.
     *
     * \warning
     *      read is only complete if it does not overflow a boundary.
     *
     * If you want to read the last N elements use \c capactiy() - N as the
     * \c offset , and set \c requestedElementsToRead to N.
     *
     * If you want to read the first N elements keep the \c offset at \c 0 and
     * set \c requestedElementsToRead to N.
     *
     * \param[in] requestedElementsToRead
     *      number of Elements to read
     *
     * \param[in] offset
     *      in elements from reading index
     *
     * \return
     *      Slice of valid elements, or \c OperationResult on error
     */
    template <class U = A,
              class = typename std::enable_if<
                      ring_buffer::detail::ImplementsDirectAccess<U>::directAccess>::type>
    outpost::Slice<T>
    peek(const size_t& requestedElementsToRead, const size_t& offset = 0);

    /**
     * Pops an element from the ring buffer and returns a slice to the elements
     * that could be popped (best effort!).
     * Only defined if the allocator supports direct access.
     *
     * \warning
     *      It is not possible to pop elements over the allocator's boundary.
     *
     * \return \c outpost::Slice with up to \c requestedElementsToRead elements.
     */
    template <class U = A,
              class = typename std::enable_if<
                      ring_buffer::detail::ImplementsDirectAccess<U>::directAccess>::type>
    inline outpost::Slice<T>
    pop(const size_t& requestedElementsToRead);

    /**
     * Quick way of resetting the FIFO.
     */
    constexpr void
    reset();

    /**
     * Reset all elements from the given offset onwards. Used for discarding
     * the newest elements at the *end* of the buffer.
     *
     * E.g. \c resetAfter(0) would reset the complete buffer,
     * \c resetAfter(getAvailableElements()) does nothing.
     *
     * \param[in] count
     *      Number of elements to skip before resetting elements.
     *
     * \return
     *      \c true if the change could be applied
     */
    constexpr bool
    resetAfter(const size_t& count);

    /**
     * Get the static capacity of the ring buffer
     *
     * \return the maximum number of usable elements
     */
    constexpr size_t
    capacity() const;

    /**
     * Get the number of populated elements
     *
     * \return the number of used elements
     */
    constexpr size_t
    getAvailableElements() const;

    /**
     * Get the number of populated *consecutive* elements.
     *
     * This is used for the maximal number of elements that
     * can be `peek`ed instead of `peekInto`ed.
     */
    constexpr size_t
    getAvailableContinuousElements(size_t offset = 0) const;

    /**
     * Get the number of remaing free elements
     *
     * \return the number of free elements
     */
    constexpr size_t
    getFreeElements() const;

    /**
     * Get the number of free *consecutive* elements.
     *
     * This is analogous to `getAvailableContinuousElements()`.
     */
    constexpr size_t
    getFreeContinuousElements() const;

    /**
     * Check if the buffer is empty
     *
     * \return \c true if the buffer is empty
     */
    constexpr bool
    isEmpty() const;

    /**
     * Check if the buffer is full
     *
     * \return \c true if the buffer is full
     */
    constexpr bool
    isFull() const;

    /**
     * Skip `count` elements to write.
     * Essentially an append without writing elements.
     *
     * \param[in] count
     *      the number of elements to skip
     *
     * \return \c OperationResult::success if successful.
     */
    constexpr OperationResult
    appendPaddingElements(const size_t& count);

    /**
     * Discard `count` oldest elements that would have been read.
     *
     * Works exactly like `pop()` with discarding the return.
     * In contrast to `pop()`, this function is always defined.
     */
    constexpr OperationResult
    discardElements(const size_t& count);

private:
    constexpr size_t
    increment(const size_t& index, const size_t& count) const;

    constexpr void
    decrementBy(const size_t& elements);
};

}  // namespace container
}  // namespace outpost

#include "ring_buffer_internal/ring_buffer_impl.h"

#endif  // OUTPOST_UTILS_RING_BUFFER_H
