#ifndef OUTPOST_CRC_TABLE_H
#define OUTPOST_CRC_TABLE_H

#include <outpost/base/slice.h>

#include <stddef.h>
#include <stdint.h>

/**
 * Custom implementation of the integer_sequence template in order to use it with C++11
 * Taken from: https://gist.github.com/ntessore/dc17769676fb3c6daa1f
 */
namespace std14
{
template <typename T, T... Ints>
struct integer_sequence
{
    typedef T value_type;

    static constexpr std::size_t
    size()
    {
        return sizeof...(Ints);
    }
};

template <std::size_t... Ints>
using index_sequence = integer_sequence<std::size_t, Ints...>;

template <typename T, std::size_t N, T... Is>
// cppcheck-suppress templateRecursion
struct make_integer_sequence : make_integer_sequence<T, N - 1, N - 1, Is...>
{
};

template <typename T, T... Is>
struct make_integer_sequence<T, 0, Is...> : integer_sequence<T, Is...>
{
};

template <std::size_t N>
using make_index_sequence = make_integer_sequence<std::size_t, N>;

template <typename... T>
using index_sequence_for = make_index_sequence<sizeof...(T)>;
}  // namespace std14

namespace outpost
{

/**
 * Template class for providing a precomputed CRC table for a reversed or non-reversed CRC
 * calculation.
 */
template <typename T, T polynomial, bool reversed>
struct CrcTable
{
    /**
     * Recursively performs bit steps during the computation of a CRC table element of a
     * non-reversed or reversed table
     */
    template <T crc, size_t N, bool rev = reversed>
    struct bitStep
    {
        static constexpr T step = bitStep<crc, 1, rev>::value;
        static constexpr T value = bitStep<step, N - 1, rev>::value;
    };

    /**
     * Performs one bit step during the computation of a CRC table element of a non-reversed table
     */
    template <T crc>
    struct bitStep<crc, 1, false>
    {
        static constexpr T mask = (static_cast<T>(1)) << (sizeof(T) * 8 - 1);
        static constexpr T value = crc & mask ? static_cast<T>(crc << 1) ^ polynomial : (crc << 1);
    };

    /**
     * Performs one bit step during the computation of a CRC table element of a reversed table
     */
    template <T crc>
    struct bitStep<crc, 1, true>
    {
        static constexpr T mask = 1;
        static constexpr T value = crc & mask ? static_cast<T>(crc >> 1) ^ polynomial : (crc >> 1);
    };

    /**
     * Declaration for template specializations (see below)
     */
    template <T crc, bool rev = reversed>
    struct computeTableElement
    {
    };

    /**
     * Initializes the computation of a CRC table element of a non-reversed table
     */
    template <T crc>
    struct computeTableElement<crc, false>
    {
        static constexpr size_t numOfSteps = 8;
        static constexpr T init = crc << ((sizeof(T) * 8) - 8);
        static constexpr T value = bitStep<init, numOfSteps, false>::value;
    };

    /**
     * Initializes the computation of a CRC table element of a reversed table
     */
    template <T crc>
    struct computeTableElement<crc, true>
    {
        static constexpr size_t numOfSteps = 8;
        static constexpr T init = crc;
        static constexpr T value = bitStep<init, numOfSteps, true>::value;
    };

    /**
     * Computes the according CRC table element for all integers in the integer sequence
     */
    template <size_t N, int... I>
    static constexpr std::array<T, N>
    computeTable_impl(std14::integer_sequence<int, I...>)
    {
        return {computeTableElement<I, reversed>::value...};
    }

    /**
     * Creates an integer_sequence as base for the computation of the CRC table
     */
    template <size_t N>
    static constexpr std::array<T, N>
    computeTable()
    {
        using Sequence = std14::make_integer_sequence<int, N>;
        return computeTable_impl<N>(Sequence{});
    }

    /**
     * For accessing the elements of the CRC table
     */
    T
    operator[](size_t i) const
    {
        return table[i];
    }

    static constexpr size_t tableLength = 256;
    static constexpr std::array<T, tableLength> table = computeTable<tableLength>();
};

template <typename T, T polynomial, bool reversed>
constexpr std::array<T, outpost::CrcTable<T, polynomial, reversed>::tableLength>
        outpost::CrcTable<T, polynomial, reversed>::table;

}  // namespace outpost

#endif
