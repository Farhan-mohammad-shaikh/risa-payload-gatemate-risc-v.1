/*
 * Copyright (c) 2017, Fabian Greif
 * Copyright (c) 2022, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_ITERATOR_H
#define OUTPOST_UTILS_ITERATOR_H

namespace outpost
{
// forward declaration
template <typename T>
struct ConstArrayIterator;

template <typename T>
struct DefaultConstArrayIterator;

template <typename T>
struct ArrayIterator
{
    explicit inline ArrayIterator(T* parameters);

    inline ArrayIterator(const ArrayIterator& other);

    inline ArrayIterator&
    operator=(const ArrayIterator& other);
    inline ArrayIterator&
    operator++();
    inline ArrayIterator&
    operator--();
    inline bool
    operator==(ArrayIterator other) const;
    inline bool
    operator!=(ArrayIterator other) const;

protected:
    friend struct ConstArrayIterator<T>;

    T* mPos;
};

template <typename T>
struct DefaultArrayIterator : public ArrayIterator<T>
{
    explicit inline DefaultArrayIterator(T* parameters);

    inline T&
    operator*();
    inline T*
    operator->();

private:
    friend struct DefaultConstArrayIterator<T>;
};

// ----------------------------------------------------------------------------
/**
 * Iterator for immutable arrays.
 *
 * \author  Fabian Greif
 */
template <typename T>
struct ConstArrayIterator
{
    explicit inline ConstArrayIterator(const T* parameters);

    inline ConstArrayIterator(const ArrayIterator<T>& other);
    inline ConstArrayIterator(const ConstArrayIterator& other);

    inline ConstArrayIterator&
    operator=(const ConstArrayIterator& other);
    inline ConstArrayIterator&
    operator++();
    inline ConstArrayIterator&
    operator--();
    inline bool
    operator==(ConstArrayIterator other) const;
    inline bool
    operator!=(ConstArrayIterator other) const;

protected:
    const T* mPos;
};

template <typename T>
struct DefaultConstArrayIterator : public ConstArrayIterator<T>
{
    explicit inline DefaultConstArrayIterator(const T* parameters);

    inline DefaultConstArrayIterator(const DefaultArrayIterator<T>& other);

    inline const T&
    operator*() const;
    inline const T*
    operator->() const;
};

}  // namespace outpost

#include "iterator_impl.h"

#endif
