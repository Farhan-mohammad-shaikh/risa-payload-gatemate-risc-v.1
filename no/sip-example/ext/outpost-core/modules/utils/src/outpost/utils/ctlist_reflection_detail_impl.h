/*
 * Copyright (c) 2024, Pascal Pieper
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_CTLIST_REFLECTION_DETAIL_IMPL_H
#define OUTPOST_UTILS_CTLIST_REFLECTION_DETAIL_IMPL_H
#define MODULES_UTILS_SRC_OUTPOST_UTILS_CTLIST_REFLECTION_DETAIL_IMPL_H

// Only for indexer. Should be called by the following file anyways.
#include "ctlist_reflection_detail.h"

#include <utility>

// #include <iostream> // debug only

namespace outpost::reflection::detail
{

// ----------------------------------------------------------------------------
// Way of circumventing const / non-const access to only one function
// ----------------------------------------------------------------------------

/**
 * @brief Gets the constant list of a const type
 *        that has the const function defined (easy case)
 * @return \c CompileTimeList<...> with immutable types
 */
template <typename E, typename>
constexpr auto
getConstList(const E& element)
{
    return element.getMember();
}

/**
 * @brief Gets the constant list of a type
 *        that *has not* the const function defined, but
 *        at least has the function non-const defined.
 * @tparam E the element to access `getMember()` of
 * @return \c CompileTimeList<...> with immutable types
 */
template <typename E>
constexpr auto
getConstList(const E& element)
{
    // const cast is OK as long as non-const `getMember()` will
    // *never* do more than to return the ReferenceList.
    return const_cast<std::remove_const_t<E>*>(&element)->getMember().asConstList();
}

/**
 * @brief Dispatches call to \c s.getMember() either const or non-const
 * @return A \c CompileTimeList<...> with either mutable or un-mutable types
 */
template <typename T>
constexpr auto
getMaybeConstMember(T& s)
{
    if constexpr (std::is_const_v<T>)
    {
        // `s` is constant, so try and get CompileTimeList with immutable elements.
        // delay the decision which access function to use.
        return getConstList(s);
    }
    else
    {
        // the easy way out.
        // `s` has (const || non-const) function defined,
        // and compiler will prefer non-const.
        return s.getMember();
    }
}

// ----------------------------------------------------------------------------
// "Tree" iteration (depth first) functions for Node or Leaf
// ----------------------------------------------------------------------------

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
visitLeaf(Element& node, VisitorFunction& fun, bool)
{
    // std::cout << "VisitLeaf without hasNext" << std::endl;
    fun(node);
}

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
visitLeaf(Element& node, VisitorFunction& fun, bool hasNext)
{
    // std::cout << "VisitLeaf with hasNext: " << hasNext << std::endl;
    fun(node, hasNext);
}

/**
 * @brief Generic visitor for Nodes with descendants (branches).
 *
 * @tparam Element an object with `getMember()`, thus a branch.
 * @tparam VisitorFunction any Functor
 * @param hasNext Info whether there are more elements after that.
 */
template <typename Element, typename VisitorFunction>
constexpr std::enable_if_t<implements_any_access_v<Element>>
descent(Element& nodeWithList, VisitorFunction& fun, bool hasNext)
{
    // std::cout << "Decending into " << demangled(typeid(Element).name()) << std::endl;
    visit(getMaybeConstMember(nodeWithList), fun, hasNext);
}

/**
 * @brief Generic visitor for Nodes without descendants (leafs).
 *
 * @tparam Element an object without `getMember()`, thus a leaf.
 * @tparam VisitorFunction any Functor
 * @param hasNext Info whether there are more elements after that.
 */
template <typename Element, typename VisitorFunction>
constexpr std::enable_if_t<!implements_any_access_v<Element>>
descent(Element& node, VisitorFunction& fun, bool hasNext)
{
    // std::cout << demangled(typeid(Element).name()) << " is leaf with";
    // if (!hasNext)
    //     std::cout << "out";
    // std::cout << " next Elements" << std::endl;
    visitLeaf(node, fun, hasNext);
}

// ----------------------------------------------------------------------------
// List iterating functions (for determining the child nodes)
// ----------------------------------------------------------------------------

/**
 * @brief Recursion-stop-function of list iteration.
 *        Version where VisitorFunctor does not implement "stop" (void)
 */
template <typename VisitorFunction>
constexpr std::enable_if_t<!visitor_accepts_void_v<VisitorFunction>>
visit(EmptyList, VisitorFunction& /*fun*/, bool /*hasNext*/)
{
    // std::cout << "End of List" << std::endl;
}

/**
 * @brief Recursion-stop-function of list iteration.
 *        Version where VisitorFunctor *does* implement "stop" (void)
 */
template <typename VisitorFunction>
constexpr std::enable_if_t<visitor_accepts_void_v<VisitorFunction>>
visit(EmptyList, VisitorFunction& fun, bool hasNext)
{
    // std::cout << "End of List with fun(void)" << std::endl;
    if (!hasNext)
        fun();
}

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
          class List,
          typename VisitorFunction>
constexpr void
visit(List<HeadElement, NextElements...>& list, VisitorFunction& fun, bool hasNext)
{
    HeadElement& element = list.get();  // Reference to const or non-const HeadElement
    const bool hasAnyBranchNext = hasNext || list.hasNext();

    // decides whether to descent into (nested) list,
    // or whether to finally apply function (with "leaf")
    descent(element, fun, hasAnyBranchNext);

    visit(list.next(), fun, hasNext);
}

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
          class List,
          typename VisitorFunction>
constexpr void
visit(List<HeadElement, NextElements...>&& list, VisitorFunction& fun, bool hasNext)
{
    HeadElement& element = list.get();  // Reference to const or non-const HeadElement
    const bool hasAnyBranchNext = hasNext || list.hasNext();

    // decides whether to descent into (nested) list,
    // or whether to finally apply function (with "leaf")
    descent(element, fun, hasAnyBranchNext);

    visit(list.next(), fun, hasNext);
}

}  // namespace outpost::reflection::detail

#endif /* MODULES_UTILS_SRC_OUTPOST_UTILS_CTLIST_REFLECTION_DETAIL_IMPL_H */
