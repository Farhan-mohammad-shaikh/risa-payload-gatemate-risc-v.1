/*
 * Copyright (c) 2013-2018, Fabian Greif
 * Copyright (c) 2018, Olaf Maibaum
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2022, Jan Sommer
 * Copyright (c) 2023, Jan-Gerd Mess
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_DEQUE_H
#define OUTPOST_DEQUE_H

#include <outpost/base/slice.h>

#include <stddef.h>
#include <stdint.h>

// Needed for memcpy
#include <string.h>

namespace outpost
{
enum class DequeAppendStrategy
{
    /// Only append data if all data can be added.
    complete,

    /// Append as much data as possible.
    partial
};

/**
 * Double ended queue.
 *
 * Internally organized as a ring buffer.
 *
 *                 tail --\                 /-- head
 *                        |                 |
 *              +------+------+---- ----+------+------+
 *            0 |      | data |   ...   | data |      | N-1
 *              +------+------+---- ----+------+------+
 *                 |      |                 |      |
 *     prepend() --/      |                 |      \-- append()
 *                        |                 |
 *           getFront() --/                 \-- getBack()
 *
 * \tparam  T   Type of the elements
 *
 * \warning This class don't check if the container is empty before
 *          a pop-operation. You have to do this by yourself!
 *
 * \author  Fabian Greif
 */
template <typename T, DequeAppendStrategy Strategy = DequeAppendStrategy::partial>
class Deque
{
public:
    typedef size_t Index;
    typedef Index Size;

public:
    /**
     * \deprecated
     *      Use the slice based constructor instead. This constructor will
     *      be removed in one of the next versions of the library.
     */
    Deque(T* backendBuffer, size_t n);

    explicit Deque(outpost::Slice<T> backendBuffer);

    // disable copy constructor
    Deque(const Deque&) = delete;

    // disable copy assignment operator
    Deque&
    operator=(const Deque&) = delete;

    inline bool
    isEmpty() const;

    inline bool
    isFull() const;

    inline Size
    getSize() const;

    inline Size
    getMaxSize() const;

    inline Size
    getAvailableSpace() const;

    /**
     * Clear the queue.
     *
     * \warning    This will discard all the items in the container
     */
    void
    clear();

    inline T&
    getFront();

    inline const T&
    getFront() const;

    inline T&
    getBack();

    inline const T&
    getBack() const;

    /**
     * \param value One value to append to the deque.
     *
     * \result True when value is appended.
     */
    bool
    append(const T& value);

    /**
     * Append a list of elements to the queue.
     *
     * If the append strategy is set to `partial` and not enough space is
     * available, the queue is filled up with the first values from the slice
     * until it is full.
     * Otherwise the append operation is aborted completely.
     *
     * \param values
     *      A slice to append to the deque.
     *
     * \result Number of appended values.
     */
    size_t
    append(outpost::Slice<T> values);

    bool
    prepend(const T& value);

    void
    removeBack();

    void
    removeFront();

private:
    /**
     * Depending if T is trivially copy assignable the corresponding implementation is selected.
     * If yes, memcpy is used, else element-wise copy assignment is used.
     *
     * This method will not check the input arguments for validity (same as memcpy)
     */
    template <typename U = T>
    typename std::enable_if<std::is_trivially_copy_assignable<U>::value>::type
    appendMultipleElements(Size index, outpost::Slice<T> values) const
    {
        memcpy(&mBuffer[index], values.getDataPointer(), sizeof(T) * values.getNumberOfElements());
    }

    template <typename U = T>
    typename std::enable_if<!std::is_trivially_copy_assignable<U>::value>::type
    appendMultipleElements(Size index, outpost::Slice<T> values)
    {
        for (Size i = 0; i < values.getNumberOfElements(); ++i)
        {
            mBuffer[index + i] = values[i];
        }
    }

    T* const mBuffer;
    const Size mMaxSize;

    Index mHead;
    Index mTail;
    Size mSize;
};
}  // namespace outpost

#include "deque_impl.h"

#endif
