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

#ifndef OUTPOST_UTILS_CTLIST_REFLECTION_H
#define OUTPOST_UTILS_CTLIST_REFLECTION_H

#include "ctlist_reflection_detail.h"

namespace outpost::reflection
{

/**
 * @brief Startpoint for visiting nested nodes with `getMember()` and
 *        applying the functor on all referenced (non-list) elements.
 *
 * This is the Visitor lvalue-version (reference).
 */
template <typename ReflectionElement, typename VisitorFunction>
constexpr std::enable_if_t<detail::implements_any_access_v<ReflectionElement>>
visit(ReflectionElement& element, VisitorFunction& fun)
{
    detail::visit(detail::getMaybeConstMember(element), fun);
}

/**
 * @brief Startpoint for visiting nested nodes with `getMember()` and
 *        applying the functor on all referenced (non-list) elements.
 *
 * This is the rvalue-version which holds its own Visitor.
 * example:
 * ```c++
 * visit(abc, IncrementVisitor{});
 * ```
 */
template <typename ReflectionElement, typename VisitorFunction>
constexpr std::enable_if_t<detail::implements_any_access_v<ReflectionElement>>
visit(ReflectionElement& element, VisitorFunction&& fun)
{
    detail::visit(detail::getMaybeConstMember(element), fun);
}

}  // namespace outpost::reflection

#endif /* MODULES_UTILS_SRC_OUTPOST_UTILS_CTLIST_REFLECTION_H */
