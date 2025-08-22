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
 * \author Pascal Pieper
 *
 * \brief two array allocators for ringbuffer
 */

#ifndef OUTPOST_UTILS_RING_BUFFER_ALLOCATORS_ARRAY_H
#define OUTPOST_UTILS_RING_BUFFER_ALLOCATORS_ARRAY_H

#include "../ring_buffer.h"

#include <outpost/base/slice.h>

#include <inttypes.h>

#include <array>

namespace outpost
{

namespace container
{

/**
 * "Referenced" means that this Allocator gets a "reference" to a
 * continuous Slice of memory. Hence no need for a capacity template parameter.
 */
template <typename T>
struct ReferencedArrayAllocator
{
    using value_type = T;

    constexpr explicit ReferencedArrayAllocator(outpost::Slice<T> buffer) : mBuffer(buffer)
    {
    }

    void
    read(const size_t& offs, const outpost::Slice<T>& to) const
    {
        OUTPOST_ASSERT(offs + to.getNumberOfElements() <= capacity(), "read out of bounds");
        const auto ret = to.copyFrom(mBuffer.skipFirst(offs).first(to.getNumberOfElements()));
        OUTPOST_ASSERT(ret, "copyFrom did not succeed");
        static_cast<void>(ret);
    }

    void
    write(const size_t& offs, const outpost::Slice<const T>& from) const
    {
        OUTPOST_ASSERT(offs + from.getNumberOfElements() <= capacity(), "write out of bounds");
        const auto ret = mBuffer.skipFirst(offs).copyFrom(from);
        OUTPOST_ASSERT(ret, "array copy did not succeed");
        // suppress unused variable warning if not building with assertions
        static_cast<void>(ret);
    }

    // optional direct access
    /*
     * not outpost::Slice<**const** T> because some ringbuffer users
     * rely on a peek() or pop() 'ed Slice to write into.
     */
    constexpr const outpost::Slice<T>
    getBufferAt(const size_t& offs, const size_t& len) const
    {
        OUTPOST_CONSTEXPR_ASSERT(offs + len <= capacity(), "getBufferAt out of bounds!");
        return mBuffer.subSlice(offs, len);
    }

    constexpr size_t
    capacity() const
    {
        return mBuffer.getNumberOfElements();
    }

private:
    outpost::Slice<T> mBuffer;
};

template <typename T>
using ArrayRingBuffer = RingBuffer<ReferencedArrayAllocator<T>, T>;

/**
 * @brief Storage initialization class which allows for *inherited* memory ownership.
 *        This is to have a guaranteed initialization order for the ringbuffers.
 */
template <size_t numberOfElements, typename T>
class ArrayStorage
{
public:
    ArrayStorage()
    {
        // no initialization needed currently
    }

protected:
    constexpr outpost::Slice<T>
    getBuffer()
    {
        return outpost::Slice<T>(mBufferStorage);
    }

private:
    std::array<T, numberOfElements> mBufferStorage = {};
};

/*
 * @brief Array RingBuffer that owns the storage.
 * Needs to be a child of `ArrayRingBuffer` to be able to
 * pass around Referenced Ringbuffers by inheritance.
 *
 * I.e.:
 * ```
 * ArrayRingBufferStorage<20, uint8_t> storage;
 * ArrayRingBuffer* ringBuffer = &storage;
 * ```
 */
template <size_t size, typename T>
class ArrayRingBufferStorage : private ArrayStorage<size, T>, public ArrayRingBuffer<T>
{
public:
    ArrayRingBufferStorage() :
        ArrayStorage<size, T>(), ArrayRingBuffer<T>(ArrayStorage<size, T>::getBuffer())
    {
    }
};

}  // namespace container

}  // namespace outpost

#endif  // OUTPOST_UTILS_RING_BUFFER_ALLOCATORS_ARRAY_H
