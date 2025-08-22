/*
 * Copyright (c) 2013-2019, Fabian Greif
 * Copyright (c) 2023, Hannah Kirstein
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_POSIX_TIMER_H
#define OUTPOST_RTOS_POSIX_TIMER_H

#include "outpost/posix/timer_raw.h"
#include <outpost/base/callable.h>
#include <outpost/parameter/rtos.h>
#include <outpost/time/duration.h>

#include <signal.h>
#include <time.h>

namespace outpost
{
namespace rtos
{
/**
 * Software timer.
 *
 * \author    Fabian Greif
 * \ingroup    rtos
 */
class Timer : public outpost::posix::TimerRaw
{
public:
    template <typename T>
    struct TimerFunction
    {
        typedef void (T::*type)(Timer* timer);
    };

    /**
     * Create a timer.
     *
     * \param object
     *         Instance to with the function to be called belongs. Must
     *         be sub-class of outpost::rtos::Callable.
     * \param function
     *         Member function of \p object to call when the timer
     *         expires.
     * \param name
     *         Name of the timer. Maximum length is four characters. Longer
     *         names will be truncated.
     *
     * \see    outpost::Callable
     */
    template <typename T>
    Timer(T* object, typename TimerFunction<T>::type function, const char* name = "TIM-");

private:
};

// ----------------------------------------------------------------------------
// Implementation of the template constructor
template <typename T>
Timer::Timer(T* object, typename TimerFunction<T>::type function, const char* name) :
    TimerRaw(this, reinterpret_cast<Callable*>(object), reinterpret_cast<Function>(function), name)
{
}

}  // namespace rtos
}  // namespace outpost

#endif
