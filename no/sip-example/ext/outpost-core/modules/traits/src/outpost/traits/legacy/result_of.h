/*
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_TRAITS_LEGACY_TRAITS_RESULT_OF_H
#define OUTPOST_TRAITS_LEGACY_TRAITS_RESULT_OF_H

/*
 * \brief std::result_of implementation for c++11 that still does SFINAE (c++14)
 * Heavily inspired by https://en.cppreference.com/w/cpp/types/result_of
 */

#if __cplusplus < 201402L

#include <type_traits>
#include <utility>

namespace outpost
{

namespace detail
{
template <class T>
struct is_reference_wrapper : std::false_type
{
};

template <class U>
struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type
{
};

template <class T>
struct invoke_impl
{
    template <class F, class... Args>
    static auto
    call(F&& f, Args&&... args) -> decltype(std::forward<F>(f)(std::forward<Args>(args)...));
};

template <class B, class MT>
struct invoke_impl<MT B::*>
{
    template <class T,
              class Td = typename std::decay<T>::type,
              class = typename std::enable_if<std::is_base_of<B, Td>::value>::type>
    static auto
    get(T&& t) -> T&&;

    template <class T,
              class Td = typename std::decay<T>::type,
              class = typename std::enable_if<is_reference_wrapper<Td>::value>::type>
    static auto
    get(T&& t) -> decltype(t.get());

    template <class T,
              class Td = typename std::decay<T>::type,
              class = typename std::enable_if<!std::is_base_of<B, Td>::value>::type,
              class = typename std::enable_if<!is_reference_wrapper<Td>::value>::type>
    static auto
    get(T&& t) -> decltype(*std::forward<T>(t));

    template <class T,
              class... Args,
              class MT1,
              class = typename std::enable_if<std::is_function<MT1>::value>::type>
    static auto
    call(MT1 B::*pmf, T&& t, Args&&... args)
            -> decltype((invoke_impl::get(std::forward<T>(t)).*pmf)(std::forward<Args>(args)...));

    template <class T>
    static auto
    call(MT B::*pmd, T&& t) -> decltype(invoke_impl::get(std::forward<T>(t)).*pmd);
};

template <class F, class... Args, class Fd = typename std::decay<F>::type>
auto
INVOKE(F&& f, Args&&... args)
        -> decltype(invoke_impl<Fd>::call(std::forward<F>(f), std::forward<Args>(args)...));
}  // namespace detail

// Minimal C++11 implementation:
template <class>
struct result_of;

template <class F, class... ArgTypes>
struct result_of<F(ArgTypes...)>
{
    using type = decltype(detail::INVOKE(std::declval<F>(), std::declval<ArgTypes>()...));
};

}  // namespace outpost

#else
#include <type_traits>

namespace outpost
{
using std::result_of;
}
#endif
#endif  // OUTPOST_TRAITS_LEGACY_TRAITS_RESULT_OF_H
