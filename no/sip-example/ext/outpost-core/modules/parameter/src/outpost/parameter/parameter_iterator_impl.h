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

#ifndef OUTPOST_PARAMETER_PARAMETER_ITERATOR_IMPL_H_
#define OUTPOST_PARAMETER_PARAMETER_ITERATOR_IMPL_H_

#include "parameter_iterator.h"

namespace outpost
{
namespace parameter
{
template <typename Friend>
inline bool
ParameterIterator<Friend>::operator==(const ParameterIterator& o) const
{
    return mPointer == o.mPointer;
}

template <typename Friend>
inline bool
ParameterIterator<Friend>::operator!=(const ParameterIterator& o) const
{
    return mPointer != o.mPointer;
}

template <typename Friend>
inline ParameterIterator<Friend>&
ParameterIterator<Friend>::operator+=(DiffType i)
{
    mPointer += i;
    return *this;
}

template <typename Friend>
inline ParameterIterator<Friend>&
ParameterIterator<Friend>::operator-=(DiffType i)
{
    mPointer -= i;
    return *this;
}

template <typename Friend>
inline ParameterIterator<Friend>
ParameterIterator<Friend>::operator+(DiffType i) const
{
    ParameterIterator ret = *this;
    ret += i;
    return ret;
}

template <typename Friend>
inline ParameterIterator<Friend>
ParameterIterator<Friend>::operator-(DiffType i) const
{
    ParameterIterator ret = *this;
    ret -= i;
    return ret;
}

template <typename Friend>
inline ParameterIterator<Friend>&
ParameterIterator<Friend>::operator++()
{
    return (*this) += 1;
}

template <typename Friend>
inline ParameterIterator<Friend>&
ParameterIterator<Friend>::operator--()
{
    return (*this) -= 1;
}

template <typename Friend>
inline ParameterIterator<Friend>
ParameterIterator<Friend>::operator++(int)
{
    ParameterIterator ret = *this;
    (*this) += 1;
    return ret;
}

template <typename Friend>
inline ParameterIterator<Friend>
ParameterIterator<Friend>::operator--(int)
{
    ParameterIterator ret = *this;
    (*this) -= 1;
    return ret;
}

template <typename Friend>
inline typename ParameterIterator<Friend>::DiffType
ParameterIterator<Friend>::operator-(const ParameterIterator& o) const
{
    return this->mPointer - o.mPointer;
}

template <typename Friend>
inline const ParameterBase*
ParameterIterator<Friend>::operator->() const
{
    return *mPointer;
}

template <typename Friend>
inline const ParameterBase&
ParameterIterator<Friend>::operator*() const
{
    return **mPointer;
}

}  // namespace parameter
}  // namespace outpost

#endif
