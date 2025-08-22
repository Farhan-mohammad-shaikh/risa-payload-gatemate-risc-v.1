/*
 * Copyright (c) 2023-2024, Janosch Reinking
 * Copyright (c) 2024, Pascal Pieper
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_HAL_GPIO_VIEW_H
#define OUTPOST_HAL_GPIO_VIEW_H

#include "outpost/base/testing_assert.h"
#include "outpost/hal/gpio/index.h"
#include "outpost/hal/gpio/input_pin.h"
#include "outpost/hal/gpio/output_pin.h"
#include "outpost/utils/expected.h"
#include "outpost/utils/log2.h"

#include <array>

namespace outpost
{
namespace hal
{

// ---------------------------------------------------------------------------
/**
 * \class N2UintType
 *
 * Mapping from group to uint*_t type. A group descripts all numbers of pins
 * that can be represented by a certain uint*_t type. E.g.: 5 and 7 pins can
 * be represented by an uint8_t. There should be both mapped to group 0U.
 *
 * \tparam N
 *      group
 */
template <size_t N>
struct N2UintType;

template <>
struct N2UintType<0U>
{
    using Type = uint8_t;
};

template <>
struct N2UintType<1U>
{
    using Type = uint16_t;
};

template <>
struct N2UintType<2U>
{
    using Type = uint32_t;
};

template <>
struct N2UintType<3U>
{
    using Type = uint64_t;
};

// ---------------------------------------------------------------------------
/**
 * \brief numberofBytes2Index
 * \param bytes
 *      the number of bytes
 */
static constexpr size_t
numberofBytes2Index(size_t bytes)
{
    return (bytes == 1U) ? (0U) : (outpost::Log2(bytes - 1U) + 1U);
}

// ---------------------------------------------------------------------------
/**
 * \class numberOfBits2Index
 *
 * Mapping from number of pins to group
 */
static constexpr size_t
numberOfBits2Index(size_t bits)
{
    return numberofBytes2Index((bits + 7) / 8);
}

// ---------------------------------------------------------------------------
/**
 * \class BankTypeOf
 *
 * Type to represent N number of bits.
 *
 * \tparam N
 *      number of bits
 */
template <size_t N>
using BankTypeOf = typename N2UintType<numberOfBits2Index(N)>::Type;

// ---------------------------------------------------------------------------
/**
 * \class GpiView
 *
 * Reduce the port to \c N number of pins. This class can be used as interface
 * to reduce the dependency to a concrete port. This class provides methods
 * to read all pins of the view. It is also possible to get a single pin and
 * perform operations on it. Per default the \c read() function is not atomic,
 * but it can be overwritten in a project-specific class to implement an atomic
 * \c read() method.
 *
 * \tparam N
 *      number of pins for this view
 */
template <size_t N>
class GpiView
{
public:
    static_assert(N > 0U && N <= 64U, "Number of bits must fit into uint64_t!");

    using BankType = BankTypeOf<N>;

    template <typename... Pins>
    explicit GpiView(Pins&... pins);

    GpiView(const GpiView&) = delete;

    GpiView&
    operator=(const GpiView&) = delete;

    virtual ~GpiView() = default;

    /**
     * Access the pin at the given \c index
     *
     * \param index
     *      the index of the pin to retrieve. The index must be in range.
     *
     * \return the pin at the given index
     */
    InputPin&
    getPin(size_t index);

    /**
     * Access the pin at the given index \c I
     *
     * \param index
     *      the index of the pin to retrieve. The index will be checked at
     *      compile time
     *
     * \return the pin at the given index
     */
    template <IndexType I>
    InputPin&
    getPin();

    /**
     * Read the view. Only the first \c N bits of the return value are used.
     *
     * \return the value of the pins
     */
    virtual Expected<BankType, ReturnCode>
    read() const;

protected:
    std::array<InputPin*, N> mPins;
};

// ---------------------------------------------------------------------------
template <size_t N>
template <typename... Pins>
GpiView<N>::GpiView(Pins&... pins) : mPins{&pins...}
{
    static_assert(N == sizeof...(Pins), "Number of pins must match the width of this view!");
}

template <size_t N>
InputPin&
GpiView<N>::getPin(size_t index)
{
    OUTPOST_ASSERT(index < N, "Out of bounds!");
    // cannot be nullptr since the constructor expects exact N pins
    // but to be sure:
    OUTPOST_ASSERT(mPins[index] != nullptr, "Pin is not set!");
    return *mPins[index];
}

template <size_t N>
template <IndexType I>
InputPin&
GpiView<N>::getPin()
{
    static_assert(I < N, "Out of bounds!");
    OUTPOST_ASSERT(mPins[I] != nullptr, "Pin is not set!");
    return *mPins[I];
}

template <size_t N>
Expected<typename GpiView<N>::BankType, ReturnCode>
GpiView<N>::read() const
{
    ReturnCode code = ReturnCode::success;
    BankType tmp = 0U;
    for (size_t index = 0U; index < N; index++)
    {
        if (mPins[index] != nullptr)
        {
            const auto maybeValue = mPins[index]->read();
            if (maybeValue.has_value())
            {
                tmp |= (*maybeValue << index);
            }
            else
            {
                code = maybeValue.error();
            }
        }
    }

    if (code == ReturnCode::success)
    {
        return tmp;
    }
    else
    {
        return unexpected(code);
    }
}

// ---------------------------------------------------------------------------
/**
 * \class SimpleAtomicGpiView
 *
 * \tparam N
 *      number of pins for this view
 */
template <typename BankType, size_t N>
class SimpleAtomicGpiView : public GpiView<N>
{
public:
    using InnerBankType = typename GpiView<N>::BankType;
    using OuterBankType = BankType;

    static_assert(sizeof(OuterBankType) >= sizeof(InnerBankType),
                  "The view must be smaller or equal to the port!");

    template <typename... Pins>
    explicit SimpleAtomicGpiView(Gpi<BankType>& gpi, Pins&... pins);

    SimpleAtomicGpiView(const SimpleAtomicGpiView&) = delete;

    SimpleAtomicGpiView&
    operator=(const SimpleAtomicGpiView&) = delete;

    virtual ~SimpleAtomicGpiView() = default;

    /**
     * Read all pins.
     *
     * Read the view. Only the first \c N bits are used.
     *
     * \return the value of the pins
     */
    Expected<InnerBankType, ReturnCode>
    read() const override;

private:
    // inline might remove const-limited recursion
    template <typename First, typename... Rest>
    inline void
    update(size_t index, First& pin, Rest&... rest);

    inline void
    update(size_t index, SimpleInputPin<OuterBankType>& pin);

    outpost::hal::Gpi<OuterBankType>& mGpi;
    OuterBankType mMask;

    // uint8_t is sufficient since only the index is stored which is has a
    // maximum of 64.
    std::array<uint8_t, sizeof(OuterBankType) * 8U> mMapping;
};

// ---------------------------------------------------------------------------
template <typename BankType, size_t N>
template <typename... Pins>
SimpleAtomicGpiView<BankType, N>::SimpleAtomicGpiView(Gpi<BankType>& gpi, Pins&... pins) :
    GpiView<N>(pins...), mGpi(gpi), mMask(0U)
{
    std::fill(mMapping.begin(), mMapping.end(), 0U);
    update(0, pins...);
}

template <typename BankType, size_t N>
Expected<typename SimpleAtomicGpiView<BankType, N>::InnerBankType, ReturnCode>
SimpleAtomicGpiView<BankType, N>::read() const
{
    auto origValue = mGpi.get(mMask);
    /// just to make sure that no pins are set which are not in this view.
    /// this allows to initialize \c mMapping[i] with a default value of \c 0U
    /// instead of excluding them explicit.

    origValue &= mMask;

    InnerBankType mappedValue = 0U;
    for (size_t index = 0; index < sizeof(OuterBankType) * 8U; index++)
    {
        mappedValue |= ((origValue >> index) & 1U) << mMapping[index];
    }

    return mappedValue;
}

template <typename BankType, size_t N>
template <typename First, typename... Rest>
void
SimpleAtomicGpiView<BankType, N>::update(size_t index, First& pin, Rest&... rest)
{
    update(index, pin);
    update(index + 1, rest...);
}

template <typename BankType, size_t N>
void
SimpleAtomicGpiView<BankType, N>::update(size_t index, SimpleInputPin<OuterBankType>& pin)
{
    mMask |= pin.getMask();
    mMapping[outpost::Log2(pin.getMask())] = index;
}

// ---------------------------------------------------------------------------
/**
 * \class GpoView
 *
 * Reduce the port to \c N number of pins. This class can be used as interface
 * to reduce the dependency to a concrete port. This class provides methods
 * to write all pins of the view. It is also possible to get a single pin and
 * perform operation on it. Per default the \c \set() \c clear()  and
 * \c write() function is not atomic. But they can be overwritten in a
 * project-specific class to implement atomic operations.
 *
 * \tparam N
 *      number of pins for this view
 */
template <size_t N>
class GpoView
{
public:
    static_assert(N > 0U && N <= 64U, "Number of bits must fit into uint64_t!");

    using BankType = BankTypeOf<N>;

    template <typename... Pins>
    explicit GpoView(Pins&... pins);

    GpoView(const GpoView&) = delete;

    GpoView&
    operator=(const GpoView&) = delete;

    virtual ~GpoView() = default;

    /**
     * Get the pin at the given \c index
     *
     * \param index
     *      the index of the pin to retrieve. The index must be in range.
     *
     * \return the pin at the given index
     */
    OutputPin&
    getPin(size_t index);

    /**
     * Get the pin at the given index \c I.
     *
     * \param index
     *      the index of the pin to retrieve. The index will be checked at
     *      compile time.
     *
     * \return the pin at the given index
     */
    template <IndexType I>
    OutputPin&
    getPin();

    /**
     * Write the pins
     *
     * \param value
     *      the value to write
     *
     * \return
     *      \c ReturnCode::success on success, an implementation dependend
     *      error code on failure. Some pins might be written and some not
     *      since this operation might apply the write an each pin
     *      individual.
     */
    virtual ReturnCode
    write(BankType value);

    /**
     * Set all pins
     *
     * \return
     *      \c ReturnCode::success on success, an implementation dependend
     *      error code on failure. Some pins might be written and some not
     *      since this operation might apply the write an each pin
     *      individual.
     */
    virtual ReturnCode
    set();

    /**
     * Toggle all pins
     *
     * \return
     *      \c ReturnCode::success on success, an implementation dependend
     *      error code on failure. Some pins might be written and some not
     *      since this operation might apply the write an each pin
     *      individual.
     */
    virtual ReturnCode
    toggle();

    /**
     * Clear all pins
     *
     * \return
     *      \c ReturnCode::success on success, an implementation dependend
     *      error code on failure. Some pins might be written and some not
     *      since this operation might apply the write an each pin
     *      individual.
     */
    virtual ReturnCode
    clear();

protected:
    std::array<OutputPin*, N> mPins;
};

// ---------------------------------------------------------------------------
template <size_t N>
template <typename... Pins>
GpoView<N>::GpoView(Pins&... pins) : mPins{&pins...}
{
    static_assert(N == sizeof...(Pins), "Number of pins must match the width of this view!");
}

template <size_t N>
OutputPin&
GpoView<N>::getPin(size_t index)
{
    OUTPOST_ASSERT(index < N, "Out of bound!");
    // cannot be nullptr since the constructor expects exact N pins
    // but to be sure:
    OUTPOST_ASSERT(mPins[index] != nullptr, "Pin is not set!");
    return *mPins[index];
}

template <size_t N>
template <IndexType I>
OutputPin&
GpoView<N>::getPin()
{
    static_assert(I < N, "Out of bound!");
    OUTPOST_ASSERT(mPins[I] != nullptr, "Pin is not set!");
    return *mPins[I];
}

template <size_t N>
ReturnCode
GpoView<N>::write(BankType value)
{
    ReturnCode code = ReturnCode::success;
    for (size_t index = 0U; index < N; index++)
    {
        if (mPins[index] != nullptr)
        {
            ReturnCode tmp;
            if (value & (1U << index))
            {
                tmp = mPins[index]->set();
            }
            else
            {
                tmp = mPins[index]->clear();
            }

            if (tmp != ReturnCode::success)
            {
                code = tmp;
            }
        }
    }
    return code;
}

template <size_t N>
ReturnCode
GpoView<N>::set()
{
    ReturnCode code = ReturnCode::success;
    for (size_t index = 0U; index < N; index++)
    {
        ReturnCode tmp = ReturnCode::success;
        if (mPins[index] != nullptr)
        {
            tmp = mPins[index]->set();
        }

        if (tmp != ReturnCode::success)
        {
            code = tmp;
        }
    }
    return code;
}

template <size_t N>
ReturnCode
GpoView<N>::toggle()
{
    ReturnCode code = ReturnCode::success;
    for (size_t index = 0U; index < N; index++)
    {
        ReturnCode tmp = ReturnCode::success;
        if (mPins[index] != nullptr)
        {
            tmp = mPins[index]->toggle();
        }

        if (tmp != ReturnCode::success)
        {
            code = tmp;
        }
    }
    return code;
}

template <size_t N>
ReturnCode
GpoView<N>::clear()
{
    ReturnCode code = ReturnCode::success;
    for (size_t index = 0U; index < N; index++)
    {
        ReturnCode tmp = ReturnCode::success;
        if (mPins[index] != nullptr)
        {
            tmp = mPins[index]->clear();
        }
        if (tmp != ReturnCode::success)
        {
            code = tmp;
        }
    }
    return code;
}

// ---------------------------------------------------------------------------
/**
 * \class SimpleAtomicGpoView
 *
 * \tparam BankType
 *      BankType of the gpo interface
 * \tparam N
 *      number of pins for this view
 */
template <typename BankType, size_t N>
class SimpleAtomicGpoView : public GpoView<N>
{
public:
    using OuterBankType = BankType;
    using InnerBankType = typename GpoView<N>::BankType;

    static_assert(sizeof(OuterBankType) >= sizeof(InnerBankType),
                  "The view must be smaller or equal to the port!");

    template <typename... Pins>
    explicit SimpleAtomicGpoView(Gpo<OuterBankType>& gpo, Pins&... pins);

    SimpleAtomicGpoView(const SimpleAtomicGpoView&) = delete;

    SimpleAtomicGpoView&
    operator=(const SimpleAtomicGpoView&) = delete;

    virtual ~SimpleAtomicGpoView() = default;

    /**
     * \param value
     *      the value to write
     */
    ReturnCode
    write(InnerBankType value) override;

    /**
     * set all pins of the view
     *
     * \return
     *      \c ReturnCode::success on success, an implementation dependend
     *      error code in the case that \c set() failed.
     */
    ReturnCode
    set() override;

    /**
     * toggle all pins of the view
     *
     * \return
     *      \c ReturnCode::success on success, an implementation dependend
     *      error code in the case that \c toggle() failed.
     */
    ReturnCode
    toggle() override;

    /**
     * clear all pins of the view
     *
     * \return
     *      \c ReturnCode::success on success, an implementation dependend
     *      error code in the case that \c clear() failed.
     */
    ReturnCode
    clear() override;

    inline OuterBankType
    getMask() const;

private:
    // inline might remove const-limited recursion
    template <typename First, typename... Rest>
    inline void
    update(size_t index, First& pin, Rest&... rest);

    template <typename First>
    inline void
    update(size_t index, First& pin);

    Gpo<OuterBankType>& mGpo;
    OuterBankType mMask;
    std::array<size_t, N> mMapping;
};

// ---------------------------------------------------------------------------
template <typename BankType, size_t N>
template <typename... Pins>
SimpleAtomicGpoView<BankType, N>::SimpleAtomicGpoView(Gpo<OuterBankType>& gpo, Pins&... pins) :
    outpost::hal::GpoView<N>(pins...), mGpo(gpo), mMask(0U)
{
    std::fill(mMapping.begin(), mMapping.end(), 0U);
    update(0, pins...);
}

template <typename BankType, size_t N>
ReturnCode
SimpleAtomicGpoView<BankType, N>::write(InnerBankType value)
{
    OuterBankType mappedValue = 0U;
    for (size_t index = 0; index < N; index++)
    {
        mappedValue |= (((value >> index) & 1U) << mMapping[index]);
    }
    return mGpo.write(mappedValue, mMask);
}

template <typename BankType, size_t N>
ReturnCode
SimpleAtomicGpoView<BankType, N>::set()
{
    return mGpo.set(mMask);
}

template <typename BankType, size_t N>
ReturnCode
SimpleAtomicGpoView<BankType, N>::toggle()
{
    return mGpo.toggle(mMask);
}

template <typename BankType, size_t N>
ReturnCode
SimpleAtomicGpoView<BankType, N>::clear()
{
    return mGpo.clear(mMask);
}

template <typename BankType, size_t N>
BankType
SimpleAtomicGpoView<BankType, N>::getMask() const
{
    return mMask;
}

template <typename BankType, size_t N>
template <typename First, typename... Rest>
void
SimpleAtomicGpoView<BankType, N>::update(size_t index, First& pin, Rest&... rest)
{
    update(index, pin);
    update(index + 1, rest...);
}

template <typename BankType, size_t N>
template <typename First>
void
SimpleAtomicGpoView<BankType, N>::update(size_t index, First& pin)
{
    mMask |= pin.getMask();
    mMapping[index] = outpost::Log2(pin.getMask());
}

}  // namespace hal
}  // namespace outpost

#endif  // OUTPOST_HAL_GPIO_VIEW_H
