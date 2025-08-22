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

#ifndef OUTPOST_UTILS_SAFE_MASK_H
#define OUTPOST_UTILS_SAFE_MASK_H

#include <inttypes.h>

namespace outpost
{

/**
 * Typesafe class that defines operators on otherwise POD types like `int`.
 * It is typesafe in the way that derived classes are strongly typed different.
 * Example:
 * ```
 * class A : public Mask<A>
 * {
 * public:
 *  constexpr A(const Mask<A>& mask) : Mask<A>(mask) {};
 * private:
 *  using Mask<A>::Mask;
 * };
 * class B : public Mask<B>
 * {
 * public:
 *      constexpr B(const Mask<B>& mask) : Mask<B>(mask) {};
 * private:
 *  using Mask<B>::Mask;
 * };
 *
 * auto a = A::Thing() | A::OtherThing() // works
 * auto b = A::Thing() | B::ThirdThing() // does not compile
 * ```
 *
 * \param Derived is the derived class that inherits from Mask
 * \param ValueType is the underlying value type.
 *
 * It uses (and needs) the constructor of the derived class
 * with the Parameter `Mask<A>`.
 */
template <typename Derived, typename ValueType = uint8_t>
class Mask
{
public:
    using Type = ValueType;

    constexpr Mask() = default;

    explicit constexpr Mask(const Mask& other) = default;

    constexpr Mask&
    operator=(const Mask&) = default;

    /**
     * Test whether the Mask set has a certain bit enabled.
     *
     * Short for `static_cast<bool>(mask & X)`
     */
    constexpr bool
    isSet(const Mask& o) const
    {
        return (value & o.value) == o.value;
    }

    constexpr bool
    operator==(const Mask& o) const
    {
        return value == o.value;
    }

    constexpr bool
    operator!=(const Mask& o) const
    {
        return value != o.value;
    }

    constexpr Derived
    operator&(const Mask& o) const
    {
        // `value & o.value` might has type of `int` which might cause a
        // warning when it is assigned back to a smaller type in the
        // constructor of `Mask`. This can happen if the type of the operants
        // is smaller than `int`. Since this is a binary operation it should be
        // safe to cast it back into the type of the operants.
        return Mask{static_cast<Type>(value & o.value)};
    }

    constexpr Derived
    operator|(const Mask& o) const
    {
        // `value | o.value` might has type of `int` which might cause a
        // warning when it is assigned back to a smaller type in the
        // constructor of `Mask`. This can happen if the type of the operants
        // is smaller than `int`. Since this is a binary operation it should be
        // safe to cast it back into the type of the operants.
        return Mask{static_cast<Type>(value | o.value)};
    }

protected:
    /**
     * Constructor for explicit values, to be used by derived class
     */
    constexpr explicit Mask(const ValueType& v) : value{v} {};

private:
    ValueType value = 0;
};

}  // namespace outpost

#endif  // OUTPOST_UTILS_SAFE_MASK_H
