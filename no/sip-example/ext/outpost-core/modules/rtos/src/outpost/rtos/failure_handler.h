/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 * Copyright (c) 2018, Jan Sommer
 * Copyright (c) 2020, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

/**
 * \file
 * \brief   Failure code and failure handler class
 *
 * \author  Fabian Greif
 */

#ifndef OUTPOST_RTOS_FAILURE_H
#define OUTPOST_RTOS_FAILURE_H

#include <stdint.h>

namespace outpost
{
namespace rtos
{
struct Resource
{
    enum Type
    {
        other = 0,
        thread = 1,
        timer = 2,
        semaphore = 3,
        mutex = 4,
        interrupt = 5,
        messageQueue = 6,
        clock = 7,
        periodicTask = 8,
        driverManager = 9,
        barrier = 10
    };
};

// FIXME create an overview of the values used here
class FailureCode
{
public:
    inline static FailureCode
    resourceAllocationFailed(Resource::Type resource = Resource::other)
    {
        FailureCode code = FailureCode(0xF0010000 | static_cast<uint32_t>(resource));
        return code;
    }

    inline static FailureCode
    returnFromThread()
    {
        FailureCode code = FailureCode(0xF0020000);
        return code;
    }

    inline static FailureCode
    genericRuntimeError(Resource::Type resource = Resource::other)
    {
        FailureCode code = FailureCode(0xF0030000 | static_cast<uint32_t>(resource));
        return code;
    }

    inline static FailureCode
    userError(uint16_t id = 0)
    {
        FailureCode code = FailureCode(0xF0040000 | id);
        return code;
    }

    inline uint32_t
    getCode() const
    {
        return mCode;
    }

protected:
    explicit inline FailureCode(uint32_t code) : mCode(code)
    {
    }

private:
    const uint32_t mCode;
};

class FailureHandler
{
public:
    typedef void (*Handler)(FailureCode code);

    /**
     * Fatal failure has occurred which makes a continuation of the
     * current execution path impossible.
     *
     * If compiled using RTEMS this function will execute the RTEMS
     * Fatal Error Manager.
     *
     * \warning This function WILL NOT RETURN!
     *
     * \param code
     *         Failure code to identify the failure.
     */
    static void
    fatal(FailureCode code);

    /**
     * Set a new failure handler.
     *
     * \warning This function must not return!
     *
     * \param newHandler
     */
    static void
    setFailureHandlerFunction(Handler newHandler);

    /**
     * Set a new clean-up handler
     *
     * The handler will be called before the fatal error handler is called.
     * It can be used to e.g. save some critical system states into non-volatile
     * memory before the system is halted and eventually rebooted.
     *
     * \param newHandler
     */
    static void
    setCleanupHandlerFunction(Handler newHandler);

protected:
    static Handler handler;
    static Handler cleanup;
};

}  // namespace rtos
}  // namespace outpost

#endif
