/*
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2021, Passenberg, Felix Constantin
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2022-2023, Jan-Gerd Mess
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2023, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_PARAMETER_PARAMETER_IMPL_H_
#define OUTPOST_PARAMETER_PARAMETER_IMPL_H_

#include "parameter.h"

#include <outpost/base/testing_assert.h>

namespace outpost
{
namespace parameter
{

template <uint16_t index>
ParameterList IndexedParameterList<index>::list;

template <typename T>
Parameter<T>::Parameter(IDType id,
                        const T& initialValue,
                        const outpost::time::SpacecraftElapsedTime& time,
                        ParameterList& list,
                        bool hasMultipleWriters) :
    ParameterBase(list)
{
    mID = id;
    mWritesCounter = 0;

    if (id == invalidID)
    {
        mMultipleWriters = false;
        mAssignedIdInvalid = true;
    }
    else
    {
        mMultipleWriters = hasMultipleWriters;

        mElements[0].data = initialValue;
        mElements[0].changeTime = time;

        mElements[1].data = initialValue;
        mElements[1].changeTime = time;

        mAssignedIdInvalid = false;
    }
}

template <typename T>
OperationResult
Parameter<T>::initialize(IDType id,
                         const T& initialValue,
                         const outpost::time::SpacecraftElapsedTime& time,
                         bool hasMultipleWriters)
{
    if (mID != invalidID)
    {
        return OperationResult::alreadyInitialized;
    }

    if (id == invalidID)
    {
        mAssignedIdInvalid = true;
        return OperationResult::invalidParameter;
    }

    mID = id;

    mMultipleWriters = hasMultipleWriters;

    mElements[0].data = initialValue;
    mElements[0].changeTime = time;

    mElements[1].data = initialValue;
    mElements[1].changeTime = time;

    mWritesCounter = 0;
    mAssignedIdInvalid = false;
    return OperationResult::success;
}

template <typename T>
OperationResult
Parameter<T>::getValue(T& store, outpost::time::SpacecraftElapsedTime* time) const
{
    if (mID == invalidID)
    {
        return OperationResult::notInitialized;
    }

    Element tmp;

    uint32_t tries = 0U;

    bool success = false;
    do
    {
        // memory_order_acquire assures that the read operation of the elements
        // will not happen before the first load of the counter
        CounterRaw before = mWritesCounter.load(std::memory_order_acquire);
        tmp = mElements[before % 2];

        /// this standalone fence prevents that loading of \c mElements will
        /// not reorder with any load or store operation after the fence.
        /// (a.k.a second load of the counter)
        std::atomic_thread_fence(std::memory_order_acquire);
        CounterRaw after = mWritesCounter.load(std::memory_order_relaxed);

        success = (before == after);
        tries++;
    } while ((!success) && (tries < maxReadTries));  // Fails if a write occurred during read
                                                     // each try needs its own write to fail

    if (!success)
    {
        return OperationResult::tooManyConcurrentWrites;
    }

    store = tmp.data;
    if (nullptr != time)
    {
        *time = tmp.changeTime;
    }

    return OperationResult::success;
}

template <typename T>
OperationResult
Parameter<T>::setValue(const T& data, const outpost::time::SpacecraftElapsedTime& time)
{
    if (mID == invalidID)
    {
        return OperationResult::notInitialized;
    }

    if (!mMultipleWriters)
    {
        // no need for synchronization since only the value is important
        // to access the right element: use memory_order_relaxed
        CounterRaw counter = mWritesCounter.load(std::memory_order_relaxed);

        mElements[(counter + 1) % 2].changeTime = time;
        mElements[(counter + 1) % 2].data = data;

        // memory_order_release ensures that the write operation to the
        // elements are done at this point. Furtheremore this release
        // syncs with the memory_order_acquire of the load operation
        // when reading an element.
        mWritesCounter.fetch_add(1, std::memory_order_release);

        return OperationResult::success;
    }
    else
    {
        if (!mWriteInProgress.test_and_set())
        {
            // no need for synchronization since only the value is important
            // to access the right element: use memory_order_relaxed
            CounterRaw counter = mWritesCounter.load(std::memory_order_relaxed);

            mElements[(counter + 1) % 2].changeTime = time;
            mElements[(counter + 1) % 2].data = data;

            // memory_order_release ensures that the write operation to the
            // elements are done at this point. Furtheremore this release
            // syncs with the memory_order_acquire of the load operation
            // when reading an element.
            mWritesCounter.fetch_add(1, std::memory_order_release);

            mWriteInProgress.clear();
            return OperationResult::success;
        }
        else
        {
            return OperationResult::concurrentWrite;
        }
    }
}

}  // namespace parameter
}  // namespace outpost

#endif
