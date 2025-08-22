/*
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Pascal Pieper
 * Copyright (c) 2024, Cold, Erin Cynthia
 * Copyright (c) 2024, Janosch Reinking
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
 *      ring_buffer_detail.h
 *
 * \author
 *      Pascal Pieper
 *
 * \brief
 *      Detail implementation behind the generic RingBuffer.
 *      It contains the "contracts" between the RingBuffer and
 *      the Allocators. Also, the wrapper for different, optional,
 *      functions are implemented here.
 */

#ifndef OUTPOST_UTILS_RING_BUFFER_DETAIL_H
#define OUTPOST_UTILS_RING_BUFFER_DETAIL_H

#include <outpost/base/slice.h>
#include <outpost/traits/legacy/result_of.h>

#include <type_traits>

namespace outpost
{
namespace container
{
namespace ring_buffer
{
namespace detail
{

// available only from c++17 onwards, so implement our own version
template <typename... Ts>
struct make_void
{
    typedef void type;
};

template <typename... Ts>
using void_t = typename make_void<Ts...>::type;

// ---- "Contracts" for Allocators ----

template <typename T, typename = void>
struct allocated_type
{
    using value_type = uint8_t;
};

template <typename T>
struct allocated_type<T, void_t<typename T::value_type>>
{
    using value_type = typename T::value_type;
};

template <typename T>
using allocatedType = typename allocated_type<T>::value_type;

template <class T>
struct ImplementsAllocator
{
    // mandatory functions to implement
    static constexpr bool readConst =
            std::is_same<typename outpost::result_of<decltype (&T::read)(
                                 T,
                                 const size_t& /* offs */,
                                 const outpost::Slice<allocatedType<T>>& /* buf */)>::type,
                         void>::value;
    static constexpr bool readNonConst = std::is_same<
            typename outpost::result_of<decltype (&T::read)(
                    T, const size_t& /* offs */, outpost::Slice<allocatedType<T>>& /* buf */)>::type,
            void>::value;
    static constexpr bool write =
            std::is_same<typename outpost::result_of<decltype (&T::write)(
                                 T,
                                 const size_t& /* offs */,
                                 const outpost::Slice<const allocatedType<T>>& /* buf */)>::type,
                         void>::value;

    static constexpr bool capacity = true;
    // std::is_same<typename outpost::result_of<decltype (
    //                      &T::capacity)(T)>::type,
    //              size_t>::value;

    static constexpr bool mandatoryFunctions = (readConst || readNonConst) && write && capacity;
};

template <class T, class = void>
struct ImplementsDirectAccess
{
    /* No direct Access to underlying memory.
     * Not contiguous, move in/out will always need to copy.
     */
    static constexpr bool directAccess = false;
};

template <class T>
struct ImplementsDirectAccess<
        T,
        void_t<decltype (&T::getBufferAt)(T, const size_t& /* offset */, const size_t& /* len */)>>
{
    // Direct Access to underlying memory. Contiguous, without the need to copy.
    static constexpr bool directAccess =
            std::is_same<typename outpost::result_of<decltype (&T::getBufferAt)(
                                 T, const size_t& /* offset */, const size_t& /* len */)>::type,
                         const outpost::Slice<allocatedType<T>>>::value;
    /*
     * not outpost::Slice<**const** allocatedType> because some ringbuffer users
     * rely on a peek() or pop() 'ed Slice to write into.
     */
};

template <class T, class = void>
struct ImplementsMetadataAccess
{
    /**
     * Allocator does not offer metadata access functions.
     * It will probably not offer persistency between instantiations.
     * Simplest implementation.
     */
    static constexpr bool value = false;
};

template <class T>
struct ImplementsMetadataAccess<
        T,
        // this tests for correct function parameters
        void_t<decltype(std::declval<T>().getReadIndex()),
               decltype(std::declval<T>().setReadIndex(
                       std::declval<const size_t&>() /* readIndex */)),
               decltype(std::declval<T>().getNumberOfElementsUsed()),
               decltype(std::declval<T>().setNumberOfElementsUsed(
                       std::declval<const size_t&>() /* numberOfElementsUsed */))>>
{
    /**
     * This Allocator *does* offer metadata access functions.
     * The reason behind this is that it might persist the pointers
     * and data in-between instantiations. Think of a filesystem or similar.
     */

    // This additionally tests for correct return types
    static constexpr bool value =
            std::is_same<typename outpost::result_of<decltype (&T::getReadIndex)(T)>::type,
                         const size_t&>::value
            && std::is_same<typename outpost::result_of<decltype (
                                    &T::setReadIndex)(T, const size_t& /* readIndex */)>::type,
                            void>::value
            && std::is_same<
                    typename outpost::result_of<decltype (&T::getNumberOfElementsUsed)(T)>::type,
                    const size_t&>::value
            && std::is_same<typename outpost::result_of<decltype (&T::setNumberOfElementsUsed)(
                                    T, const size_t& /* numberOfElementsUsed */)>::type,
                            void>::value;
};

template <class T, class = void>
struct ImplementsAtomicOperations
{
    /**
     * Allocator does not offer *atomic* metadata access functions.
     * This is the default and used where no (critical) power-losses occur.
     */
    static constexpr bool value = false;
};

template <class T>
struct ImplementsAtomicOperations<
        T,
        void_t<decltype(std::declval<T>().setReadIndexAndElementsUsedAtomically(
                std::declval<const size_t&>() /* readIndex */,
                std::declval<const size_t&>() /* numberOfElementsUsed */))>>
{
    /**
     * The case where the Allocator *does* offer atomic metadata access functions.
     * It needs to handle the atomicity itself.
     * The function is needed when `pop()`ing an element.
     */
    static constexpr bool value = std::is_same<
            typename outpost::result_of<decltype (&T::setReadIndexAndElementsUsedAtomically)(
                    T, const size_t& /* readIndex */, const size_t& /* numberOfElementsUsed */)>::
                    type,
            void>::value;
};

// -----

}  // namespace detail

/**
 * @brief Class that holds and owns the allocator
 */
template <class T,
          typename = typename std::enable_if<detail::ImplementsAllocator<T>::mandatoryFunctions,
                                             void>::type>
class AllocatorStorage
{
public:
    using Allocator = T;

    template <typename... Args>
    constexpr explicit AllocatorStorage(Args&&... args) : mAllocator(std::forward<Args>(args)...)
    {
    }

protected:
    constexpr const Allocator&
    getAllocator() const
    {
        return mAllocator;
    }

    constexpr Allocator&
    getAllocator()
    {
        return mAllocator;
    }

private:
    Allocator mAllocator;
};

// ---- Wrappers for optional atomic function(s) ----

/**
 * @brief Wrapper that implements atomic functions in case the Allocator does not.
 */
template <class T,
          typename =
                  typename std::enable_if<detail::ImplementsMetadataAccess<T>::value, void>::type>
class AtomicWrapper : public AllocatorStorage<T>
{
    // Allocator offers Metadata access, but not atomic operations
public:
    template <typename... Args>
    constexpr explicit AtomicWrapper(Args&&... args) :
        AllocatorStorage<T>(std::forward<Args>(args)...)
    {
        // std::cout << "AtomicWrapper: Allocator offers Metadata access, but _not_ atomic
        // operations" << std::endl;
    }

protected:
    constexpr void
    setReadIndexAndElementsUsedAtomically(const size_t& readIndex,
                                          const size_t& numberOfElementsUsed)
    {
        // default implementation where allocator does not offer atomic set
        // std::cout << "Using wrapper-default::setReadIndexAndElementsUsedAtomically()" <<
        // std::endl;
        getAllocator().setReadIndex(readIndex);
        getAllocator().setNumberOfElementsUsed(numberOfElementsUsed);
    }

    using AllocatorStorage<T>::getAllocator;
};

/**
 * @brief Wrapper that forwards atomic functions to the Allocator if it offers them.
 */
template <class T>
class AtomicWrapper<
        T,
        typename std::enable_if<detail::ImplementsAtomicOperations<T>::value, void>::type>
    : public AllocatorStorage<T>
{
public:
    template <typename... Args>
    constexpr explicit AtomicWrapper(Args&&... args) :
        AllocatorStorage<T>(std::forward<Args>(args)...)
    {
        /* Allocator takes responsibility of initializing
         * read Index and numberOfElementsUsed
         * This is to be able to save this in a different data structure / persistence
         */
        // std::cout << "AtomicWrapper: Allocator offers Metadata access and atomic operations"
        //           << std::endl;
    }

protected:
    constexpr void
    setReadIndexAndElementsUsedAtomically(const size_t& readIndex,
                                          const size_t& numberOfElementsUsed)
    {
        getAllocator().setReadIndexAndElementsUsedAtomically(readIndex, numberOfElementsUsed);
    }

    using AllocatorStorage<T>::getAllocator;
};

// ----

// ---- Wrappers for optional Metadata function ----

/**
 * @brief Wrapper that *owns* metadata and offers access to it; for simple Allocators.
 */
template <class T,
          typename = typename std::enable_if<detail::ImplementsAllocator<T>::mandatoryFunctions,
                                             void>::type>
class MetadataWrapper : public AllocatorStorage<T>
{
    // Case where Allocator does not offer Metadata access functions.
    // Also directly implements "atomic" metadata access function.
public:
    template <typename... Args>
    constexpr explicit MetadataWrapper(Args&&... args) :
        AllocatorStorage<T>(std::forward<Args>(args)...), mReadIndex(0), mNumberOfElementsUsed(0)
    {
        // std::cout << "MetadataWrapper: Allocator does _not_ offer Metadata access functions"
        //           << std::endl;
    }

protected:
    constexpr const size_t&
    getReadIndex() const
    {
        return mReadIndex;
    }

    constexpr void
    setReadIndex(const size_t& readIndex)
    {
        mReadIndex = readIndex;
    }

    constexpr const size_t&
    getNumberOfElementsUsed() const
    {
        return mNumberOfElementsUsed;
    }

    constexpr void
    setNumberOfElementsUsed(const size_t& numberOfElementsUsed)
    {
        mNumberOfElementsUsed = numberOfElementsUsed;
    }

    constexpr void
    setReadIndexAndElementsUsedAtomically(const size_t& readIndex,
                                          const size_t& numberOfElementsUsed)
    {
        mReadIndex = readIndex;
        mNumberOfElementsUsed = numberOfElementsUsed;
    }

    using AllocatorStorage<T>::getAllocator;

private:
    size_t mReadIndex;
    size_t mNumberOfElementsUsed;
};

/**
 * @brief Wrapper that forwards metadata access functions
 *        for Allocators that manage metadata themselves.
 */
template <class T>
class MetadataWrapper<T, typename std::enable_if<detail::ImplementsMetadataAccess<T>::value>::type>
    : public AtomicWrapper<T>
{
    // Implementation where Allocator offers Metadata access functions.
    // Uses AtomicWrapper that provides either the Allocators' or own atomic functions
public:
    template <typename... Args>
    // cppcheck-suppress uninitMemberVar
    constexpr explicit MetadataWrapper(Args&&... args) :
        AtomicWrapper<T>(std::forward<Args>(args)...)
    {
        /* Allocator takes responsibility of initializing
         * \c mReadIndex and \c mNumberOfElementsUsed
         * This is to be able to save this in a different data structure / persistence
         */
        // std::cout << "MetadataWrapper: Allocator does offer Metadata access functions" <<
        // std::endl;
    }

protected:
    constexpr const size_t&
    getReadIndex() const
    {
        return getAllocator().getReadIndex();
    }

    constexpr void
    setReadIndex(const size_t& readIndex)
    {
        getAllocator().setReadIndex(readIndex);
    }

    constexpr const size_t&
    getNumberOfElementsUsed() const
    {
        return getAllocator().getNumberOfElementsUsed();
    }

    constexpr void
    setNumberOfElementsUsed(const size_t& numberOfElementsUsed)
    {
        getAllocator().setNumberOfElementsUsed(numberOfElementsUsed);
    }

    constexpr void
    setReadIndexAndElementsUsedAtomically(const size_t& readIndex,
                                          const size_t& numberOfElementsUsed)
    {
        AtomicWrapper<T>::setReadIndexAndElementsUsedAtomically(readIndex, numberOfElementsUsed);
    }

    using AllocatorStorage<T>::getAllocator;
};

// -----

}  // namespace ring_buffer
}  // namespace container
}  // namespace outpost

#endif  // OUTPOST_UTILS_RING_BUFFER_DETAIL_H
