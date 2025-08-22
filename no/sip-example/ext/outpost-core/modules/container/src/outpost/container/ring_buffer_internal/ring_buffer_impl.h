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
 * \file ring_buffer_impl.h
 * \author Pascal Pieper
 *
 * \brief Definitions of functions declared by \file ring_buffer.h
 */

#ifndef MODULES_UTILS_SRC_OUTPOST_CONTAINER_RING_BUFFER_INTERNAL_RING_BUFFER_IMPL_H
#define MODULES_UTILS_SRC_OUTPOST_CONTAINER_RING_BUFFER_INTERNAL_RING_BUFFER_IMPL_H

// only for indexer, this won't be used directly
#include "../ring_buffer.h"

#include <outpost/base/testing_assert.h>

// #include <iostream> // for debug only

namespace outpost
{
namespace container
{

template <class A, class T>
template <typename... Args>
constexpr RingBuffer<A, T>::RingBuffer(Args&&... args) :
    ring_buffer::MetadataWrapper<A>(std::forward<Args>(args)...)
{
}

template <class Allocator, class T>
inline RingBufferBase::OperationResult
RingBuffer<Allocator, T>::append(const outpost::Slice<const T>& from)
{
    const auto numberOfElementsUsed = getNumberOfElementsUsed();
    const auto readIndex = getReadIndex();
    const auto requestedElementsToWrite = from.getNumberOfElements();

    // LT-Comparison because of \c auto
    // cppcheck-suppress unsignedLessThanZero
    if (requestedElementsToWrite <= 0)
    {
        // well, we don't have to write anything
        return OperationResult::success;
    }

    if (numberOfElementsUsed + requestedElementsToWrite > getAllocator().capacity())
    {
        // std::cout << "append: notEnoughSpace for " << requestedElementsToWrite
        //           << " additional elements to " << numberOfElementsUsed << " of " <<
        //           getAllocator().capacity()
        //           << std::endl;
        return OperationResult::notEnoughSpace;
    }
    // calculate write index
    const auto writeStartIndex = increment(readIndex, numberOfElementsUsed);
    const auto writableContinuousElements = getFreeContinuousElements();

    //     std::cout << "append: write from element " << writeStartIndex << " to " <<
    //     writeStartIndex + requestedElementsToWrite
    //               << std::endl;
    if (requestedElementsToWrite <= writableContinuousElements)
    {
        // no wrap-around, we can copy fast
        getAllocator().write(writeStartIndex, from);
    }
    else
    {
        // would wrap in-between, so copy in two chunks
        //        std::cout << "append " << requestedElementsToWrite << ": writing over boundary" <<
        //        std::endl;
        const auto firstPartLength = writableContinuousElements;
        const auto secondPartLength = requestedElementsToWrite - firstPartLength;
        //        std::cout << writeStartIndex << " - " << writeStartIndex + firstPartLength << " ,
        //        "; std::cout << 0 << " - " << 0 + secondPartLength << std::endl;
        getAllocator().write(writeStartIndex, from.first(firstPartLength));
        getAllocator().write(0, from.subSlice(firstPartLength, secondPartLength));
    }

    setNumberOfElementsUsed(numberOfElementsUsed + requestedElementsToWrite);

    return OperationResult::success;
}

template <typename Allocator, typename T>
RingBufferBase::OperationResult
RingBuffer<Allocator, T>::popInto(const outpost::Slice<T>& to)
{
    const auto result = peekInto(to);
    if (result == OperationResult::success)
    {
        decrementBy(to.getNumberOfElements());
    }
    return result;
}

template <typename Allocator, typename T>
RingBufferBase::OperationResult
RingBuffer<Allocator, T>::peekInto(const outpost::Slice<T>& to, const size_t& offset) const
{
    const auto numberOfElementsUsed = getNumberOfElementsUsed();
    const auto requestedElementsToRead = to.getNumberOfElements();

    // *less* or equal b/c it is `auto`
    // cppcheck-suppress unsignedLessThanZero
    if (requestedElementsToRead <= 0)
    {
        return OperationResult::success;
    }

    if (offset + requestedElementsToRead > numberOfElementsUsed)
    {
        // std::cout << "peekInto: read of " << requestedElementsToRead << " elements at offset "
        //           << offset << " would overflow high water mark"
        //           << "(" << numberOfElementsUsed << ")" << std::endl;
        return OperationResult::readWriteTooBig;
    }

    const auto readStartIndex = increment(getReadIndex(), offset);
    const auto readableContinuousElements = getAvailableContinuousElements(offset);

    // std::cout << "peekInto: read from element " << readStartIndex
    //           << ", requested " << requestedElementsToRead << " elements" << std::endl;

    if (requestedElementsToRead <= readableContinuousElements)
    {
        // no wrap-around, we can copy fast
        getAllocator().read(readStartIndex, to);
    }
    else
    {
        // would wrap in-between, so split into two copies
        const auto firstPartLength = readableContinuousElements;
        const auto secondPartLength = requestedElementsToRead - firstPartLength;

        // std::cout << "peekInto: reading over boundary, so reading ";
        // std::cout << readStartIndex << "-" << readStartIndex + firstPartLength;
        // std::cout << " and ";
        // std::cout << 0 << "-" << 0 + secondPartLength;
        // std::cout << std::endl;
        // std::cout << "   (with a total capacity of " << getAllocator().capacity() << ")" <<
        // std::endl;

        getAllocator().read(readStartIndex, to.first(firstPartLength));
        getAllocator().read(0, to.subSlice(firstPartLength, secondPartLength));
    }

    return OperationResult::success;
}

template <typename Allocator, typename T>
template <class U, class>  // implements
                           // ring_buffer::detail::ImplementsDirectAccess<U>::directAccess
outpost::Slice<T>
RingBuffer<Allocator, T>::peek(const size_t& requestedElementsToRead, const size_t& offset)
{
    const auto numberOfElementsUsed = getNumberOfElementsUsed();

    if (requestedElementsToRead == 0 || offset >= numberOfElementsUsed)
    {
        // requested zero elements or obviously outside of valid elements
        // Also guards arithmetic against overflow later
        return outpost::Slice<T>::empty();
    }

    // limit to available number of elements
    const auto maxReadableElements =
            std::min(requestedElementsToRead, numberOfElementsUsed - offset);
    const auto readStartIndex = increment(getReadIndex(), offset);
    // further limit to available continuous region
    const auto readEndIndex = std::min(readStartIndex + maxReadableElements, this->capacity());
    const auto actualElementsToRead = readEndIndex - readStartIndex;

    // std::cout << "request read from " << offset << " - " << offset + requestedElementsToRead
    //           << std::endl;
    // std::cout << "     act. reading " << readStartIndex << " - "
    //           << readStartIndex + actualElementsToRead << std::endl;

    return getAllocator().getBufferAt(readStartIndex, actualElementsToRead);
}

template <typename Allocator, typename T>
template <class U, class>  // implements
                           // ring_buffer::detail::ImplementsDirectAccess<U>::directAccess
outpost::Slice<T>
RingBuffer<Allocator, T>::pop(const size_t& requestedElementsToRead)
{
    auto result = peek(requestedElementsToRead);
    decrementBy(result.getNumberOfElements());
    return result;
}

template <typename Allocator, typename T>
constexpr void
RingBuffer<Allocator, T>::reset()
{
    // Might also be "atomically",
    // but this order is OK as well
    setNumberOfElementsUsed(0);
    // set read index to zero is not really necessary,
    // but nice for a more complete reset.
    setReadIndex(0);
}

template <typename Allocator, typename T>
constexpr bool
RingBuffer<Allocator, T>::resetAfter(const size_t& count)
{
    if (getNumberOfElementsUsed() <= count)
    {
        return false;
    }
    setNumberOfElementsUsed(count);
    return true;
}

template <typename Allocator, typename T>
constexpr RingBufferBase::OperationResult
RingBuffer<Allocator, T>::appendPaddingElements(const size_t& count)
{
    const auto numberOfElementsUsed = getNumberOfElementsUsed();

    if (count == 0)
    {
        return OperationResult::success;
    }

    if (numberOfElementsUsed + count > getAllocator().capacity())
    {
        return OperationResult::notEnoughSpace;
    }

    setNumberOfElementsUsed(numberOfElementsUsed + count);
    return OperationResult::success;
}

template <typename Allocator, typename T>
constexpr RingBufferBase::OperationResult
RingBuffer<Allocator, T>::discardElements(const size_t& count)
{
    if (count > getAvailableElements())
    {
        return OperationResult::readWriteTooBig;
    }
    decrementBy(count);
    return OperationResult::success;
}

template <typename Allocator, typename T>
constexpr size_t
RingBuffer<Allocator, T>::capacity() const
{
    return getAllocator().capacity();
}

template <typename Allocator, typename T>
constexpr size_t
RingBuffer<Allocator, T>::getAvailableElements() const
{
    return getNumberOfElementsUsed();
}

template <typename Allocator, typename T>
constexpr size_t
RingBuffer<Allocator, T>::getAvailableContinuousElements(size_t offset) const
{
    if (offset > getAvailableElements())
    {
        // overflow protection for subtraction later
        return 0;
    }
    const auto readIndex = increment(getReadIndex(), offset);
    const auto endOfContinuousRegion = this->capacity();

    const auto continuousElements =
            std::min(endOfContinuousRegion - readIndex, getAvailableElements() - offset);
    OUTPOST_ASSERT(readIndex + continuousElements <= this->capacity(),
                   "Continous elements over boundary");
    return continuousElements;
}

template <typename Allocator, typename T>
constexpr size_t
RingBuffer<Allocator, T>::getFreeElements() const
{
    return this->capacity() - getNumberOfElementsUsed();
}

template <typename Allocator, typename T>
constexpr size_t
RingBuffer<Allocator, T>::getFreeContinuousElements() const
{
    const auto writeIndex = increment(getReadIndex(), getNumberOfElementsUsed());
    const auto endOfContinuousRegion = this->capacity();
    // std::min will be constexpr with >= c++14
    return std::min(endOfContinuousRegion - writeIndex, getFreeElements());
}

template <typename Allocator, typename T>
constexpr bool
RingBuffer<Allocator, T>::isEmpty() const
{
    return getNumberOfElementsUsed() == 0;
}

template <typename Allocator, typename T>
constexpr bool
RingBuffer<Allocator, T>::isFull() const
{
    return getNumberOfElementsUsed() == capacity();
}

template <typename Allocator, typename T>
constexpr size_t
RingBuffer<Allocator, T>::increment(const size_t& index, const size_t& count) const
{
    return (index + count) % capacity();
}

template <typename Allocator, typename T>
constexpr void
RingBuffer<Allocator, T>::decrementBy(const size_t& elements)
{
    const auto readIndex = getReadIndex();
    const auto newReadIndex = increment(readIndex, elements);
    const auto newElementsUsed = getNumberOfElementsUsed() - elements;
    setReadIndexAndElementsUsedAtomically(newReadIndex, newElementsUsed);
}

}  // namespace container
}  // namespace outpost

#endif /* MODULES_UTILS_SRC_OUTPOST_CONTAINER_RING_BUFFER_INTERNAL_RING_BUFFER_IMPL_H */
