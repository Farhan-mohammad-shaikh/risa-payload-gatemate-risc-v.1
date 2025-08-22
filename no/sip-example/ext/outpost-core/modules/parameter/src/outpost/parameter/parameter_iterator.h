/*
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2022, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_PARAMETER_PARAMETER_ITERATOR_H_
#define OUTPOST_PARAMETER_PARAMETER_ITERATOR_H_

#include "parameter.h"

namespace outpost
{
namespace parameter
{
/**
 * A parameter iterator to use for a array of array pointer
 */
template <typename Friend>
class ParameterIterator
{
    friend Friend;

public:
    using DiffType = int;

    ParameterIterator(const ParameterIterator&) = default;
    ParameterIterator&
    operator=(const ParameterIterator&) = default;

    inline bool
    operator==(const ParameterIterator& o) const;
    inline bool
    operator!=(const ParameterIterator& o) const;

    inline ParameterIterator&
    operator+=(DiffType i);
    inline ParameterIterator&
    operator-=(DiffType i);

    inline ParameterIterator
    operator+(DiffType i) const;
    inline ParameterIterator
    operator-(DiffType i) const;

    inline ParameterIterator&
    operator++();
    inline ParameterIterator&
    operator--();
    inline ParameterIterator
    operator++(int);
    inline ParameterIterator
    operator--(int);

    inline DiffType
    operator-(const ParameterIterator& o) const;

    inline const ParameterBase*
    operator->() const;
    inline const ParameterBase&
    operator*() const;

private:
    // For friend
    ParameterIterator() = default;

    const ParameterBase** mPointer = nullptr;
};
}  // namespace parameter
}  // namespace outpost

#include "parameter_iterator_impl.h"

#endif
