/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2013, Norbert Toth
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2021, pfef_to
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2022, Jan-Gerd Mess
 * Copyright (c) 2023, Hannah Kirstein
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "thread.h"

#include "thread_priorities.h"

#include <outpost/rtos/failure_handler.h>

#include <stdio.h>

/// Minimum stack size configured through the FreeRTOS configuration.
static const size_t minimumStackSize = configMINIMAL_STACK_SIZE * sizeof(portSTACK_TYPE);

// ----------------------------------------------------------------------------
void
outpost::rtos::Thread::wrapper(void* object)
{
    Thread* thread = reinterpret_cast<Thread*>(object);
    thread->run();

    // Returning from a FreeRTOS thread is a fatal error, nothing more to
    // do here than call the fatal error handler.
    rtos::FailureHandler::fatal(rtos::FailureCode::returnFromThread());
}

// ----------------------------------------------------------------------------
outpost::rtos::Thread::Thread(uint8_t priority,
                              size_t stack,
                              const char* name,
                              FloatingPointSupport /*floatingPointSupport*/,
                              CpuMask /*affinity*/) :
    mHandle(0), mPriority(priority), mStackSize(stack), mName(name)
{
    if (mStackSize < minimumStackSize)
    {
        mStackSize = minimumStackSize;
    }
}

outpost::rtos::Thread::~Thread()
{
    if (mHandle != 0)
    {
        vTaskDelete(mHandle);
    }
}

// ----------------------------------------------------------------------------
void
outpost::rtos::Thread::start()
{
    if (mHandle == 0)
    {
        int status = xTaskCreate(&Thread::wrapper,
                                 mName,
                                 (mStackSize / sizeof(portSTACK_TYPE)) + 1,
                                 this,
                                 static_cast<unsigned portBASE_TYPE>(
                                         toFreeRtosPriority(mPriority, configMAX_PRIORITIES)),
                                 // 3,
                                 &mHandle);

        if (status != pdPASS)
        {
            FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::thread));
        }
    }
}

// ----------------------------------------------------------------------------
outpost::rtos::Thread::Identifier
outpost::rtos::Thread::getIdentifier() const
{
    if (mHandle == 0)
    {
        return invalidIdentifier;
    }
    else
    {
        return reinterpret_cast<Identifier>(mHandle);
    }
}

outpost::rtos::Thread::Identifier
outpost::rtos::Thread::getCurrentThreadIdentifier()
{
    return reinterpret_cast<Identifier>(xTaskGetCurrentTaskHandle());
}

// ----------------------------------------------------------------------------
void
outpost::rtos::Thread::setPriority(uint8_t priority)
{
    vTaskPrioritySet(mHandle, toFreeRtosPriority(priority, configMAX_PRIORITIES));
}

uint8_t
outpost::rtos::Thread::getPriority() const
{
    return fromFreeRtosPriority(uxTaskPriorityGet(mHandle), configMAX_PRIORITIES);
}

// ----------------------------------------------------------------------------
void
outpost::rtos::Thread::yield()
{
    taskYIELD();
}

// ----------------------------------------------------------------------------
void
outpost::rtos::Thread::sleep(::outpost::time::Duration duration)
{
    if (duration.milliseconds() > 1000 * 1000)
    {  // prevent overflows of large delays and integer division to zero of small delays
        vTaskDelay((duration.milliseconds() / 1000) * configTICK_RATE_HZ);
    }
    else
    {
        vTaskDelay((duration.milliseconds() * configTICK_RATE_HZ) / 1000);
    }
}

// ----------------------------------------------------------------------------
void
outpost::rtos::Thread::startScheduler()
{
#if !defined(GOMSPACE)
    vTaskStartScheduler();
    FailureHandler::fatal(FailureCode::returnFromThread());
#else
    FailureHandler::fatal(FailureCode::genericRuntimeError(Resource::thread));
#endif
}
