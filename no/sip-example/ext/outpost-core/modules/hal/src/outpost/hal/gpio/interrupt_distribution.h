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

#ifndef OUTPOST_HAL_GPIO_INTERRUPT_DISTRIBUTION_H
#define OUTPOST_HAL_GPIO_INTERRUPT_DISTRIBUTION_H

#include "outpost/hal/gpio.h"
#include "outpost/hal/gpio/input_pin.h"

namespace outpost
{
namespace hal
{

// ---------------------------------------------------------------------------
/**
 * \class InterruptDistribution
 *
 * \c InterruptDistribution is responsible to forward all interrupts of a
 * given port to the pins. There will be usally one \c InterruptDistribution
 * per port.
 */
class InterruptDistribution : public outpost::Callable
{
public:
    explicit InterruptDistribution();

    InterruptDistribution(const InterruptDistribution&) = delete;

    InterruptDistribution&
    operator=(const InterruptDistribution&) = delete;

    /**
     * This method must be called when all \c InterruptHandlers and
     * \c InputPins are registered and the ISR is active. Any attemp to add
     * a \c InterruptHandler or \c InputPin will be ignored.
     */
    void
    initialize();

    ~InterruptDistribution() = default;

    void
    registerPin(InputPin& pin);

    void
    callHandlers(InterruptType type, uint8_t index);

protected:
    outpost::List<InputPin> mHandlers;
    bool mInitialized;
};

// ---------------------------------------------------------------------------
/**
 * \class SimpleInterruptDistribution
 */
template <typename BankType>
class SimpleInterruptDistribution : public InterruptDistribution
{
public:
    explicit SimpleInterruptDistribution(Gpi<BankType>& gpi, InterruptType type);

    explicit SimpleInterruptDistribution(Gpi<BankType>& gpi, InterruptType type, BankType mask);

    SimpleInterruptDistribution(const SimpleInterruptDistribution&) = delete;

    SimpleInterruptDistribution&
    operator=(const SimpleInterruptDistribution&) = delete;

    ~SimpleInterruptDistribution() = default;

    /**
     * Configure the interrupt service routine to the port. After this call no
     * pins or InterruptHandler should be added anymore due the race-
     * condition with the ISR.
     */
    void
    initialize();

    /**
     * Forward the interrupt to the handlers.
     *
     * \param type
     *      type of the interrupt. Can be one of the following: \c fallingEdge
     *      \c risingEdge, \c lowLevel or \c highLevel
     *
     * \param mask
     *      the affected pins
     */
    void
    callHandlers(InterruptType type, BankType mask);

private:
    Gpi<BankType>& mPort;
    InterruptType mType;
    BankType mMask;
};

// ---------------------------------------------------------------------------
template <typename BankType>
SimpleInterruptDistribution<BankType>::SimpleInterruptDistribution(Gpi<BankType>& gpi,
                                                                   InterruptType type) :
    mPort(gpi), mType(type), mMask(outpost::hal::Gpi<BankType>::maskAllSet)
{
}

template <typename BankType>
SimpleInterruptDistribution<BankType>::SimpleInterruptDistribution(Gpi<BankType>& gpi,
                                                                   InterruptType type,
                                                                   BankType mask) :
    mPort(gpi), mType(type), mMask(mask)
{
}

template <typename BankType>
void
SimpleInterruptDistribution<BankType>::initialize()
{
    InterruptDistribution::initialize();
    mPort.configureInterrupt(mType,
                             this,
                             static_cast<typename outpost::hal::Gpi<BankType>::Handler>(
                                     &SimpleInterruptDistribution::callHandlers),
                             mMask);
}

template <typename BankType>
void
SimpleInterruptDistribution<BankType>::callHandlers(InterruptType type, BankType mask)
{
    auto iter = mHandlers.begin();
    while (iter != mHandlers.end())
    {
        if (mask & (1U << iter->getCustomData()))
        {
            iter->callHandlers(type);
        }
        ++iter;
    }
}

}  // namespace hal
}  // namespace outpost

#endif  // OUTPOST_HGPIO_MANAGER_H
