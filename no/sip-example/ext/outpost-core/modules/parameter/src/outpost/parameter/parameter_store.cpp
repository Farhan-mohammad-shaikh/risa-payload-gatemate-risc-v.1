/*
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2022, Jan-Gerd Mess
 * Copyright (c) 2023, Pascal Pieper
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "parameter_store.h"

#include <algorithm>

namespace outpost
{
namespace parameter
{
OperationResult
ParameterStore::initialize(ParameterList& list)
{
    ParameterList* listArray[1];
    listArray[0] = &list;

    return initialize(outpost::asSlice(listArray));
}

OperationResult
ParameterStore::initialize(outpost::Slice<ParameterList*> lists)
{
    if (mInitialized)
    {
        return OperationResult::alreadyInitialized;
    }

    if (lists.getNumberOfElements() > mParameters.getNumberOfElements())
    {
        return OperationResult::tooManyElements;
    }

    size_t count = 0;
    ParameterBase* pos = nullptr;
    for (unsigned int it = 0; it < lists.getNumberOfElements(); it++)
    {
        pos = lists[it]->anchor;
        while (nullptr != pos)
        {
            if (count >= mParameters.getNumberOfElements())
            {
                return OperationResult::tooManyElements;
            }

            if (pos->hasInvalidIdAssigned())
            {
                return OperationResult::invalidParameter;
            }
            else if (!pos->isInitialized())
            {
                return OperationResult::uninitializedParameter;
            }
            mParameters[count] = pos;
            pos = pos->getNext();
            count++;
        }
    }

    mCount = count;

    ParameterBase::sort(mParameters.first(mCount));

    for (size_t i = 1; i < mCount; i++)
    {
        if (!((*mParameters[i - 1]) < (*mParameters[i])))
        {
            return OperationResult::duplicatedID;
        }
    }

    mInitialized = true;

    return OperationResult::success;
}

OperationResult
ParameterStore::getParameter(IDType id, const ParameterBase*& store) const
{
    if (!mInitialized)
    {
        return OperationResult::notInitialized;
    }

    // sanity check
    if (mCount == 0)
    {
        return OperationResult::noSuchID;
    }

    if (id == ParameterBase::invalidID)
    {
        return OperationResult::invalidParameter;
    }

    auto pointer = ParameterBase::findInSorted(mParameters.first(mCount), id);

    if (nullptr == pointer)
    {
        return OperationResult::noSuchID;
    }

    store = pointer;
    return OperationResult::success;
}

ParameterStore::Iterator
ParameterStore::begin() const
{
    ParameterStore::Iterator it;
    it.mPointer = this->mParameters.begin();
    return it;
}

ParameterStore::Iterator
ParameterStore::end() const
{
    // mCount is 0 if ParameterStore is not initialized
    const auto maxNumParameters = this->mParameters.getNumberOfElements();
    if (mCount == 0 || maxNumParameters == 0)
    {
        return begin();
    }
    else
    {
        ParameterStore::Iterator it;
        // ugly way of getting a pointer beyond valid data
        it.mPointer = &(this->mParameters.getDataPointer()[mCount]);
        return it;
    }
}

size_t
ParameterStore::getCount() const
{
    return mCount;
}

bool
ParameterStore::isInitialized() const
{
    return mInitialized;
}

}  // namespace parameter
}  // namespace outpost
