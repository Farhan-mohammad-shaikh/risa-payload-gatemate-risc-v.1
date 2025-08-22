/*
 * Copyright (c) 2023-2024, Janosch Reinking
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_HAL_GPIO_INPUT_PIN_H
#define OUTPOST_HAL_GPIO_INPUT_PIN_H

#include "outpost/hal/gpio.h"
#include "outpost/hal/gpio/index.h"
#include <outpost/base/testing_assert.h>
#include <outpost/container/list.h>
#include <outpost/utils/expected.h>
#include <outpost/utils/functor.h>

namespace outpost
{
namespace hal
{

// ---------------------------------------------------------------------------
/**
 * \class InterruptHandler
 *
 * The \c InterruptHandler combines a pointer to a method with an interrupt
 * type. Object of this type must be registered to a \c InputPin.  When a
 * interrupt of the given type occures the callback handler is executed.
 *
 * \warning
 *      usally the callback will be called from an ISR. so make sure that you
 *      do not call blocking functions!
 */
class InterruptHandler : public outpost::ListElement
{
public:
    using Functor1 = outpost::Functor<void()>;

    using Functor2 = outpost::Functor<void(InterruptType)>;

    template <typename T>
    using Function1 = void (T::*)();

    template <typename T>
    using Function2 = void (T::*)(InterruptType);

    /**
     * Contructor
     *
     * \param type
     *      the interrupt type to listen
     * \param functor
     *      callback for the interrupt
     */
    template <typename T>
    explicit InterruptHandler(InterruptType type, T* object, Function1<T> function) :
        mFunctor(Functor1(*object, function)), mType(type)
    {
    }

    /**
     * Contructor
     *
     * \param type
     *      the interrupt type to listen
     * \param functor
     *      callback for the interrupt
     */
    template <typename T>
    explicit InterruptHandler(InterruptType type, T* object, Function2<T> function) :
        mFunctorWithInterruptType(Functor2(*object, function)), mType(type)
    {
    }

    InterruptHandler(const InterruptHandler&) = delete;

    InterruptHandler&
    operator=(const InterruptHandler&) = delete;

    ~InterruptHandler() = default;

    /**
     * call the handler if the interrupt type match
     *
     * \param type
     *      the type of the current interrupt
     */
    inline void
    callHandler(InterruptType type)
    {
        if ((type == mType)
            || ((mType == InterruptType::bothEdges)
                && ((type == InterruptType::fallingEdge) || (type == InterruptType::risingEdge)))
            || ((mType == InterruptType::bothLevels)
                && ((type == InterruptType::lowLevel) || (type == InterruptType::highLevel))))
        {
            if (!mFunctor.isEmpty())
            {
                mFunctor();
            }
            else
            {
                mFunctorWithInterruptType(type);
            }
        }
    }

private:
    const Functor2 mFunctorWithInterruptType;
    const Functor1 mFunctor;
    const InterruptType mType;
};

// ---------------------------------------------------------------------------
/**
 * \class InputPin
 *
 * An input pin is an abstraction for pins. It does not carry information on
 * the concrete pin which is used or on which port it is. Multiple
 * \c InterruptHandler can be added. There is an implementation which can be
 * used with the \c outpost::hal::Gpi<BankType> interface which is
 * \c SimpleInputPin. But it is intended that there can be project specific
 * implementations. A class / module that wants to use a pin should have a
 * parameter which takes a reference to an \c InputPin.
 */
class InputPin : public outpost::ListElement
{
public:
    using CustomData = uint32_t;

    explicit InputPin();

    /**
     * Constructor
     *
     * \param customData
     *      extra data which can be used by project specific implementation to
     *      store per \c InputPin data. This can be for example the index of
     *      the pin or a bit mask representing the pin.
     */
    explicit InputPin(CustomData customData);

    InputPin(const InputPin&) = delete;

    InputPin&
    operator=(const InputPin&) = delete;

    virtual ~InputPin() = default;

    /**
     * This function must be called when the initialization phase is over.
     * A.k.a it is not possible to add further \c InterruptHandler since
     * the list is used by the ISR to forward the interrupt. Any write
     * to the list would cause a race condition.
     */
    void
    initialized();

    /**
     * Register a \c InterruptHandler
     *
     * \warning
     *      handlers must be added at system start when no interrupt can be
     *      triggered!
     *
     * \param handler
     *      the handler to add
     */
    void
    registerInterruptHandler(InterruptHandler& handler);

    /**
     * This method must be called from the interrupt manager. It will forward
     * the interrupt to the handlers if the type matches.
     *
     * \param type
     *      type of the interrupt. Can be one of the following: \c fallingEdge
     *      \c risingEdge, \c lowLevel or \c highLevel
     */
    inline void
    callHandlers(InterruptType type)
    {
        auto iter = mHandlers.begin();
        while (iter != mHandlers.end())
        {
            iter->callHandler(type);
            ++iter;
        }
    }

    /**
     * Read the pin.
     *
     * \return
     *      the value of the pin
     */
    virtual Expected<bool, ReturnCode>
    read() const;

    /**
     * Get custom data
     *
     * \warning
     *      This method should not be called by the user of the \c InputPin.
     *
     * It is intended to be used by project specific implementations. It will
     * return the custom data passed to the constructor.
     */
    CustomData
    getCustomData() const;

private:
    outpost::List<InterruptHandler> mHandlers;

    const CustomData mCustomData;
    bool mInitialized;
};

// ---------------------------------------------------------------------------
/**
 * \class SimpleInputPin
 *
 * A concrete implementation for \c InputPin. It works with the interface
 * \c outpost::hal::Gpi<BankType>. This class should only be used to create
 * pins. No references should be passed arround. For this case use references
 * to \c InputPin.
 */
template <typename BankType = uint32_t>
class SimpleInputPin : public InputPin
{
public:
    /**
     * \param port
     *      the port of the pin
     * \param index
     *      must be in range of the port
     */
    explicit SimpleInputPin(Gpi<BankType>& port, uint8_t index);

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
     *      index of the pin
     */
    template <IndexType I>
    explicit SimpleInputPin(Gpi<BankType>& port, Index<I> pin);

    SimpleInputPin(const SimpleInputPin&) = delete;

    SimpleInputPin&
    operator=(const SimpleInputPin&) = delete;

    virtual ~SimpleInputPin() = default;

    void
    callHandlers(BankType mask, InterruptType type);

    Expected<bool, ReturnCode>
    read() const override;

    const Gpi<BankType>&
    getPort() const;

    BankType
    getMask() const;

private:
    const BankType mPin;
    Gpi<BankType>& mPort;
};

// ---------------------------------------------------------------------------
template <typename BankType>
SimpleInputPin<BankType>::SimpleInputPin(Gpi<BankType>& port, uint8_t index) :
    InputPin(index), mPin(1U << index), mPort(port)
{
    OUTPOST_ASSERT(index < (sizeof(BankType) * 8U), "Index out of range!");
}

template <typename BankType>
template <IndexType I>
SimpleInputPin<BankType>::SimpleInputPin(Gpi<BankType>& port, Index<I> /* pin */) :
    SimpleInputPin(port, I)
{
    static_assert(I < (sizeof(BankType) * 8U), "Index out of range!");
}

template <typename BankType>
void
SimpleInputPin<BankType>::callHandlers(BankType mask, InterruptType type)
{
    if (mask & mPin)
    {
        InputPin::callHandlers(type);
    }
}

template <typename BankType>
Expected<bool, ReturnCode>
SimpleInputPin<BankType>::read() const
{
    return mPort.get(mPin) > 0U;
}

template <typename BankType>
const Gpi<BankType>&
SimpleInputPin<BankType>::getPort() const
{
    return mPort;
}

template <typename BankType>
BankType
SimpleInputPin<BankType>::getMask() const
{
    return mPin;
}

}  // namespace hal
}  // namespace outpost

#endif  // OUTPOST_HAL_GPIO_INPUT_PIN_H
