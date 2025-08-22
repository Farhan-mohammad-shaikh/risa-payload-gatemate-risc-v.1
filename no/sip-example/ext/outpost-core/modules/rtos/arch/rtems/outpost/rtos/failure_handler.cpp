/*
 * Copyright (c) 2013-2018, Fabian Greif
 * Copyright (c) 2018, Jan Sommer
 * Copyright (c) 2018, Jan Malburg
 * Copyright (c) 2022, Jan-Gerd Mess
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <rtems.h>

#include <outpost/rtos/failure_handler.h>

void
defaultFatalHandler(outpost::rtos::FailureCode code);

void
defaultFatalHandler(outpost::rtos::FailureCode code)
{
    rtems_fatal_error_occurred(code.getCode());
}

void
defaultCleanupHandler(outpost::rtos::FailureCode code);

void
defaultCleanupHandler(outpost::rtos::FailureCode /*code*/)
{
}

outpost::rtos::FailureHandler::Handler outpost::rtos::FailureHandler::handler =
        &defaultFatalHandler;
outpost::rtos::FailureHandler::Handler outpost::rtos::FailureHandler::cleanup =
        &defaultCleanupHandler;

// ---------------------------------------------------------------------------
void
outpost::rtos::FailureHandler::fatal(FailureCode code)
{
    // forward call to handler function
    cleanup(code);
    handler(code);
}

void
outpost::rtos::FailureHandler::setFailureHandlerFunction(Handler newHandler)
{
    handler = newHandler;
}

void
outpost::rtos::FailureHandler::setCleanupHandlerFunction(Handler newHandler)
{
    cleanup = newHandler;
}
