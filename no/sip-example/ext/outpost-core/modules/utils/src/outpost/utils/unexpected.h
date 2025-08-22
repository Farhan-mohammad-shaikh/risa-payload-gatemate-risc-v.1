/*
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2023, Jan-Gerd Mess
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_UNEXPECTED_H
#define OUTPOST_UTILS_UNEXPECTED_H

#if __cplusplus < 201402L || defined(EXPECTED_TEST_C11)
#define CONSTEXPR_C14  // nothing
#else
#define CONSTEXPR_C14 constexpr
#endif

#include <utility>  // std::move

namespace outpost
{

template <class ErrorType>
class Unexpected
{
    template <typename T>
    using remove_cr = typename std::remove_reference<typename std::remove_const<T>::type>::type;

    template <typename T>
    using not_unexpected_t =
            typename std::enable_if<!std::is_same<Unexpected, remove_cr<T>>::value>::type;

public:
    // allow access from differently typed Unexpected
    template <class E>
    friend class Unexpected;

    // constructors
    constexpr Unexpected(const Unexpected&) = default;
    constexpr Unexpected(Unexpected&&) = default;

    template <class E = ErrorType>
    // cppcheck-suppress noExplicitConstructor
    constexpr Unexpected(const Unexpected<E>& other) : mUnexpected(other.mUnexpected)
    {
    }

    template <class E = ErrorType>
    // cppcheck-suppress noExplicitConstructor
    constexpr Unexpected(Unexpected<E>&& other) : mUnexpected(std::move(other.mUnexpected))
    {
    }

    ~Unexpected() = default;

    template <class E = ErrorType, class = not_unexpected_t<E>>
    constexpr explicit Unexpected(E&& unexpectedValue) : mUnexpected(unexpectedValue)
    {
    }

    // assignment
    CONSTEXPR_C14 Unexpected&
    operator=(const Unexpected&) = default;
    CONSTEXPR_C14 Unexpected&
    operator=(Unexpected&&) = default;

    template <class E = ErrorType, class = not_unexpected_t<E>>
    CONSTEXPR_C14 Unexpected&
    operator=(const Unexpected<E>& other)
    {
        mUnexpected = other.mUnexpected;
        return *this;
    }

    template <class E = ErrorType, class = not_unexpected_t<E>>
    CONSTEXPR_C14 Unexpected&
    operator=(Unexpected<E>&& other)
    {
        mUnexpected = std::forward<E>(other.mUnexpected);
        return *this;
    }

    // observer
    constexpr const ErrorType&
    error() const& noexcept
    {
        return mUnexpected;
    }

    CONSTEXPR_C14 ErrorType&
    error() & noexcept
    {
        return mUnexpected;
    }

    constexpr const ErrorType&&
    error() const&& noexcept
    {
        // cppcheck-suppress returnStdMoveLocal
        return std::move(error());
    }

    CONSTEXPR_C14 ErrorType&&
    error() && noexcept
    {
        // cppcheck-suppress returnStdMoveLocal
        return std::move(error());
    }

    // swap
    CONSTEXPR_C14 void
    swap(Unexpected& other) noexcept
    {
        std::swap(mUnexpected, other.mUnexpected);
    }

    friend CONSTEXPR_C14 void
    swap(Unexpected& x, Unexpected& y) noexcept(noexcept(x.swap(y)))
    {
        x.swap(y);
    }

    // equality operator
    template <class E2>
    friend constexpr bool
    operator==(const Unexpected& a, const Unexpected<E2>& b)
    {
        return a.mUnexpected == b.mUnexpected;
    }

private:
    ErrorType mUnexpected;
};

// "Outside" Constructor for automatic template deduction
template <class E>
Unexpected<E>
unexpected(const E& unexpectedValue)
{
    return Unexpected<E>(unexpectedValue);
}

}  // namespace outpost

#endif  // OUTPOST_UTILS_UNEXPECTED_H
