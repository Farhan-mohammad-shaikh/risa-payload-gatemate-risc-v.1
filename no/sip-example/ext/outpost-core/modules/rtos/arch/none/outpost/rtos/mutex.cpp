/*
 * Copyright (c) 2013-2017, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "mutex.h"

#include <outpost/rtos/failure_handler.h>

// ----------------------------------------------------------------------------
outpost::rtos::Mutex::Mutex()
{
    // TODO
}

outpost::rtos::Mutex::~Mutex()
{
    // TODO
}

// ----------------------------------------------------------------------------
bool
outpost::rtos::Mutex::acquire()
{
    // TODO
    return false;
}

bool
outpost::rtos::Mutex::acquire(time::Duration timeout)
{
    (void) timeout;  // not used

    // TODO
    return false;
}
