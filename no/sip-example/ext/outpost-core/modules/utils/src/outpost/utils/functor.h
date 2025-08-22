/*
 * Copyright (c) 2017-2018, Fabian Greif
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2022, Jan-Gerd Mess
 * Copyright (c) 2023-2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_FUNCTOR_H
#define OUTPOST_UTILS_FUNCTOR_H

#include <outpost/base/callable.h>
#include <outpost/utils/expected.h>
#include <outpost/utils/operation_result.h>

namespace outpost
{

template <typename Signature>
class Functor;

// TODO can be replaced with auto in c++14
template <typename ReturnType>
struct ExecuteReturnType
{
    using Type = Expected<ReturnType, outpost::utils::OperationResult>;
};

template <>
struct ExecuteReturnType<void>
{
    using Type = outpost::utils::OperationResult;
};

/**
 * Definition of a function which is executable by telecommands.
 */
template <typename R, typename... As>
class Functor<R(As...)>
{
private:
    using OperationResult = outpost::utils::OperationResult;

    typedef R (Callable::*FunctionBaseType)(As...);

    typedef R ReturnType;

public:
    template <typename C>
    struct FunctionType
    {
        typedef R (C::*Type)(As...);
    };

    inline Functor() : mObject(nullptr), mFunction(nullptr)
    {
    }

    /**
     * Bind a member function.
     *
     * \param[in]   object
     *      Has to be subclass of outpost::Callable
     * \param[in]   function
     *      Member function pointer of the subscribing class.
     */
    template <typename C>
    inline Functor(C& object, typename FunctionType<C>::Type function) :
        mObject(reinterpret_cast<Callable*>(&object)),
        mFunction(reinterpret_cast<FunctionBaseType>(function))
    {
    }

    /**
     * Checks if the functor is valid.
     *
     * @return True: if object or member function pointer are not set, otherwise false
     */
    bool
    isEmpty() const
    {
        return (mObject == nullptr || mFunction == nullptr);
    }

    /**
     * Execute the contained function.
     *
     * @param args...
     *      the arguments of the function call
     *
     * @return
     *      An \c OperationResult if the underyling function is a void function
     *      An \c Expected which contains either the returned value or an error
     *      code if the function could not be called.
     */
    constexpr typename ExecuteReturnType<R>::Type
    execute(As... args) const
    {
        return executeInternal(mFunction, std::forward<As>(args)...);
    }

    /**
     * Execute the contained function.
     *
     * @param args...
     *      the arguments of the function call
     *
     * @return
     *      An \c OperationResult if the underyling function is a void function
     *      An \c Expected which contains either the returned value or an error
     *      code if the function could not be called.
     */
    constexpr typename ExecuteReturnType<R>::Type
    operator()(As... args) const
    {
        return executeInternal(mFunction, std::forward<As>(args)...);
    }

protected:
    template <typename ReturnType>
    using NonVoidReturnType = Expected<ReturnType, OperationResult>;

    template <typename ReturnType, typename... Args>
    typename std::enable_if<std::is_same<ReturnType, void>::value, OperationResult>::type
    executeInternal(ReturnType (Callable::*)(Args...), Args&&... args) const
    {
        if (!isEmpty())
        {
            (mObject->*mFunction)(args...);
            return OperationResult::success;
        }
        return OperationResult::invalid;
    }

    template <typename ReturnType, typename... Args>
    typename std::enable_if<!std::is_same<ReturnType, void>::value, NonVoidReturnType<R>>::type
    executeInternal(ReturnType (Callable::*)(Args...), Args&&... args) const
    {
        if (!isEmpty())
        {
            return (mObject->*mFunction)(args...);
        }
        return unexpected(OperationResult::invalid);
    }

    Callable* mObject;
    FunctionBaseType mFunction;
};

}  // namespace outpost

#endif
