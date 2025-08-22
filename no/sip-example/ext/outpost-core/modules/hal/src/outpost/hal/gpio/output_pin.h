/*
 * Copyright (c) 2023-2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_HAL_GPIO_OUTPUT_PIN_H
#define OUTPOST_HAL_GPIO_OUTPUT_PIN_H

#include "outpost/base/testing_assert.h"
#include "outpost/hal/gpio.h"
#include "outpost/hal/gpio/index.h"

namespace outpost
{
namespace hal
{

// ---------------------------------------------------------------------------
/**
 * \class OutputPin
 *
 * This class is an abstraction of output pins. A reference to an \c OutputPin
 * should be used if a module needs to write some pins. \c OutpinImpl is a
 * concrete implementation which is based on \c outpost::hal::Gpo<BankType>.
 * But also project-specific implementation are possible.
 */
class OutputPin
{
public:
    explicit OutputPin() = default;

    OutputPin(const OutputPin&) = delete;

    OutputPin&
    operator=(const OutputPin&) = delete;

    virtual ~OutputPin() = default;

    /**
     * Toggle the pin
     *
     * \return
     *      \c ReturnCode::success, on success and an error code in case of an
     *      error.
     */
    virtual ReturnCode
    toggle() = 0;

    /**
     * Set the pin
     *
     * \return
     *      \c ReturnCode::success, on success and an error code in case of an
     *      error.
     */
    virtual ReturnCode
    set() = 0;

    /**
     * Clear the pin
     *
     * \return
     *      \c ReturnCode::success, on success and an error code in case of an
     *      error.
     */
    virtual ReturnCode
    clear() = 0;

private:
};

// ---------------------------------------------------------------------------
/**
 * \class SimpleOutputPin
 */
template <typename BankType>
class SimpleOutputPin : public OutputPin
{
public:
    /**
     * \param port
     *      the port of the pin
     * \param index
     *      index of the pin
     */
    explicit SimpleOutputPin(Gpo<BankType>& port, BankType index);

    /**
     * This constructor should be prefered over the one above since it performs
     * static checks.
     *
     * \param port
     *      the port of the pin
     * \param pin
     *      the index of the pin
     *
     * \tparam I
     *      the index of the pin
     */
    template <IndexType I>
    explicit SimpleOutputPin(Gpo<BankType>& port, Index<I> pin);

    SimpleOutputPin(const SimpleOutputPin&) = delete;

    SimpleOutputPin&
    operator=(const SimpleOutputPin&) = delete;

    virtual ~SimpleOutputPin() = default;

    ReturnCode
    toggle() override;

    ReturnCode
    set() override;

    ReturnCode
    clear() override;

    BankType
    getMask() const;

    const Gpo<BankType>&
    getPort() const;

private:
    BankType mPin;
    Gpo<BankType>& mPort;
};

// ---------------------------------------------------------------------------
template <typename BankType>
SimpleOutputPin<BankType>::SimpleOutputPin(Gpo<BankType>& port, BankType index) :
    mPin(1U << index), mPort(port)
{
    OUTPOST_ASSERT(index < (sizeof(BankType) * 8U), "Index out of range!");
}

template <typename BankType>
template <IndexType I>
SimpleOutputPin<BankType>::SimpleOutputPin(Gpo<BankType>& port, Index<I> /* pin */) :
    SimpleOutputPin(port, I)
{
    static_assert(I < (sizeof(BankType) * 8U), "Index out of range!");
}

template <typename BankType>
ReturnCode
SimpleOutputPin<BankType>::set()
{
    return mPort.set(mPin);
}

template <typename BankType>
ReturnCode
SimpleOutputPin<BankType>::clear()
{
    return mPort.clear(mPin);
}

template <typename BankType>
ReturnCode
SimpleOutputPin<BankType>::toggle()
{
    return mPort.toggle(mPin);
}

template <typename BankType>
BankType
SimpleOutputPin<BankType>::getMask() const
{
    return mPin;
}

template <typename BankType>
const Gpo<BankType>&
SimpleOutputPin<BankType>::getPort() const
{
    return mPort;
}

}  // namespace hal
}  // namespace outpost

#endif  // OUTPOST_HAL_GPIO_OUTPUT_PIN_H
