/*
 * Copyright (c) 2019, 2021, Jan Malburg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_HAL_TIMECODE_DISPATCHER_H_
#define OUTPOST_HAL_TIMECODE_DISPATCHER_H_

#include "timecode.h"

#include <outpost/rtos.h>
#include <outpost/rtos/queue.h>

#include <stdint.h>

#include <array>

namespace outpost
{
namespace hal
{
/**
 * An explicit Timecode/SpWInterrupt Dispatcher class such we don't get template parameters into
 * SpaceWire
 */
class TimeCodeDispatcherInterface
{
public:
    TimeCodeDispatcherInterface() = default;
    virtual ~TimeCodeDispatcherInterface() = default;

    /**
     * Dispatches a timecode to the Listener
     * This function shall be save to call within an ISR.
     *
     * @param tc the timecode(SpW Interrupt to dispatch
     */
    virtual void
    dispatchTimeCode(const TimeCode& tc) = 0;

    /**
     * Add a listener for timecode
     * @param queue the queue to add
     * @return false if queue == nullptr or all places for Listener are filled
     */
    virtual bool
    addListener(outpost::rtos::Queue<TimeCode>* queue) = 0;
};

template <uint32_t numberOfQueues>  // how many queues can be included
class TimeCodeDispatcher : public TimeCodeDispatcherInterface
{
public:
    TimeCodeDispatcher() : mNumberOfListeners(0)
    {
        mListener.fill(nullptr);
    };

    virtual ~TimeCodeDispatcher() = default;

    virtual void
    dispatchTimeCode(const TimeCode& tc) override
    {
        // For one we are in a ISR and also nNumberOfListener only increments to
        // we don't need any mutex as worst case a just added listener don't get a notify.
        for (uint32_t i = 0; i < mNumberOfListeners; i++)
        {
            mListener[i]->send(tc);
        }
    }

    virtual bool
    addListener(outpost::rtos::Queue<TimeCode>* queue) override
    {
        if (nullptr == queue)
        {
            return false;
        }

        outpost::rtos::MutexGuard lock(mMutex);
        if (mNumberOfListeners >= numberOfQueues)
        {
            return false;
        }

        mListener[mNumberOfListeners] = queue;
        mNumberOfListeners++;
        return true;
    }

private:
    std::array<outpost::rtos::Queue<TimeCode>*, numberOfQueues> mListener;
    uint32_t mNumberOfListeners;
    outpost::rtos::Mutex mMutex;
};

}  // namespace hal
}  // namespace outpost

#endif /* MODULES_HAL_SRC_OUTPOST_HAL_TIMECODE_DISPATCHER_H_ */
