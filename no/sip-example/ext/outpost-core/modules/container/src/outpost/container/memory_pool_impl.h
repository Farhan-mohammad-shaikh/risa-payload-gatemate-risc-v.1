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

#ifndef OUTPOST_CONTAINER_MEMORY_POOL_IMPL_H
#define OUTPOST_CONTAINER_MEMORY_POOL_IMPL_H

#include "outpost/container/memory_pool.h"

namespace outpost
{
namespace container
{

template <size_t chunkSize, size_t numberOfChunks, size_t alignment>
MemoryPool<chunkSize, numberOfChunks, alignment>::~MemoryPool()
{
    OUTPOST_ASSERT(getNumberOfFreeChunks() == numberOfChunks, "memory-leak");
}

template <size_t chunkSize, size_t numberOfChunks, size_t alignment>
MemoryPool<chunkSize, numberOfChunks, alignment>::MemoryPool() :
    mAvailableChunks(numberOfChunks), mNextFreeChunkIndex(0U)
{
    mBuffer.fill(0);

    // let each element point to the next (n + 1) one
    for (IndexType i = 0U; i < numberOfChunks; i++)
    {
        uint8_t* chunk = index2Chunk(i);
        setIndexInChunk(chunk, i + 1);
    }

    // point to the first free element
    mNextFreeChunkIndex = 0U;
}

template <size_t chunkSize, size_t numberOfChunks, size_t alignment>
const outpost::Slice<uint8_t>
MemoryPool<chunkSize, numberOfChunks, alignment>::allocate()
{
    if (isAvailable())
    {
        uint8_t* chunk = index2Chunk(mNextFreeChunkIndex);
        mNextFreeChunkIndex = getIndexFromChunk(chunk);
        mAvailableChunks--;
        return outpost::asSliceUnsafe(chunk, chunkSize);
    }
    else
    {
        return outpost::Slice<uint8_t>::empty();
    }
}

template <size_t chunkSize, size_t numberOfChunks, size_t alignment>
void
MemoryPool<chunkSize, numberOfChunks, alignment>::free(uint8_t*& chunk)
{
    OUTPOST_ASSERT(chunk != nullptr, "Can not free nullptr");

    if (chunk != nullptr)
    {
        OUTPOST_ASSERT(isInFreeList(chunk) == false, "double-free");

        setIndexInChunk(chunk, mNextFreeChunkIndex);
        mNextFreeChunkIndex = chunk2index(chunk);
        mAvailableChunks++;
        chunk = nullptr;
    }
}

template <size_t chunkSize, size_t numberOfChunks, size_t alignment>
void
MemoryPool<chunkSize, numberOfChunks, alignment>::free(const outpost::Slice<uint8_t>& chunk)
{
    auto* ptr = chunk.begin();
    this->free(ptr);
}

template <size_t chunkSize, size_t numberOfChunks, size_t alignment>
void
MemoryPool<chunkSize, numberOfChunks, alignment>::free(outpost::Slice<uint8_t>& chunk)
{
    free(static_cast<const outpost::Slice<uint8_t>&>(chunk));
    chunk = outpost::Slice<uint8_t>::empty();
}

template <size_t chunkSize, size_t numberOfChunks, size_t alignment>
constexpr bool
MemoryPool<chunkSize, numberOfChunks, alignment>::isAvailable() const
{
    return mAvailableChunks > 0U;
}

template <size_t chunkSize, size_t numberOfChunks, size_t alignment>
constexpr bool
MemoryPool<chunkSize, numberOfChunks, alignment>::isEmpty() const
{
    return !this->isAvailable();
}

template <size_t chunkSize, size_t numberOfChunks, size_t alignment>
constexpr size_t
MemoryPool<chunkSize, numberOfChunks, alignment>::getNumberOfFreeChunks() const
{
    return mAvailableChunks;
}

// ---------------------------------------------------------------------------
template <size_t chunkSize, size_t numberOfChunks, size_t alignment>
constexpr uint8_t*
MemoryPool<chunkSize, numberOfChunks, alignment>::index2Chunk(const IndexType index)
{
    OUTPOST_ASSERT(index < numberOfChunks, "Index out of bounds");
    return &mBuffer[index * chunkSize];
}

template <size_t chunkSize, size_t numberOfChunks, size_t alignment>
constexpr typename MemoryPool<chunkSize, numberOfChunks, alignment>::IndexType
MemoryPool<chunkSize, numberOfChunks, alignment>::chunk2index(uint8_t* chunk) const
{
    OUTPOST_ASSERT(chunk >= &mBuffer[0] && chunk < &mBuffer[numberOfChunks * chunkSize],
                   "Chunk is not part of this pool");
    OUTPOST_ASSERT((chunk - &mBuffer[0]) % chunkSize == 0U, "Does not point to start of chunk");
    return (chunk - &mBuffer[0]) / chunkSize;
}

template <size_t chunkSize, size_t numberOfChunks, size_t alignment>
void
MemoryPool<chunkSize, numberOfChunks, alignment>::setIndexInChunk(uint8_t* chunk,
                                                                  IndexType index) const
{
    outpost::Serialize serialize(chunk);
    serialize.store(index);
}

template <size_t chunkSize, size_t numberOfChunks, size_t alignment>
typename MemoryPool<chunkSize, numberOfChunks, alignment>::IndexType
MemoryPool<chunkSize, numberOfChunks, alignment>::getIndexFromChunk(const uint8_t* chunk) const
{
    outpost::Deserialize deserialize(chunk);
    return deserialize.read<IndexType>();
}

template <size_t chunkSize, size_t numberOfChunks, size_t alignment>
constexpr bool
MemoryPool<chunkSize, numberOfChunks, alignment>::isInFreeList(const uint8_t* chunk) const
{
    IndexType current = mNextFreeChunkIndex;
    bool found = false;
    for (size_t i = 0; i < getNumberOfFreeChunks() && !found; i++)
    {
        const uint8_t* currentChunk = index2Chunk(current);
        found = (currentChunk == chunk);
        current = getIndexFromChunk(currentChunk);
    }
    return found;
}

template <size_t chunkSize, size_t numberOfChunks, size_t alignment>
constexpr const uint8_t*
MemoryPool<chunkSize, numberOfChunks, alignment>::index2Chunk(const IndexType index) const
{
    OUTPOST_ASSERT(index < numberOfChunks, "Index out of bounds");
    return &mBuffer[index * chunkSize];
}

// ---------------------------------------------------------------------------
template <typename T, size_t numberOfObjects>
template <typename... Args>
T*
ObjectPool<T, numberOfObjects>::newObject(Args&&... args)
{
    if (isAvailable())
    {
        auto chunk = this->allocate();
        // should never happen in practice since the memory is aligned
        OUTPOST_ASSERT(reinterpret_cast<intptr_t>(chunk.begin()) % alignof(T) == 0,
                       "Address must be multiple of alignment");
        return new (chunk.begin()) T(std::forward<Args>(args)...);
    }
    else
    {
        return nullptr;
    }
}

template <typename T, size_t numberOfObjects>
void
ObjectPool<T, numberOfObjects>::deleteObject(T*& object)
{
    OUTPOST_ASSERT(object != nullptr, "Can not delete nullptr");

    if (object != nullptr)
    {
        object->~T();
        uint8_t* chunk = reinterpret_cast<uint8_t*>(object);
        // Can happen if an invalid pointer is passed to this method
        OUTPOST_ASSERT(reinterpret_cast<intptr_t>(chunk) % alignof(T) == 0,
                       "Address must be multiple of alignment");
        this->free(chunk);
        object = nullptr;
    }
}

template <typename T, size_t numberOfObjects>
constexpr bool
ObjectPool<T, numberOfObjects>::isAvailable() const
{
    return BaseClass::isAvailable();
}

template <typename T, size_t numberOfObjects>
constexpr bool
ObjectPool<T, numberOfObjects>::isEmpty() const
{
    return BaseClass::isEmpty();
}

template <typename T, size_t numberOfObjects>
constexpr size_t
ObjectPool<T, numberOfObjects>::getNumberOfFreeObjects() const
{
    return BaseClass::getNumberOfFreeChunks();
}

}  // namespace container
}  // namespace outpost

#endif  // OUTPOST_CONTAINER_MEMORY_POOL_IMPL_H
