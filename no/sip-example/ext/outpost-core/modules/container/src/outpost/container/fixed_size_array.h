/*
 * Copyright (c) 2014-2017, Fabian Greif
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2022, Adrian Roeser
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2021, pfef_to
 * Copyright (c) 2022-2023, Jan-Gerd Mess
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2023, Kirstein
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_FIXED_SIZE_ARRAY_H
#define OUTPOST_FIXED_SIZE_ARRAY_H

#include <stddef.h>
#include <string.h>  // for memcpy

#include <type_traits>

// workaround missing "is_trivially_copyable" in g++ < 5.0
#if __GNUG__ && (__GNUC__ < 5) && !__clang__
#define IS_TRIVIALLY_COPYABLE(T) __has_trivial_copy(T)
#else
#define IS_TRIVIALLY_COPYABLE(T) std::is_trivially_copyable<T>::value
#endif

namespace outpost
{
/**
 * Wrapper for C style arrays with a fixed length.
 *
 * Similar to std::array in C++11.
 *
 * \author  Fabian Greif
 */
template <typename T, size_t N>
class FixedSizeArray
{
public:
    typedef T Type;
    typedef typename std::remove_const<T>::type NonConstType;
    typedef const NonConstType ConstType;

    friend class FixedSizeArray<const T, N>;

    /// Works only for types which can be copied bitwise for now
    /// because of the usage of memcpy
    static_assert(IS_TRIVIALLY_COPYABLE(T), "T must be copyable via memcpy");

    constexpr FixedSizeArray() = default;

    template <typename... U>
    explicit constexpr FixedSizeArray(const U... ts) : mData{ts...}
    {
    }

    /**
     * Initialize directly from a C style array.
     *
     * The array needs to still have all the type information attached and
     * must not have degraded to a pointer type.
     *
     * Example:
     * \code
     * uint8_t array[7];
     * FixedSizeArray<uint8_t, 7> wrappedArray(array);
     * \endcode
     *
     * \param array
     *      Array which should be wrapped.
     */
    explicit inline FixedSizeArray(T (&array)[N])
    {
        memcpy(mData, array, sizeof(mData));
    }

    // This constructor is non-explicit to allow for a conversion from
    // const to non-const
    // cppcheck-suppress noExplicitConstructor
    inline FixedSizeArray(const FixedSizeArray<NonConstType, N>& rhs)
    {
        memcpy(mData, rhs.mData, sizeof(mData));
    }

    /**
     * Initialize from a pointer to an array.
     *
     * Example:
     * \code
     * uint8_t array[7];
     * FixedSizeArray<uint8_t, 4> wrappedArray = FixedSizeArray<uint8_t,
     * 4>::fromArray(array); \endcode
     *
     * \param array
     *      Pointer to the first element of the array.
     * \param numberOfElements
     *      Number of elements in the array.
     */
    static inline FixedSizeArray<T, N>
    fromArray(NonConstType* array)
    {
        FixedSizeArray a;
        memcpy(a.mData, array, sizeof(a.mData));
        return a;
    }

    static inline FixedSizeArray<T, N>
    fromArray(ConstType* array)
    {
        FixedSizeArray a;
        memcpy(a.mData, array, sizeof(a.mData));
        return a;
    }

    /**
     * Get number of elements in the array.
     */
    static inline size_t
    getNumberOfElements()
    {
        return N;
    }

    /**
     * Access elements of the array.
     *
     * \code
     * FixedSizeArray<uint8_t, ...> array(...);
     *
     * for (size_t i = 0; i < array.getNumberOfElements(); ++i)
     * {
     *     array[i] = i;
     * }
     * \endcode
     *
     * \warning
     *      No out-of-bound error checking is performed.
     */
    inline T&
    operator[](size_t index)
    {
        return mData[index];
    }

    /**
     * Access elements of the array.
     *
     * \warning
     *      No out-of-bound error checking is performed.
     */
    inline const T&
    operator[](size_t index) const
    {
        return mData[index];
    }

private:
    T mData[N]{{}};
};

template <typename T, size_t N>
class FixedSizeArrayView
{
public:
    typedef T Type;
    typedef typename std::remove_const<T>::type NonConstType;
    typedef const NonConstType ConstType;

    // STL compatibility
    typedef T value_type;
    typedef const T* const_iterator;

    friend class FixedSizeArrayView<const T, N>;

    // False positive: array cannot be pointer to const, because mData is not const
    // cppcheck-suppress constParameterPointer
    FixedSizeArrayView(T* array, size_t offset) : mData(&array[offset])
    {
    }

    /**
     * Initialize directly from a C style array.
     *
     * The array needs to still have all the type information attached and
     * must not have degraded to a pointer type.
     *
     * Example:
     * \code
     * uint8_t array[7];
     * FixedSizeArrayView<uint8_t, 7> wrappedArray(array);
     * \endcode
     *
     * \param array
     *      Array with should be wrapped.
     */
    explicit inline FixedSizeArrayView(T (&array)[N]) : mData(array)
    {
    }

    // This constructor is non-explicit to allow for a conversion from
    // const to non-const
    // cppcheck-suppress noExplicitConstructor
    inline FixedSizeArrayView(const FixedSizeArray<T, N>& rhs) : mData(rhs.mData)
    {
    }

    /**
     * Get number of elements in the array.
     */
    static inline size_t
    getNumberOfElements()
    {
        return N;
    }

    /**
     * Get raw pointer to data.
     *
     * \warning
     *      Operations working directly with the raw pointer are unsafe.
     */
    inline constexpr T*
    getDataPointer() const
    {
        return mData;
    }

    /**
     * Access elements of the array.
     *
     * \code
     * FixedSizeArrayView<uint8_t, ...> array(...);
     *
     * for (size_t i = 0; i < array.getNumberOfElements(); ++i)
     * {
     *     array[i] = i;
     * }
     * \endcode
     *
     * \warning
     *      No out-of-bound error checking is performed.
     */
    inline T&
    operator[](size_t index)
    {
        return mData[index];
    }

    /**
     * Access elements of the array.
     *
     * \warning
     *      No out-of-bound error checking is performed.
     */
    inline const T&
    operator[](size_t index) const
    {
        return mData[index];
    }

    inline
    operator FixedSizeArrayView<const T, N>() const
    {
        return FixedSizeArrayView<const T, N>(mData, 0);
    }

    inline const_iterator
    begin() const
    {
        return &mData[0];
    }

    inline const_iterator
    end() const
    {
        return &mData[N];
    }

private:
    T* mData;
};

}  // namespace outpost

#endif
