/*
 * Copyright (c) 2023-2024, Pascal Pieper
 * Copyright (c) 2023, Jan-Gerd Mess
 * Copyright (c) 2023-2024, Pieper, Pascal
 * Copyright (c) 2023-2024, Janosch Reinking
 * Copyright (c) 2023, Kirstein
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_EXPECTED_H
#define OUTPOST_UTILS_EXPECTED_H

#include "unexpected.h"  // EXPECTED_CONSTEXPR_C14 defined here

#include <outpost/base/testing_assert.h>

#include <new>      // for placement-new
#include <utility>  // std::move

namespace outpost
{
/**
 * C++11/14 implementation of std::expected.
 * Heavily inspired by https://en.cppreference.com/w/cpp/utility/expected/expected
 *
 * The class template std::expected provides a way to store either of two values.
 * An object of std::expected at any given time either holds an expected value of type T,
 * or an unexpected value of type E. std::expected is never valueless.
 * The stored value is allocated directly within the storage occupied by the expected object.
 * No dynamic memory allocation takes place. A program is ill-formed
 * if it instantiates an expected with a reference type, a function type, or a
 * specialization of std::unexpected.
 */

template <typename ValueType, typename ErrorType>
class Expected
{
    // pre-c++17 implementation, from https://en.cppreference.com/w/cpp/types/void_t
    template <typename... Ts>
    struct make_void
    {
        typedef void type;
    };

    template <typename... Ts>
    using void_t = typename make_void<Ts...>::type;

    template <class T, template <class...> class Template>
    struct is_specialization : std::false_type
    {
    };

    template <template <class...> class Template, class... Args>
    struct is_specialization<Template<Args...>, Template> : std::true_type
    {
    };

    template <typename T>
    using remove_cvref = typename std::remove_reference<
            typename std::remove_const<typename std::remove_volatile<T>::type>::type>::type;

    template <typename T>
    using is_not_expected_t =
            typename std::enable_if<!is_specialization<remove_cvref<T>, Expected>::value>::type;

    template <typename T>
    using is_default_constructible_t =
            typename std::enable_if<std::is_default_constructible<remove_cvref<T>>::value
                                    == true>::type;

    template <class T, typename = void>
    struct has_bool_operator
    {
        static constexpr bool value = false;
    };

    template <class T>
    struct has_bool_operator<T, void_t<decltype(std::declval<T&>().operator bool())>>
    {
        static constexpr bool value = true;
    };

    template <typename T>
    using is_not_bool_or_boolean_operator_t =
            typename std::enable_if<!std::is_same<T, bool>::value
                                    && !has_bool_operator<T>::value>::type;

public:
    // lowercase-class to stay compatible with std
    using value_t = ValueType;
    using error_t = ErrorType;
    using unexpected_t = Unexpected<ErrorType>;

    // allow access from differently typed Expected
    template <class U, class G>
    friend class Expected;

    // constructors

    // Default constructor not standard, but nice to have
    template <class U = ValueType, class = is_default_constructible_t<U>>
    constexpr Expected() : mHasValue(true), mValue(ValueType{})
    {
    }

    // templated to enable automatic type conversion if available
    template <class U = ValueType, class = is_not_expected_t<U>>
    // cppcheck-suppress noExplicitConstructor
    constexpr Expected(const U& value) : mHasValue(true), mValue(value)
    {
    }

    // templated to enable automatic type conversion if available
    template <class G = ErrorType>
    // cppcheck-suppress noExplicitConstructor
    constexpr Expected(const Unexpected<G>& unexpected) : mHasValue(false), mUnexpected(unexpected)
    {
    }

    CONSTEXPR_C14
    Expected(const Expected& other) noexcept : mHasValue(other.mHasValue)
    {
        if (has_value())
        {
            // This calls ValueType's the assignment operator.
            // Therefore, we need placement new to enforce calling the copy constructor instead,
            // in case the underlying type relies on one of its constructors being called.
            new (&mValue) ValueType(other.mValue);
        }
        else
        {
            new (&mUnexpected) unexpected_t(other.mUnexpected);
        }
    }

    CONSTEXPR_C14
    Expected(Expected&& other) noexcept
    {
        mHasValue = std::move(other.mHasValue);
        if (has_value())
        {
            // This calls ValueType's the assignment operator.
            // Therefore, we need placement new to enforce calling the copy constructor instead,
            // in case the underlying type relies on one of its constructors being called.
            new (&mValue) ValueType(std::move(other.mValue));
        }
        else
        {
            new (&mUnexpected) unexpected_t(std::move(other.mUnexpected));
        }
    }

    template <class U, class G>
    CONSTEXPR_C14 explicit Expected(const Expected<U, G>& other) noexcept :
        mHasValue(other.mHasValue)
    {
        if (has_value())
        {
            // This calls ValueType's the assignment operator.
            // Therefore, we need placement new to enforce calling the copy constructor instead,
            // in case the underlying type relies on one of its constructors being called.
            new (&mValue) ValueType(other.mValue);
        }
        else
        {
            new (&mUnexpected) unexpected_t(other.mUnexpected);
        }
    }

    template <class U, class G>
    CONSTEXPR_C14 explicit Expected(Expected<U, G>&& other) noexcept
    {
        mHasValue = std::move(other.mHasValue);
        if (has_value())
        {
            // This calls ValueType's the assignment operator.
            // Therefore, we need placement new to enforce calling the copy constructor instead,
            // in case the underlying type relies on one of its constructors being called.
            new (&mValue) ValueType(std::move(other.mValue));
        }
        else
        {
            new (&mUnexpected) unexpected_t(std::move(other.mUnexpected));
        }
    }

    ~Expected() noexcept
    {
        if (has_value())
        {
            mValue.~value_t();
        }
        else
        {
            mUnexpected.~unexpected_t();
        }
    }

    // Intentionally no constructor with direct ErrorType.
    // This is to 1) stay within spec, and
    // 2) to prevent mishaps when value_t == error_t (or convertible)

    // assignment operators
    CONSTEXPR_C14 Expected&
    operator=(const Expected& other) noexcept
    {
        if (has_value() != other.has_value())
        {
            this->~Expected();
        }
        mHasValue = other.mHasValue;
        if (has_value())
        {
            mValue = other.mValue;
        }
        else
        {
            mUnexpected = other.mUnexpected;
        }
        return *this;
    }

    CONSTEXPR_C14 Expected&
    operator=(Expected&& other) noexcept
    {
        if (has_value() != other.has_value())
        {
            this->~Expected();
        }
        mHasValue = std::move(other.mHasValue);
        if (has_value())
        {
            mValue = std::move(other.mValue);
        }
        else
        {
            mUnexpected = std::move(other.mUnexpected);
        }
        return *this;
    }

    template <class U, class G>
    CONSTEXPR_C14 Expected&
    operator=(const Expected<U, G>& other) noexcept
    {
        mHasValue = other.mHasValue;
        if (has_value())
        {
            mValue = other.mValue;
        }
        else
        {
            mUnexpected = other.mUnexpected;
        }
        return *this;
    }

    template <class U = ValueType, class = is_not_expected_t<U>>
    CONSTEXPR_C14 Expected&
    operator=(U&& v) noexcept
    {
        if (!has_value())
        {
            this->~Expected();
            mHasValue = true;
        }
        mValue = std::forward<U>(v);
        return *this;
    }

    template <class G>
    CONSTEXPR_C14 Expected&
    operator=(const Unexpected<G>& other)
    {
        if (has_value())
        {
            this->~Expected();
            mHasValue = false;
        }
        mUnexpected = other;
        return *this;
    }

    template <class G>
    CONSTEXPR_C14 Expected&
    operator=(Unexpected<G>&& other)
    {
        if (has_value())
        {
            this->~Expected();
            mHasValue = false;
        }
        mUnexpected = std::forward<Unexpected<G>>(other);
        return *this;
    }

    // observers
    constexpr const ValueType*
    operator->() const noexcept
    {
        return &value();
    }

    CONSTEXPR_C14 ValueType*
    operator->() noexcept
    {
        return &value();
    }

    constexpr const ValueType&
    operator*() const& noexcept
    {
        return value();
    }

    CONSTEXPR_C14 ValueType&
    operator*() & noexcept
    {
        return value();
    }

    constexpr const ValueType&&
    operator*() const&& noexcept
    {
        return value();
    }

    CONSTEXPR_C14 ValueType&&
    operator*() && noexcept
    {
        return std::move(value());
    }

    constexpr bool
    has_value() const noexcept
    {
        return mHasValue;
    }

    /**
     * Convenience function alias to \c has_value() .
     * Disabled for \c bool or \c operator bool() ValueTypes for safety reasons.
     */
    template <class U = ValueType, class = is_not_bool_or_boolean_operator_t<U>>
    constexpr explicit
    operator bool() const noexcept
    {
        return has_value();
    }

    CONSTEXPR_C14 ErrorType&
    error() &
    {
        OUTPOST_ASSERT(!mHasValue, "[Expected] Invalid access to error value");
        return mUnexpected.error();
    }

    constexpr const ErrorType&
    error() const&
    {
        OUTPOST_CONSTEXPR_ASSERT(!mHasValue, "[Expected] Invalid access to error value");
        return mUnexpected.error();
    }

    CONSTEXPR_C14 ErrorType&&
    error() &&
    {
        // cppcheck-suppress returnStdMoveLocal
        return std::move(error());
    }

    constexpr const ErrorType&&
    error() const&&
    {
        // cppcheck-suppress returnStdMoveLocal
        return std::move(error());
    }

    template <class U>
    constexpr ValueType
    value_or(U&& otherValue) const&
    {
        return (mHasValue) ? mValue : std::forward<U>(otherValue);
    }

    template <class U>
    CONSTEXPR_C14 ValueType
    value_or(U&& otherValue) &&
    {
        return (mHasValue) ? mValue : std::forward<U>(otherValue);
    }

private:
    /* value() observers were decided to be private
     * because accessing the value with the pointer observers
     * adds more feeling of a pointer in codestyle.
     * This pushes library-users to be more careful with
     * dereferencing the values.
     */
    CONSTEXPR_C14 ValueType&
    value() &
    {
        OUTPOST_ASSERT(mHasValue, "[Expected] Invalid access to expected value");
        return mValue;
    }

    CONSTEXPR_C14 const ValueType&
    value() const&
    {
        OUTPOST_ASSERT(mHasValue, "[Expected] Invalid access to expected value");
        return mValue;
    }

    CONSTEXPR_C14 ValueType&&
    value() &&
    {
        // cppcheck-suppress returnStdMoveLocal
        return std::move(value());
    }

    CONSTEXPR_C14 const ValueType&&
    value() const&&
    {
        // cppcheck-suppress returnStdMoveLocal
        return std::move(value());
    }

private:
    bool mHasValue;

    union
    {
        ValueType mValue;
        unexpected_t mUnexpected;
    };
};

}  // namespace outpost

#endif  // OUTPOST_UTILS_EXPECTED_H
