/*
 * Copyright (c) 2024, Pascal Pieper
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_CONTAINER_COMPILE_TIME_LIST_H
#define OUTPOST_CONTAINER_COMPILE_TIME_LIST_H

#include <stddef.h>

#include <utility>
#include <variant>

namespace outpost::container
{
namespace detail
{

/**
 * helper that makes references also const
 *
 * (in contrast to `make_const_t<int&> == int&`!)
 */
template <typename T>
using make_reference_const_t =
        std::conditional_t<std::is_reference_v<T>, const std::remove_reference_t<T>&, const T>;

static_assert(std::is_same_v<const int, make_reference_const_t<int>>);
static_assert(std::is_same_v<const int&, make_reference_const_t<int&>>);
static_assert(std::is_same_v<int* const, make_reference_const_t<int*>>);

// End type for unique typenames
template <typename T, typename... Ts>
struct unique : T
{
    using type = T;
};

// Helper that takes a set of typenames and
// shortens them to unique occurrences
template <typename... Ts, typename U, typename... Us>
struct unique<std::variant<Ts...>, U, Us...>
    : std::conditional_t<(std::is_same_v<U, Ts> || ...),
                         unique<std::variant<Ts...>, Us...>,
                         unique<std::variant<Ts..., U>, Us...>>
{
};

// Helper that takes a set of typenames and
// shortens them to unique occurrences
template <typename... Ts>
using unique_variant = typename unique<std::variant<>, Ts...>::type;

}  // namespace detail

/**
 * Generic, compile-time, type-safe, list.
 * Accessed by templated recursion to hold different types,
 * known and resolved in compile-time.
 *
 * This generic type may or may not have elements.
 */
template <typename... Ts>
struct CompileTimeList
{
    // For template type deduction, announce that this constructor is always available.
    explicit constexpr CompileTimeList(Ts...);

    // the following forward declarations are for IDE indexer hints only.
    // the `void` implementations will *never* be used.

    /**
     * @brief Check whether there is a next element available.
     */
    static constexpr bool
    hasNext();

    /**
     * @brief Get a reference to the current (head) element.
     *        Only implemented if it actually has an element.
     */
    constexpr void
    get() const;

    /**
     * @brief Get the next Elements
     *        Only implemented if it is *not* an empty list.
     * @return Another `CompileTimeList` with the Tail elements (or `EmptyList`!)
     */
    constexpr void
    next() const;

    /**
     * @brief Get a list copy but force holding const elements
     */
    CompileTimeList<detail::make_reference_const_t<Ts>...> constexpr asConstList() const;
};

// ----------------------------------------------------------------------------

namespace compile_time_list
{

// Alias for "No element"
using Nothing = std::monostate;
// Alias for a CompileTimeList with no elements
using EmptyList = CompileTimeList<>;

}  // namespace compile_time_list

// ----------------------------------------------------------------------------

/**
 * @brief The List-type that contains nothing (tail).
 *
 * No function `next()` or `get(void)` is implemented.
 * Calling these will result in a compile error.
 */
template <>
struct CompileTimeList<>
{
    // The recursion end function when creating const lists.
    static auto constexpr asConstList()
    {
        return compile_time_list::EmptyList{};
    }

    // recursion end function when checking size
    static constexpr size_t
    size()
    {
        return 0;
    }

    // recursion end function when run-time indexed accessing members
    template <typename>
    static constexpr auto
    get(size_t)
    {
        // forward to usual getter
        return compile_time_list::Nothing{};
    }
};

// ----------------------------------------------------------------------------

/**
 * @brief Generic, compile-time, type-safe, list.
 * Accessed by templated recursion to hold different types,
 * known and resolved in compile-time.
 *
 * This is the implementation for Lists that contain elements.
 */
template <typename HeadType, typename... TailTypes>
struct CompileTimeList<HeadType, TailTypes...>
{
    // The type of the current list
    using CurrentList = CompileTimeList<HeadType, TailTypes...>;

    // A list containing the tail types
    using NextList = CompileTimeList<TailTypes...>;

    // A std::variant that can hold all alternative types
    // that this and the following list elements return
    // (or Nothing -> std::monostate)
    using Variant = detail::unique_variant<compile_time_list::Nothing,
                                           std::remove_reference_t<HeadType>,
                                           std::remove_reference_t<TailTypes>...>;

    // Deleted, because this would be an empty list
    constexpr CompileTimeList() = delete;

    // copy (element) constructor
    constexpr CompileTimeList(const HeadType& head, const TailTypes&... tail) :
        mHead(head), mNext(tail...)
    {
    }

    // move (element) constructor
    template <typename H = HeadType,
              typename... Ts,
              // only if type is not already a reference
              typename = std::enable_if_t<std::is_same_v<std::remove_reference_t<H>, H>>>
    constexpr CompileTimeList(HeadType&& head, TailTypes&&... tail) :
        mHead(std::forward<HeadType>(head)), mNext(std::forward<TailTypes>(tail)...)
    {
    }

    // copy (list) constructor
    constexpr CompileTimeList(const CompileTimeList<HeadType, TailTypes...>& other) = default;
    // move (list) constructor
    constexpr CompileTimeList(CompileTimeList<HeadType, TailTypes...>&& other) = default;

    /**
     * @brief special prepend constructor that creates a new list
     *        from a new Head and an existing list
     * @param head Element to be the new head
     * @param tail List to point `next()` to
     */
    constexpr CompileTimeList(const HeadType& head, const CompileTimeList<TailTypes...>& tail) :
        mHead(head), mNext(tail)
    {
    }

    /**
     * @brief Return the compile-time known number of Elements
     */
    static constexpr size_t
    size()
    {
        return 1 + NextList::size();
    }

    /**
     * @brief Get a reference to the (compile time) indexed const value
     * @tparam i is the compile-time constant offset (`0` to `size()-1`)
     * @return Guaranteed to be valid element
     */
    template <size_t i = 0, typename = std::enable_if_t<bool{i < CurrentList::size()}>>
    constexpr const auto&
    get() const
    {
        if constexpr (i == 0)
        {
            return mHead;
        }
        else
        {
            if constexpr (hasNext())
            {
                return next().template get<i - 1>();
            }  // cppcheck-suppress missingReturn
            // intentionally no else branch.
            // This would be noticed by the compiler as
            // an out-of-bounds access and
            // is also guarded in the template definition
            // (both checked during compile-time)
        }
    }

    /**
     * @brief Get a reference to the (compile time) indexed value
     * @tparam i is the compile-time constant offset (0 to (size()-1))
     * @return Guaranteed to be valid element
     */
    template <size_t i = 0, typename = std::enable_if_t<bool{i < CurrentList::size()}>>
    constexpr auto&
    get()
    {
        if constexpr (i == 0)
        {
            return mHead;
        }
        else
        {
            if constexpr (hasNext())
            {
                return next().template get<i - 1>();
            }  // cppcheck-suppress missingReturn
            // intentionally no else branch.
            // This would be noticed by the compiler as
            // an out-of-bounds access and
            // is also guarded in the template definition
            // (both checked during compile-time)
        }
    }

    /**
     * @brief Get a copy to the (run-time) indexed value
     * @return An std::variant that can hold all possible return types
     */
    template <typename V = Variant>
    constexpr V
    get(size_t i) const
    {
        if (i == 0)
        {
            return mHead;
        }
        else
        {
            // no need for `hasNext()` check,
            // because `EmptyList` has `Nothing` getter
            return next().template get<V>(i - 1);
        }
    }

    /**
     * @brief Get the next CompileTimeList element.
     * @return Maybe `EmptyList`, which indicates the end.
     */
    constexpr const NextList&
    next() const
    {
        return mNext;
    }

    /**
     * @brief Get the next CompileTimeList element.
     * @return Maybe `EmptyList`, which indicates the end.
     */
    constexpr NextList&
    next()
    {
        return mNext;
    }

    /**
     * @brief Easy, always-available check whether there is a next element
     */
    static constexpr bool
    hasNext()
    {
        return !std::is_same_v<std::remove_cv_t<NextList>, compile_time_list::EmptyList>;
    }

    /**
     * @brief Returns the list, but with const types
     *
     * If called as `ReferenceList`, this will degrade the type
     * to CompileTimeList<...>. But it will still contain references.
     */
    constexpr auto
    asConstList() const
    {
        return CompileTimeList<detail::make_reference_const_t<HeadType>,
                               detail::make_reference_const_t<TailTypes>...>{mHead,
                                                                             mNext.asConstList()};
    }

private:
    // The current element
    HeadType mHead;

    // A list of the tail types
    NextList mNext;
};

// -------------------------------------------------------------

/**
 * A `CompileTimeList`, but forcing to hold references instead of values
 */
template <typename... Ts>
struct ReferenceList : CompileTimeList<Ts&...>
{
    // force taking the reference to the elements
    explicit constexpr ReferenceList(Ts&... ts) :
        CompileTimeList<Ts&...>(std::forward<Ts&>(ts)...){};

    // Construct from a (const) CompileTimeList to ReferenceList
    explicit constexpr ReferenceList(const CompileTimeList<Ts...>& other) :
        CompileTimeList<Ts&...>(other){};

    // r-value copy constructor
    constexpr ReferenceList(ReferenceList&& other) = default;

    // l-value copy constructor
    constexpr ReferenceList(const ReferenceList& other) = default;
};

}  // namespace outpost::container

#endif /* OUTPOST_CONTAINER_COMPILE_TIME_LIST_H */
