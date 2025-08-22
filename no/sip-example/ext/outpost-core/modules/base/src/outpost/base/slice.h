/*
 * Copyright (c) 2014-2019, Fabian Greif
 * Copyright (c) 2019-2021, Jan Malburg
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2022, Jan-Gerd Mess
 * Copyright (c) 2022, Mess, Jan-Gerd
 * Copyright (c) 2023, Sommer, Jan
 * Copyright (c) 2023-2024, Pascal Pieper
 * Copyright (c) 2023-2024, Pieper, Pascal
 * Copyright (c) 2023, Kirstein
 * Copyright (c) 2024, Passenberg, Felix Constantin
 * Copyright (c) 2024, roes_a0
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SLICE_H
#define OUTPOST_SLICE_H

#include <outpost/base/testing_assert.h>

#include <inttypes.h>
#include <stddef.h>
#include <string.h>

#include <gsl/span>
#include <optional>
#include <type_traits>

// workaround missing "is_trivially_copyable" in g++ < 5.0
#if __GNUG__ && (__GNUC__ < 5) && !__clang__
#define IS_TRIVIALLY_COPYABLE(T) __has_trivial_copy(T)
#else
#define IS_TRIVIALLY_COPYABLE(T) std::is_trivially_copyable<T>::value
#endif

namespace outpost
{

template <class T, typename = void>
struct has_equals_operator
{
    static constexpr bool value = false;
};

template <class T>
struct has_equals_operator<
        T,
        std::enable_if_t<
                std::is_same_v<decltype(std::declval<T>().operator==(std::declval<T>())), bool>>>
{
    static constexpr bool value = true;
};

template <class T>
static constexpr auto has_equals_operator_v = has_equals_operator<T>::value;

/**
 * Slices are a dynamically-sized view into a contiguous sequence of memory.
 *
 * Slices are a view into a block of memory represented as a pointer and
 * a length. The implementation here is inspired by [Rust slices][1] and
 * the [`span<T>` type][2] defined in the [C++ Core Guidelines][3].
 *
 * [1]: https://doc.rust-lang.org/std/slice/
 * [2]: http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#SS-views
 * [3]: http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines
 *
 * \author  Fabian Greif
 */
template <typename ElementType>
class Slice
{
public:
    using IndexType = std::size_t;
    using LengthType = std::size_t;

    // constants and types for compatibility with STL/GSL
    using value_type = ElementType;
    using reference = ElementType&;
    using const_reference = const ElementType&;
    using pointer = ElementType*;
    using const_pointer = const ElementType*;

    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    // old member types
    using Iterator = iterator;
    using ReverseIterator = reverse_iterator;

    using uint8Type = typename std::
            conditional<std::is_const<ElementType>::value, const uint8_t, uint8_t>::type;

    friend Slice<typename std::remove_const<ElementType>::type>;
    friend Slice<const ElementType>;

    template <class T, size_t N>
    friend constexpr Slice<T> asSlice(T (&arr)[N]);

    template <class T, size_t N>
    friend constexpr Slice<T>
    asSlice(std::array<T, N>& arr);

    template <class T, size_t N>
    friend constexpr Slice<const T>
    asSlice(const std::array<T, N>& arr);

    /**
     * Create from a fixed size gsl::span.
     */
    template <size_t N>
    explicit constexpr inline Slice(gsl::span<ElementType, N> span) :
        mData(span.data()), mNumberOfElements(N)
    {
    }

    /**
     * Create from a gsl::span.
     *
     * This allows creation from e.g. STL standard containers and C style
     * arrays.
     */
    explicit constexpr inline Slice(gsl::span<ElementType> span) :
        mData(span.data()), mNumberOfElements(span.size())
    {
    }

    /**
     * Create from an iterator pair.
     * @param end is not the last element, but one slot after that!
     */
    constexpr inline Slice(const iterator& firstElement, const iterator& end) :
        mData(firstElement), mNumberOfElements(std::distance(firstElement, end))
    {
    }

    Slice(const Slice& rhs) = default;

    Slice(Slice&& rhs) = default;

    Slice&
    operator=(const Slice& rhs) = default;

    Slice&
    operator=(Slice&& rhs) = default;

    /**
     * Generate an empty array.
     */
    static constexpr inline Slice
    empty()
    {
        return Slice(nullptr, IndexType(0));
    }

    /**
     * Initialize from a pointer to an array.
     *
     * \warning
     *      This operation is unsafe and should be avoided if possible. Create
     *      a Slice directly from the original array and pass that around
     *      instead. The slice can be split into smaller chunks by using
     *      first() and last().
     *
     * Example:
     * \code
     * uint8_t* ptr = ...;
     * auto slice = Slice<uint8_t>::unsafe(ptr, 7);
     * \endcode
     *
     * \param array
     *      Pointer to the first element of the array.
     * \param numberOfElements
     *      Number of elements in the array.
     */
    static constexpr inline Slice
    unsafe(ElementType* array, const LengthType& numberOfElements)
    {
        return Slice(array, numberOfElements);
    }

    /**
     * Get number of elements in the array.
     */
    inline constexpr LengthType
    getNumberOfElements() const
    {
        return mNumberOfElements;
    }

    /**
     * Access elements of the array.
     *
     * \code
     * Slice<uint8_t> array(...);
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
    inline constexpr ElementType&
    operator[](const IndexType& index) const
    {
        OUTPOST_CONSTEXPR_ASSERT(index < getNumberOfElements(), "Slice access out of bounds");
        return mData[index];
    }

    /**
     * Get raw pointer to data.
     *
     * \warning
     *      Operations working directly with the raw pointer are unsafe.
     */
    inline constexpr ElementType*
    getDataPointer() const
    {
        OUTPOST_CONSTEXPR_ASSERT(getNumberOfElements() > 0, "Empty Slice data pointer access");
        return getNumberOfElements() == 0 ? nullptr : mData;
    }

    /**
     * Begin Iterator
     *
     * Iterators of Slice<T> and const Slice<T> provide write access.
     * Iterators of Slice<const T> and const Slice<const T> are per return type iterators,
     * but writing is inhibited to mData by its constness.
     */
    inline constexpr iterator
    begin() const
    {
        return getNumberOfElements() == 0 ? iterator(nullptr) : iterator(mData);
    }

    /**
     * End Iterator
     *
     * Iterators of Slice<T> and const Slice<T> provide write access.
     * Iterators of Slice<const T> and const Slice<const T> are per return type iterators,
     * but writing is inhibited to mData by its constness.
     */
    inline constexpr iterator
    end() const
    {
        return getNumberOfElements() == 0 ? iterator(nullptr) : iterator(&mData[mNumberOfElements]);
    }

    /**
     * Reverse Begin Iterator
     *
     * Iterators of Slice<T> and const Slice<T> provide write access.
     * Iterators of Slice<const T> and const Slice<const T> are per return type iterators,
     * but writing is inhibited to mData by its constness.
     */
    inline constexpr reverse_iterator
    rbegin() const
    {
        return reverse_iterator(end());
    }

    /**
     * Reverse end Iterator
     *
     * Iterators of Slice<T> and const Slice<T> provide write access.
     * Iterators of Slice<const T> and const Slice<const T> are per return type iterators,
     * but writing is inhibited to mData by its constness.
     */
    inline constexpr reverse_iterator
    rend() const
    {
        return reverse_iterator(begin());
    }

    /**
     * Set all elements in the slice to a given value
     * @param v      The value to set all values to
     */
    constexpr void
    fill(const ElementType& v) const
    {
        for (IndexType i = 0; i < mNumberOfElements; ++i)
        {
            mData[i] = v;
        }
    }

    /**
     * Create a sub-slice from the beginning of the slice with
     * up to @param firstElements requested elements.
     */
    constexpr Slice
    first(const LengthType& firstElements) const
    {
        if (firstElements > mNumberOfElements)
        {
            // Limit to available number of elements
            return Slice(mData, mNumberOfElements);
        }
        else
        {
            return Slice(mData, firstElements);
        }
    }

    /**
     * Create a sub-slice which skips the given number of elements.
     *
     * The slice contains the data after the given number until the
     * end of the array.
     */
    constexpr Slice
    skipFirst(const LengthType& numberOfElements) const
    {
        if (numberOfElements >= mNumberOfElements)
        {
            return Slice(mData, IndexType(0));
        }
        else
        {
            return Slice(mData + numberOfElements, mNumberOfElements - numberOfElements);
        }
    }

    /**
     * Create a sub-slice from the end of the slice.
     */
    constexpr Slice
    last(const LengthType& lastElements) const
    {
        if (lastElements > mNumberOfElements)
        {
            // Limit to available number of elements
            return Slice(mData, mNumberOfElements);
        }
        else
        {
            return Slice(mData + (mNumberOfElements - lastElements), lastElements);
        }
    }

    /**
     * Create a sub-slice from the beginning of the slice by removing
     * the given number of elements from the end.
     */
    constexpr Slice
    skipLast(const LengthType& numberOfElements) const
    {
        if (numberOfElements >= mNumberOfElements)
        {
            return Slice(mData, IndexType(0));
        }
        else
        {
            return Slice(mData, mNumberOfElements - numberOfElements);
        }
    }

    /**
     * Create a sub-slice from a starting index and a length.
     */
    constexpr Slice
    subSlice(const IndexType& offset, const LengthType& length) const
    {
        return first(offset + length).last(length);
    }

    /**
     * Create a sub-slice from two indices.
     *
     * \param   firstIndex
     *      First index which is included in the range.
     * \param   lastIndex
     *      Last index which **is not included** in range.
     */
    constexpr Slice
    subRange(const IndexType& firstIndex, const IndexType& lastIndex) const
    {
        OUTPOST_CONSTEXPR_ASSERT(lastIndex >= firstIndex,
                                 "Slice::subRange() requested invalid (negative) range");
        return subSlice(firstIndex, lastIndex - firstIndex);
    }

    constexpr
    operator Slice<const ElementType>() const
    {
        return Slice<const ElementType>(mData, mNumberOfElements);
    }

    constexpr gsl::span<ElementType>
    asSpan() const
    {
        return gsl::span<ElementType>(mData, mNumberOfElements);
    }

    // Including padding bytes
    inline Slice<uint8Type>
    asUint8Slice() const
    {
        // uint8Type required such that non const types will be "uint8_t" and const types "const
        // uint8_t" The divisor just to be save when facing exotic architectures
        return Slice<uint8Type>::unsafe(reinterpret_cast<uint8Type*>(mData),
                                        mNumberOfElements
                                                * (sizeof(ElementType) / sizeof(uint8Type)));
    }

    /**
     * @brief Length-checking memcpy of source to the beginning of the slice.
     *
     * @warning Only available for trivially copyable classes
     *
     * @param src Pointer to source data.
     * @param length Length of source data to be copied.
     * @return true The slice was large enough, data was copied.
     * @return false Not enough space in the slice to copy all data, or non-zero length nullptr
     * source.
     */
    template <typename U = ElementType>
    typename std::enable_if<IS_TRIVIALLY_COPYABLE(U), bool>::type
    copyFrom(const void* __restrict src, const size_t& length) const
    {
        if (0 == length)
        {
            // zero length always fits
            return true;
        }
        if (nullptr == src)
        {
            // prevents using memcpy with nullptr
            return false;
        }
        if (length <= getNumberOfElements())
        {
            static_assert(sizeof(U) == sizeof(ElementType),
                          "Element type of source slice must have same size as Element type of "
                          "destination slice");

            memmove(mData, src, length * sizeof(ElementType));
            return true;
        }
        return false;
    }

    /**
     * @brief Convenient overload for slice to slice copy.
     *
     * @param source Slice containing source data.
     * @return true Slice data has been copied to destination slice.
     * @return false Source slice too large or invalid.
     */
    inline constexpr bool
    copyFrom(const Slice<const ElementType>& source) const
    {
        if (source.getNumberOfElements() > 0)
        {
            return copyFrom(source.getDataPointer(), source.getNumberOfElements());
        }
        else
        {
            return true;
        }
    }

    /**
     * Convenience function for concatenation of multiple Slices into a single buffer.
     * Returns a Slice to the used elements of the buffer, if it has enough space.
     * It can be treated as an alias for:
     * ```
     * mybuffer.copyFrom(element[1]);
     * mybuffer.skipFirst(element[1].getNumberOfElements()).copyFrom(element[2]);
     * ...
     * mybuffer.first(element[1].getNumberOfElements() + ...);
     *
     * @param elements One or more Slices containing source data.
     * @return optional<Slice>
     *         on success: SubSlice of all Slice data that has been copied to destination slice.
     *         on failure: Source slice too large or invalid. One or more copies
     *                     might have been performed.
     */
    std::optional<Slice> constexpr concatenateCopyFrom(
            const std::initializer_list<const Slice<const ElementType>> elements) const
    {
        IndexType ptr = 0;
        for (const auto& element : elements)
        {
            if (!skipFirst(ptr).copyFrom(element))
            {
                return std::nullopt;
            }
            ptr += element.getNumberOfElements();
        }
        return first(ptr);
    }

    /**
     * Comparison of _data_, not the pointers themselves.
     */
    template <typename U = ElementType>
    constexpr std::enable_if_t<has_equals_operator_v<U>, bool>
    isContentEqual(const Slice<const ElementType>& other) const
    {
        if (other.getNumberOfElements() != getNumberOfElements())
        {
            return false;
        }

        // same number of elements
        if (getNumberOfElements() == 0 || other.mData == this->mData)
        {
            return true;
        }

        // point to different data, with equal but > 0 nr. of elements (no nullptr!)
        const_iterator own = this->begin();
        const_iterator otr = other.begin();
        // length of own and otr are always the same
        while (own != this->end())
        {
            if (!(*own == *otr))
            {
                return false;
            }
            ++own;
            ++otr;
        }
        return true;
    }

    template <typename U = ElementType>
    constexpr std::enable_if_t<!has_equals_operator_v<U>, bool>
    isContentEqual(const Slice<const ElementType>& source) const
    {
        if (source.getNumberOfElements() != getNumberOfElements())
        {
            return false;
        }

        // same number of elements
        if (getNumberOfElements() == 0 || source.mData == this->mData)
        {
            return true;
        }

        // point to different data, with equal but > 0 nr. of elements (no nullptr!)
        return memcmp(this->begin(), source.begin(), getNumberOfElements() * sizeof(ElementType))
               == 0;
    }

private:
    inline constexpr Slice(ElementType* array, size_t numberOfElements) :
        mData(array), mNumberOfElements(numberOfElements)
    {
    }

    /// Pointer to the array
    ElementType* mData;

    /// Size of array
    IndexType mNumberOfElements;
};

/**
 * Initialize from a pointer to an array.
 *
 * \warning
 *      This operation is unsafe and should be avoided if possible. Create
 *      a Slice directly from the original array and pass that around
 *      instead. The slice can be split into smaller chunks by using
 *      first() and last().
 */
template <class ElementType>
constexpr Slice<ElementType>
asSliceUnsafe(ElementType* ptr, typename Slice<ElementType>::IndexType count)
{
    return Slice<ElementType>::unsafe(ptr, count);
}

/**
 * Create a slice from an iterator pair.
 */
template <class ElementType>
constexpr Slice<ElementType>
asSlice(ElementType* firstElement, ElementType* lastElement)
{
    return Slice<ElementType>(firstElement, lastElement);
}

/**
 * Create slice from a C-style array.
 */
template <class ElementType, size_t N>
constexpr Slice<ElementType>
asSlice(ElementType (&arr)[N])
{
    return Slice<ElementType>(arr, N);
}

/**
 * Create slice from a std::array.
 */
template <class ElementType, size_t N>
constexpr Slice<ElementType>
asSlice(std::array<ElementType, N>& arr)
{
    return Slice<ElementType>(&std::get<0>(arr), N);
}

template <class ElementType, size_t N>
constexpr Slice<const ElementType>
asSlice(const std::array<ElementType, N>& arr)
{
    return Slice<const ElementType>(&std::get<0>(arr), N);
}

template <class ElementType>
constexpr Slice<ElementType>
asSlice(std::array<ElementType, 0>&)
{
    return Slice<ElementType>::empty();
}

/**
 * Create slice from a STL compatible container.
 */
template <class Container>
Slice<typename Container::value_type>
asSlice(Container& cont)
{
    return Slice<typename Container::value_type>(cont);
}

/**
 * Create slice from a constant STL compatible container.
 * and from a gsl::span.
 */
template <class Container>
Slice<const typename Container::value_type>
asSlice(const Container& cont)
{
    return Slice<const typename Container::value_type>(cont);
}

struct EmptySlice
{
    template <typename T>
    constexpr
    operator Slice<T>() const
    {
        return Slice<T>::empty();
    }
};

/**
 * Generate empty slice which can be assigned to slices of any type. This
 * allows to set a slice to empty without repeating the type of the slice.
 * I.e.:
 *
 * \code
 *
 * mySlice = outpost::emptySlice();
 * mySlice = outpost::Slice<uint8_t>::empty();
 *
 * \endcode
 */
constexpr EmptySlice
emptySlice()
{
    return EmptySlice();
}

}  // namespace outpost

#endif
