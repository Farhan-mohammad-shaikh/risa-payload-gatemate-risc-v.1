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

#ifndef OUTPOST_CONTAINER_MEMORY_POOL_H
#define OUTPOST_CONTAINER_MEMORY_POOL_H

#include "outpost/container/best_uint_for.h"
#include <outpost/base/testing_assert.h>
#include <outpost/storage/serialize.h>
#include <outpost/utils/limits.h>

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <new>

namespace outpost
{
namespace container
{

// ---------------------------------------------------------------------------
/**
 * \class MemoryPool
 *
 * Simple memory pool which organises a list of free chunks which allows to
 * implemnt \c allocate and \c free with O(1).
 *
 * \tparam chunkSize
 *      chunk size
 *
 * \tparam numberOfChunks
 *      pool size
 *
 * \tparam alignment
 *      the alignment of the buffer
 */
template <size_t chunkSize, size_t numberOfChunks, size_t alignment = 1>
class MemoryPool
{
public:
    // The size must be a multiple of the alignment. Otherwise it is not possible to
    // arrange them in the memory block without further effort and without breaking the
    // the alignment. This may not apply to packed structures.
    static_assert(chunkSize % alignment == 0, "Size must be multiple of alignment");

    using IndexType = BestUIntFor<numberOfChunks>;
    static constexpr size_t bufferSize = numberOfChunks * chunkSize;

    // A chunk must hold the index of the next free element. Therefore it must
    // be large enough to hold IndexType
    static_assert(chunkSize >= sizeof(IndexType),
                  "Chunk size too small for the number of chunks requested");

    explicit MemoryPool();

    MemoryPool(const MemoryPool&) = delete;

    MemoryPool&
    operator=(const MemoryPool&) = delete;

    ~MemoryPool();

    /**
     * Allocates a chunk from the pool. If the pool is empty, a nullptr will be
     * returned.
     *
     * O(1)
     *
     * \return the allocated memory as slice. If pool is empty a empty slice
     *      will be returned.
     */
    const outpost::Slice<uint8_t>
    allocate();

    /**
     * Free a chunk. The pointer must be previously allocated from this pool.
     * The pointer will be set to nullptr.
     *
     * O(1)
     *
     * \param chunk
     *      the chunk to free
     */
    void
    free(uint8_t*& chunk);

    /**
     * Free a chunk. The pointer must be previously allocated from this pool.
     * The Slice will be set to empty.
     *
     * O(1)
     *
     * \param chunk
     *      the chunk to free
     */
    void
    free(outpost::Slice<uint8_t>& chunk);

    /**
     * Free a chunk. The pointer must be previously allocated from this pool.
     * The Slice will not be set to empty, because it is `const`.
     *
     * O(1)
     *
     * \param chunk
     *      the chunk to free
     */
    void
    free(const outpost::Slice<uint8_t>& chunk);

    /**
     * \retval true, if the pool is not empty
     * \retval false, if the pool is empty
     */
    inline constexpr bool
    isAvailable() const;

    /**
     * \retval false, if the pool is empty
     * \retval true, if the pool is not empty
     */
    inline constexpr bool
    isEmpty() const;

    /**
     * \return number of free chunks
     */
    inline constexpr size_t
    getNumberOfFreeChunks() const;

private:
    inline constexpr uint8_t*
    index2Chunk(const IndexType index);

    inline constexpr IndexType
    chunk2index(uint8_t* chunk) const;

    inline void
    setIndexInChunk(uint8_t* chunk, IndexType index) const;

    inline IndexType
    getIndexFromChunk(const uint8_t* chunk) const;

    inline constexpr bool
    isInFreeList(const uint8_t* chunk) const;

    inline constexpr const uint8_t*
    index2Chunk(const IndexType index) const;

    IndexType mAvailableChunks;
    IndexType mNextFreeChunkIndex;
    alignas(alignment) std::array<uint8_t, bufferSize> mBuffer;
};

// ---------------------------------------------------------------------------
/**
 * \class ObjectPool
 *
 * Simple object pool which organises a list of free objects which allows to
 * implemnt \c newObject and \c deleteObject with O(1).
 *
 * \tparam T
 *      type of the object
 *
 * \tparam numberOfObjects
 *      pool size
 */
template <typename T, size_t numberOfObjects>
class ObjectPool : private MemoryPool<sizeof(T), numberOfObjects, alignof(T)>
{
public:
    using BaseClass = MemoryPool<sizeof(T), numberOfObjects, alignof(T)>;

    explicit ObjectPool() = default;

    ObjectPool(const ObjectPool&) = delete;

    ObjectPool&
    operator=(const ObjectPool&) = delete;

    ~ObjectPool() = default;

    /**
     * Allocates a object from the pool. If the pool is empty, a nullptr will
     * be returned. The constructor will be called with the given args.
     *
     * O(1)
     *
     * \param args
     *      The arguments forwarded to the constructor
     *
     * \return pointer to the allocated object
     */
    template <typename... Args>
    T*
    newObject(Args&&... args);

    /**
     * Delete a object. The object must be previously allocated from this pool.
     * The pointer will be set to nullptr. The destructor of the object will be
     * called.
     *
     * O(1)
     *
     * \param object
     *      the object to delete
     */
    void
    deleteObject(T*& object);

    /**
     * \retval true, if the pool is not empty
     * \retval false, if the pool is empty
     */
    inline constexpr bool
    isAvailable() const;

    /**
     * \retval false, if the pool is empty
     * \retval true, if the pool is not empty
     */
    inline constexpr bool
    isEmpty() const;

    /**
     * \return number of free chunks
     */
    inline constexpr size_t
    getNumberOfFreeObjects() const;

private:
};

}  // namespace container
}  // namespace outpost

#include "outpost/container/memory_pool_impl.h"

#endif  // OUTPOST_CONTAINER_MEMORY_POOL_H
