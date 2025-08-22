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

#ifndef UNITTEST_HAL_GPI_STUB_H
#define UNITTEST_HAL_GPI_STUB_H

#include <outpost/hal/gpio.h>

#include <functional>

namespace unittest
{
namespace hal
{

template <typename BankType = uint32_t>
class GpiStub : public outpost::hal::Gpi<BankType>
{
public:
    GpiStub() :
        mType(outpost::hal::InterruptType::none),
        mObject(nullptr),
        mHandler(nullptr),
        mPort(0U),
        mMask(0U)
    {
    }

    outpost::hal::ReturnCode
    configureInterrupt(outpost::hal::InterruptType type,
                       outpost::Callable* object,
                       typename outpost::hal::Gpi<BankType>::Handler handler,
                       BankType mask = outpost::hal::Gpi<BankType>::maskAllSet) override
    {
        mType = type;
        mHandler = handler;
        mObject = object;
        mMask = mask;
        return outpost::hal::ReturnCode::success;
    }

    BankType
    get(BankType mask) override
    {
        if (mGetFunction)
        {
            return mGetFunction(mask);
        }
        else
        {
            return mask & mPort;
        }
    }

    void
    set(BankType values)
    {
        mPort = values;
    }

    outpost::hal::ReturnCode
    enableInterrupt() override
    {
        return outpost::hal::ReturnCode::success;
    }

    outpost::hal::ReturnCode
    disableInterrupt() override
    {
        return outpost::hal::ReturnCode::success;
    }

    void
    callHandler(outpost::hal::InterruptType type, BankType mask)
    {
        if ((type == mType)
            || ((mType == outpost::hal::InterruptType::bothEdges)
                && ((type == outpost::hal::InterruptType::fallingEdge)
                    || (type == outpost::hal::InterruptType::risingEdge)))
            || ((mType == outpost::hal::InterruptType::bothLevels)
                && ((type == outpost::hal::InterruptType::lowLevel)
                    || (type == outpost::hal::InterruptType::highLevel))))
        {
            if (mask & mMask)
            {
                (*mObject.*mHandler)(type, mask);
            }
        }
    }

    outpost::hal::InterruptType mType;
    outpost::Callable* mObject;
    typename outpost::hal::Gpi<BankType>::Handler mHandler;
    BankType mPort;
    BankType mMask;
    std::function<BankType(BankType)> mGetFunction;
};

// ---------------------------------------------------------------------------
/**
 * \class GpoStub
 */
template <typename BankType = uint32_t>
class GpoStub : public outpost::hal::Gpo<BankType>
{
public:
    explicit GpoStub() : mValue(0U)
    {
    }

    GpoStub(const GpoStub&) = delete;

    GpoStub&
    operator=(const GpoStub&) = delete;

    ~GpoStub() = default;

    outpost::hal::ReturnCode
    write(BankType value, BankType mask = outpost::hal::Gpo<BankType>::maskAllSet) override
    {
        if (mWrite)
        {
            return mWrite(value, mask);
        }
        else
        {
            mValue |= (mask & value);
            mValue &= ~(mask & ~value);
            return outpost::hal::ReturnCode::success;
        }
    }

    outpost::hal::ReturnCode
    set(BankType mask = outpost::hal::Gpo<BankType>::maskAllSet) override
    {
        if (mSet)
        {
            return mSet(mask);
        }
        else
        {
            mValue |= mask;
            return outpost::hal::ReturnCode::success;
        }
    }

    outpost::hal::ReturnCode
    clear(BankType mask = outpost::hal::Gpo<BankType>::maskAllSet) override
    {
        if (mClear)
        {
            return mClear(mask);
        }
        else
        {
            mValue &= ~mask;
            return outpost::hal::ReturnCode::success;
        }
    }

    outpost::hal::ReturnCode
    toggle(BankType mask = outpost::hal::Gpo<BankType>::maskAllSet) override
    {
        if (mToggle)
        {
            return mToggle(mask);
        }
        else
        {
            mValue = mValue ^ mask;
            return outpost::hal::ReturnCode::success;
        }
    }

    BankType
    get(BankType mask = outpost::hal::Gpo<BankType>::maskAllSet) override
    {
        return mValue & mask;
    }

    BankType mValue;
    std::function<outpost::hal::ReturnCode(BankType, BankType)> mWrite;
    std::function<outpost::hal::ReturnCode(BankType)> mSet;
    std::function<outpost::hal::ReturnCode(BankType)> mClear;
    std::function<outpost::hal::ReturnCode(BankType)> mToggle;
};

}  // namespace hal
}  // namespace unittest

#endif  // UNITTEST_HAL_GPI_STUB_H
