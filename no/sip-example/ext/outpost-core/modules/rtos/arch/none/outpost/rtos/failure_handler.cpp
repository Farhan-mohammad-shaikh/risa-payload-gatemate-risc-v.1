/*
 * Copyright (c) 2014-2017, Fabian Greif
 * Copyright (c) 2018, Jan Sommer
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/rtos/failure_handler.h>

static void
defaultHandler(outpost::rtos::FailureCode code)
{
    // Avoid warnings about unused parameters.
    (void) code;
}

outpost::rtos::FailureHandler::Handler outpost::rtos::FailureHandler::handler = &defaultHandler;
outpost::rtos::FailureHandler::Handler outpost::rtos::FailureHandler::cleanup = &defaultHandler;

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
