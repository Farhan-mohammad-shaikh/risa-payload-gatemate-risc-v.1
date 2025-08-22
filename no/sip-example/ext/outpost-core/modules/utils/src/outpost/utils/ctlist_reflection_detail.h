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

#ifndef OUTPOST_UTILS_CTLIST_REFLECTION_DETAIL_H
#define OUTPOST_UTILS_CTLIST_REFLECTION_DETAIL_H

#include <outpost/container/compile_time_list.h>

#include <utility>

namespace outpost::reflection::detail
{

using EmptyList = container::compile_time_list::EmptyList;

// ----------------------------------------------------------------------------
// SFINAE check for `T::getMember()` in "visiting places" (nodes)
// ----------------------------------------------------------------------------

/// Helper that checks for `getMember() const`
template <typename T, typename = void>
struct ImplementsConstAccess
{
    // does *not* implement `getMember() const`
    static constexpr bool value = false;
};

/// Helper that checks for `getMember() const`
template <typename T>
struct ImplementsConstAccess<T, std::void_t<decltype(std::declval<const T>().getMember())>>
{
    // not checking the return type (it is complicated!)
    // does implement `getMember() const`
    static constexpr bool value = true;
};

/// Helper that checks for `getMember()`
template <typename T, typename = void>
struct ImplementsNonConstAccess
{
    // does *not* implement `getMember()`
    static constexpr bool value = false;
};

/// Helper that checks for `getMember()`
template <typename T>
struct ImplementsNonConstAccess<
        T,
        std::void_t<decltype(std::declval<std::remove_const_t<T>>().getMember())>>
{
    // not checking the return type (it is complicated!)
    // does implement `getMember()`
    static constexpr bool value = true;
};

/// Helper that checks for any (const || non-const) `T::getMember()`
template <typename T>
static constexpr bool implements_any_access_v =
        ImplementsConstAccess<T>::value || ImplementsNonConstAccess<T>::value;

// ----------------------------------------------------------------------------
// SFINAE check for `F::operator(T, maybe bool)` in Visitors
// ----------------------------------------------------------------------------

/// Helper that is valid if the `F::operator(Ts...)` is defined
/// (or at least possible)
template <typename F, typename... Ts>
using TestFunctorOperator = decltype(std::declval<F>()(std::declval<Ts>()...));

/**
 * Visitor Functor does *not* implement the usual `F::operator(T)`.
 * ... why even trying? This is mandatory!
 */
template <typename F, typename T, typename = void>
struct VisitorAcceptsElement
{
    // does *not* accept the type
    static constexpr bool value = false;
};

/**
 * Visitor Functor accepts type `F::operator(T)`.
 */
template <typename F, typename T>
struct VisitorAcceptsElement<F, T, std::void_t<TestFunctorOperator<F, T>>>
{
    // does accept element of type `F`
    static constexpr bool value = true;
};

/**
 * Helper that checks whether Functor `F` accepts element of type `T`.
 *
 * The check tests for `T` or `const T`, which is an "or" with the
 * perhaps existing predicate in `T`.
 * This is in line with the compiler choosing to const it.
 */
template <typename F, typename T>
static constexpr bool visitor_accepts_element_v =
        VisitorAcceptsElement<F, T>::value || VisitorAcceptsElement<F, std::add_const_t<T>>::value;

/**
 * Visitor Functor does *not* implement the "stop" call `F::operator(void)`.
 */
template <typename F, typename = void>
struct VisitorAcceptsVoid
{
    // does *not* accept the type
    static constexpr bool value = false;
};

/**
 * Visitor Functor does implement the "stop" call `F::operator(void)`.
 */
template <typename F>
struct VisitorAcceptsVoid<F, std::void_t<TestFunctorOperator<F>>>
{
    // does accept call `F::operator(void)`
    static constexpr bool value = true;
};

template <typename F>
static constexpr bool visitor_accepts_void_v = VisitorAcceptsVoid<F>::value;

/**
 * Visitor does not have a second parameter `F::operator(T, bool)` for
 * the info whether there are more elements coming.
 */
template <typename F, typename T, typename = void>
struct VisitorAcceptsHasNext
{
    // does *not* accept second parameter "hasNext"
    static constexpr bool value = false;
};

/**
 * Visitor *does* have a second parameter `F::operator(T, bool)` for
 * the info whether there are more elements coming.
 */
template <typename F, typename T>
struct VisitorAcceptsHasNext<F, T, std::void_t<TestFunctorOperator<F, T, bool>>>
{
    // does accept second parameter "hasNext"
    static constexpr bool value = true;
};

/** Helper that checks whether Functor `F` accepts `hasNext`: `F::operator(T, bool)`.
 *
 * The check tests for `T` or `const T`, which is an "or" with the
 * perhaps existing predicate in `T`.
 * This is in line with the compiler choosing to const it.
 */
template <typename F, typename T>
static constexpr bool visitor_accepts_hasNext_v =
        VisitorAcceptsHasNext<F, T>::value || VisitorAcceptsHasNext<F, std::add_const_t<T>>::value;

// ----------------------------------------------------------------------------
// Way of circumventing const / non-const access to only one function
// ----------------------------------------------------------------------------

/**
 * @brief Gets the constant list of a const type
 *        that has the const function defined (easy case)
 * @return \c CompileTimeList<...> with immutable types
 */
template <typename E, typename = std::void_t<decltype(std::declval<const E>().getMember())>>
constexpr auto
getConstList(const E& element);

/**
 * @brief Gets the constant list of a type
 *        that *has not* the const function defined, but
 *        at least has the function non-const defined.
 * @tparam E the element to access `getMember()` of
 * @return \c CompileTimeList<...> with immutable types
 */
template <typename E>
constexpr auto
getConstList(const E& element);

/**
 * @brief Dispatches call to \c s.getMember() either const or non-const
 * @return A \c CompileTimeList<...> with either mutable or un-mutable types
 */
template <typename T>
constexpr auto
getMaybeConstMember(T& s);

// ----------------------------------------------------------------------------
// "Tree" iteration (depth first) functions for Node or Leaf
// ----------------------------------------------------------------------------

/**
 * @brief Generic visitor for Nodes with descendants (branches).
 *
 * @tparam Element an object with `getMember()`, thus a branch.
 * @tparam VisitorFunction any Functor
 * @param hasNext Info whether there are more elements after that.
 */
template <typename Element, typename VisitorFunction>
constexpr std::enable_if_t<implements_any_access_v<Element>>
descent(Element& nodeWithList, VisitorFunction& fun, bool hasNext);

/**
 * @brief Generic visitor for Nodes without descendants (leafs).
 *
 * @tparam Element an object without `getMember()`, thus a leaf.
 * @tparam VisitorFunction any Functor
 * @param hasNext Info whether there are more elements after that.
 */
template <typename Element, typename VisitorFunction>
constexpr std::enable_if_t<!implements_any_access_v<Element>>
descent(Element& node, VisitorFunction& fun, bool hasNext);

/**
 * @brief Apply functor to leaf without `hasNext`
 *
 * Visit leaf with a Visitor that does not care about
 * the info whether there are further elements in the line.
 *
 * @tparam Element a leaf node
 * @tparam VisitorFunction Functor without bool as
 *         second parameter
 */
template <typename Element, typename VisitorFunction>
constexpr std::enable_if_t<!implements_any_access_v<Element>
                           && !visitor_accepts_hasNext_v<VisitorFunction, Element>>
visitLeaf(Element& node, VisitorFunction& fun, bool);

/**
 * @brief Apply functor to leaf with info `hasNext`
 *
 * Visit leaf with a Visitor that *does* care about
 * the info whether there are further elements in the line.
 *
 * @tparam Element a leaf node
 * @tparam VisitorFunction Functor with bool as
 *         second parameter (hasNext)
 */
template <typename Element, typename VisitorFunction>
constexpr std::enable_if_t<!implements_any_access_v<Element>
                           && visitor_accepts_hasNext_v<VisitorFunction, Element>>
visitLeaf(Element& node, VisitorFunction& fun, bool hasNext);

// ----------------------------------------------------------------------------
// List iterating functions (for determining the child nodes)
// ----------------------------------------------------------------------------

/**
 * @brief Recursion-stop-function of list iteration.
 *        Version where VisitorFunctor does not implement "stop" (void)
 */
template <typename VisitorFunction>
constexpr std::enable_if_t<!visitor_accepts_void_v<VisitorFunction>>
visit(EmptyList, VisitorFunction& /*fun*/, bool /*hasNext*/ = false);

/**
 * @brief Recursion-stop-function of list iteration.
 *        Version where VisitorFunctor *does* implement "stop" (void)
 */
template <typename VisitorFunction>
constexpr std::enable_if_t<visitor_accepts_void_v<VisitorFunction>>
visit(EmptyList, VisitorFunction& fun, bool hasNext = false);

/**
 * @brief Visiting function that iterates over Lists and descends
 *        into other lists or nodes via the above functions.
 *
 * Function that takes l-value lists (const / non-const)
 *
 * @param hasNext internal parameter, do not use unless known
 */
template <typename HeadElement,
          typename... NextElements,
          template <typename...>
          class List,  // neccessary to match const and non-const lists
          typename VisitorFunction>
constexpr void
visit(List<HeadElement, NextElements...>& list, VisitorFunction& fun, bool hasNext = false);

/**
 * @brief Visiting function that iterates over constant Lists and descends
 *        into other lists or nodes via the above functions.
 *
 * Function that takes temporary lists (const / non-const)
 *
 * @param hasNext internal parameter, do not use unless known
 */
template <typename HeadElement,
          typename... NextElements,
          template <typename...>
          class List,  // neccessary to match const and non-const lists
          typename VisitorFunction>
constexpr void
visit(List<HeadElement, NextElements...>&& list, VisitorFunction& fun, bool hasNext = false);

}  // namespace outpost::reflection::detail

#endif /* MODULES_UTILS_SRC_OUTPOST_UTILS_CTLIST_REFLECTION_DETAIL_H */

#include "ctlist_reflection_detail_impl.h"
