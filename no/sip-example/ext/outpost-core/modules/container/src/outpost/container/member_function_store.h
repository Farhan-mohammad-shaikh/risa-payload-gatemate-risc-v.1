/*
 * Copyright (c) 2016-2018, Fabian Greif
 * Copyright (c) 2016, Benjamin Weps
 * Copyright (c) 2022, Felix Passenberg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2022, Passenberg, Felix Constantin
 * Copyright (c) 2022, Jan-Gerd Mess
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_MEMBER_FUNCTION_STORE_H
#define OUTPOST_UTILS_MEMBER_FUNCTION_STORE_H

#include <outpost/base/callable.h>
#include <outpost/utils/operation_result.h>

#include <stddef.h>

#include <array>

namespace outpost
{
template <size_t N, typename Signature>
class MemberFunctionStore
{
};

/**
 * Storage class for member functions.
 *
 * \author  Fabian Greif
 */
template <size_t N, typename R, typename... As>
class MemberFunctionStore<N, R(As...)>
{
    template <typename C>
    struct MemberFunction
    {
        typedef R (C::*Type)(As...);
    };

    typedef R (Callable::*RawFunctionType)(As...);

    typedef R ReturnType;

    using OperationResult = outpost::utils::OperationResult;

public:
    /**
     * Register a new function.
     *
     * @param replace if true, already registered indexes will be overwritten.
     * @return success if the function was registered, other outOfBounds if out of range or inUse
     * already in use
     */
    template <typename C>
    inline OperationResult
    registerFunction(size_t index,
                     C& object,
                     typename MemberFunction<C>::Type function,
                     bool replace = true)
    {
        if (index < N)
        {
            if (!replace && mObjects[index])
            {
                return OperationResult::inUse;
            }
            mObjects[index] = reinterpret_cast<Callable*>(&object);
            mFunctions[index] = reinterpret_cast<RawFunctionType>(function);
            return OperationResult::success;
        }
        return OperationResult::outOfBounds;
    }

    /**
     * Call a previously registered function.
     *
     * When calling a function index which has not been registered before
     * a default constructed ReturnType is returned and the check parameter is set to false.
     *
     * @param valid check parameter to distinguish between valid default return value and the error
     * case.
     * @param args tuple of arguments for the registered function.
     *              beware implicit conversion might take place
     * @return returns the outcome of the registered function.
     *          Or a default if index refers to a non occupied slot.
     */
    inline ReturnType
    callFunction(size_t index, OperationResult& valid, As... args)
    {
        if (index < N)
        {
            if (mObjects[index] != nullptr)
            {
                valid = OperationResult::success;
                return (mObjects[index]->*mFunctions[index])(args...);
            }
            valid = OperationResult::invalid;
            return ReturnType();
        }
        valid = OperationResult::outOfBounds;
        return ReturnType();
    }

/**
 * Call a previously registered function.
 *
 * When calling a function index which has not been registered before
 * a default constructed ReturnType is returned
 *
 * @param args tuple of arguments for the registered function.
 *              beware implicit conversion might take place
 * @return returns the outcome of the registered function.
 *          Or a default if index refers to a non occupied slot.
 */
#if __cplusplus >= 201402L
    [[deprecated("Use result checking version instead.")]]
#endif
    inline ReturnType
    callFunction(size_t index, As... args)
    {
        if (index < N)
        {
            if (mObjects[index] != nullptr)
            {
                return (mObjects[index]->*mFunctions[index])(args...);
            }
        }
        return ReturnType();
    }

private:
    std::array<Callable*, N> mObjects{{nullptr}};
    std::array<RawFunctionType, N> mFunctions{{nullptr}};
};

}  // namespace outpost

#endif
