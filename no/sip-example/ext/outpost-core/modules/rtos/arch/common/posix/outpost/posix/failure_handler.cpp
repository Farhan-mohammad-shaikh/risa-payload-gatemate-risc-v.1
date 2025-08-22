/*
 * Copyright (c) 2014-2017, 2019, Fabian Greif
 * Copyright (c) 2018, Jan Sommer
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

#include <outpost/rtos/failure_handler.h>

#include <inttypes.h>
#include <stdio.h>

#include <cstdlib>

// ---------------------------------------------------------------------------
static void
defaultFatalHandler(outpost::rtos::FailureCode code)
{
    printf("Fatal Handler: 0x%08X\n", static_cast<int>(code.getCode()));
    exit(1);
}

static void
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
