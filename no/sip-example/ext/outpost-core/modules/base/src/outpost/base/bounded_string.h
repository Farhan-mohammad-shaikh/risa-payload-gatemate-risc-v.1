/*
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Pascal Pieper
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_BASE_BOUNDED_STRING_H
#define OUTPOST_BASE_BOUNDED_STRING_H

#include <outpost/base/slice.h>
#include <outpost/base/testing_assert.h>

#include <optional>
#include <string_view>

namespace outpost
{

/**
 * BoundedString is a "view" on a const char array (string).
 * It should replace the null-terminated strings used otherwise.
 *
 * It replaces the \c basic_string_view which is only available
 * with >= c++17: https://en.cppreference.com/w/cpp/string/basic_string_view
 *
 * \author  Pascal Pieper
 */
class BoundedString : public outpost::Slice<const char>
{
public:
    using MaybeIndex = std::optional<IndexType>;

    constexpr BoundedString() : Slice(empty()){};

    /**
     * \brief Constructor from degraded string pointer.
     *        Do not use unless necessary.
     */
    // cppcheck-suppress noExplicitConstructor
    constexpr BoundedString(const char* string) :
        BoundedString(std::basic_string_view<char>{string})
    {
        // std::cout << "BS from const char pointer: " << string << std::endl;
    }

    /**
     * \brief Constructor from std::basic_string_view.
     *
     * Uses the view to determine the actual size.
     * Is not basic_string_view<**const** char> because this is done implicitly in the view.
     */
    // cppcheck-suppress noExplicitConstructor
    constexpr BoundedString(const std::basic_string_view<char>& sv) : Slice(sv.begin(), sv.end())
    {
        // std::cout << "BS from basic string view: " << sv << std::endl;
    }

    /**
     * \brief Static array constructor
     *        Uses \c BoundedString::strnlen() for length deduction.
     * \warning Assumes null terminated array, or else becomes zero length
     */
    template <size_t N>
    // cppcheck-suppress noExplicitConstructor
    constexpr BoundedString(const char (&string)[N]) :
        Slice{Slice<const char>::unsafe(string, BoundedString::strnlen(string, N))}
    {
        // std::cout << "BS from string[" << N << "]: " << std::string(begin(), end()) << std::endl;
    }

    /**
     * \brief Static array constructor
     *        Uses \c BoundedString::strnlen() for length deduction.
     * \warning Assumes non-null terminated array
     */
    template <size_t N>
    // cppcheck-suppress noExplicitConstructor
    constexpr BoundedString(const std::array<char, N>& array) :
        Slice{Slice<const char>::unsafe(array.begin(), BoundedString::strnlen(array))}
    {
        // std::cout << "BS from array[" << N << "]: " << arr.begin() << std::endl;
    }

    // cppcheck-suppress noExplicitConstructor
    constexpr BoundedString(const char* begin, const char* end) : Slice(begin, end)
    {
        // std::cout << "BS from first '" << std::string(begin, end) << "'" <<
        // std::endl;
    }

    /**
     * \brief Constructor for conversion from `uint8_t` buffer (which is not char)
     * \param end is not the last valid element, but one slot after that!
     *
     * \tparam SomeUint8 matches against `unsigned char` and `const unsigned char`
     */
    template <typename SomeUint8,
              class = typename std::enable_if<
                      std::is_same<uint8_t,
                                   typename std::remove_const<SomeUint8>::type>::value>::type>
    inline BoundedString(SomeUint8* begin, SomeUint8* end) :
        Slice{reinterpret_cast<const char*>(begin), reinterpret_cast<const char*>(end)}
    {
    }

    /**
     * \brief Constructor for conversion from `uint8_t` Slice (which is not char)
     *
     * \tparam SomeUint8 matches against `unsigned char` and `const unsigned char`
     */
    template <typename SomeUint8,
              class = typename std::enable_if<
                      std::is_same<uint8_t,
                                   typename std::remove_const<SomeUint8>::type>::value>::type>
    inline BoundedString(const outpost::Slice<SomeUint8>& slice) :
        Slice{reinterpret_cast<const char*>(slice.begin()),
              reinterpret_cast<const char*>(slice.end())}
    {
    }

    /**
     * \brief Constructor for c-strings with maximum length
     * \param maxElements Including the last valid char
     */
    constexpr BoundedString(const char* string, size_t maxElements) :
        Slice{asSliceUnsafe(string, BoundedString::strnlen(string, maxElements))}
    {
    }

    /**
     * Constructor from Slice (rvalue) without length check.
     * \warning does not do a \c strlen()! Assumes bounded slice.
     *
     * \tparam someChar matches against `char` and `const char`
     */
    template <typename someChar = const char,
              class = typename std::enable_if<
                      std::is_same<char, typename std::remove_const<someChar>::type>::value>::type,
              class = void>
    // cppcheck-suppress noExplicitConstructor ; move constructor
    constexpr BoundedString(Slice<someChar>&& other) : Slice{std::move(other)}
    {
    }

    /**
     * Constructor from Slice (const lvalue) without length check.
     * \warning does not do a \c strlen()! Assumes bounded slice.
     *
     * \tparam someChar matches against `char` and `const char`
     */
    template <typename someChar = const char,
              class = typename std::enable_if<
                      std::is_same<char, typename std::remove_const<someChar>::type>::value>::type,
              class = void>
    // cppcheck-suppress noExplicitConstructor
    constexpr BoundedString(const Slice<someChar>& other) : Slice{other}
    {
    }

    // intentionally no conversion to std::string to not have the dependency here.
    // use `#include <unittest/boundend_string_helper.h>` for that.

    /**
     * Copies the contents along with a null-termination into a given slice.
     *
     * \param[in] to
     *      A slice of at least \c size() elements.
     *
     * \return
     *      If \param[in] to was big enough, a slice of the copied bytes
     *      but including the null terminator.
     *      Returns \c outpost::Slice<char>::empty() if \param[in] to
     *      was not big enough.
     */
    constexpr outpost::Slice<char>
    copyZeroTerminatedInto(const outpost::Slice<char>& to) const
    {
        const size_t neededBytes = size() + 1;  // for null termination
        if (to.getNumberOfElements() < neededBytes)
        {
            // does not fit
            return outpost::Slice<char>::empty();
        }
        to.copyFrom(*this);
        to[size()] = 0;  // null termination
        return to.first(neededBytes);
    }

    /**
     * Copies the contents along with a null-termination into a given slice,
     * and returns a raw pointer to it.
     *
     * \param[in] to
     *      A slice of at least \c size() elements.
     *
     * \return
     *      If \param[in] to was big enough, a pointer to the copied bytes
     *      including the null terminator.
     *
     * \warning
     *      Returns \c nullptr if \param[in] to was not big enough.
     */
    constexpr char*
    copyZeroTerminatedIntoUnsafe(const outpost::Slice<char>& to) const
    {
        const auto copy = copyZeroTerminatedInto(to);
        if (copy.getNumberOfElements() > 0)
        {
            return copy.getDataPointer();
        }
        else
        {
            return nullptr;
        }
    }

    /**
     * Finds the last position of a given char and returns the offset to it, if found.
     */
    constexpr MaybeIndex
    findLastOf(const char& toFind) const
    {
        for (IndexType i = 0; i < size(); i++)
        {
            const IndexType offs = (size() - 1) - i;
            if (operator[](offs) == toFind)
            {
                return offs;
            }
        }

        return std::nullopt;
    }

    /**
     * Finds the first position of a given char and returns the offset to it, if found.
     */
    constexpr MaybeIndex
    findFirstOf(const char& toFind) const
    {
        for (IndexType i = 0; i < size(); i++)
        {
            if (operator[](i) == toFind)
            {
                return i;
            }
        }

        return std::nullopt;
    }

    constexpr bool
    operator==(const BoundedString& other) const
    {
        // just forward to slice
        return isContentEqual(other);
    }

    constexpr bool
    operator!=(const BoundedString& other) const
    {
        return !operator==(other);
    }

    /**
     * Returns the number of (char) elements.
     */
    constexpr LengthType
    size() const
    {
        return getNumberOfElements();
    }

    // The following functions are Type aliases from base "Slice",
    // but returning the "correct" type.
    // This could be implemented similar to what util::TypesafeMask does,
    // but this would litter the Slice a bit.
    // So it was chosen to just "forward" these functions here.

    /**
     * Create a sub-slice from the beginning of the slice with
     * up to @param firstElements requested elements.
     */
    constexpr BoundedString
    first(const LengthType& firstElements) const
    {
        return Slice::first(firstElements);
    }

    /**
     * Create a sub-slice which skips the given number of elements.
     *
     * The slice contains the data after the given number until the
     * end of the array.
     */
    constexpr BoundedString
    skipFirst(const LengthType& numberOfElements) const
    {
        return Slice::skipFirst(numberOfElements);
    }

    /**
     * Create a sub-slice from the end of the slice.
     */
    constexpr BoundedString
    last(const LengthType& lastElements) const
    {
        return Slice::last(lastElements);
    }

    /**
     * Create a sub-slice from the beginning of the slice by removing
     * the given number of elements from the end.
     */
    constexpr BoundedString
    skipLast(const LengthType& numberOfElements) const
    {
        return Slice::skipLast(numberOfElements);
    }

    /**
     * Create a sub-slice from two indices.
     *
     * \param   firstIndex
     *      First index which is included in the range.
     * \param   lastIndex
     *      Last index which **is not included** in range.
     */
    constexpr BoundedString
    subRange(const IndexType& firstIndex, const IndexType& lastIndex) const
    {
        return Slice::subRange(firstIndex, lastIndex);
    }

    // Own, constexpr, implementation of string length calculators.
    // In some OSes there is no implementation, and these `constexpr` functions
    // can, if known at compiletime, warn about read-out-of-bounds!
    //
    // The decision of "return Number of elements on overflow" is based
    // on the usefulness in where buffer do *not* need to have the classic
    // `char[maxNumChars + 1] buf; // +1 because of zero termination` anymore.
    // Due to buffer size knowledge, the existence of a zero termination *after*
    // `maxNumChars` chars can be inferred: Thus, we save one byte for every buffer.

    /**
     * @brief Own, constexpr, implementation of `strnlen`.
     *        It is not defined everywhere, so we do it on our own.
     * @return "Number of elements" on overflow!
     */
    static constexpr size_t
    strnlen(const char* const string, size_t N)
    {
        if (string == nullptr)
        {
            return 0;
        }

        for (size_t i = 0; i < N; i++)
        {
            if (string[i] == 0)
            {
                return i;
            }
        }
        return N;
    }

    /**
     * @brief Own, constexpr, implementation of `strnlen`.
     *        It is not defined everywhere, so we do it on our own.
     * @return "Number of elements" on overflow!
     */
    template <size_t N>
    static constexpr size_t
    strnlen(const char (&string)[N])
    {
        return BoundedString::strnlen(string, N);
    }

    /**
     * @brief Own, constexpr, implementation of `strnlen`.
     *        It is not defined everywhere, so we do it on our own.
     * @return "Number of elements" on overflow!
     */
    template <size_t N>
    static constexpr size_t
    strnlen(const std::array<char, N>& string)
    {
        return BoundedString::strnlen(string.begin(), N);
    }
};

}  // namespace outpost

#endif  // OUTPOST_BASE_BOUNDED_STRING_H
