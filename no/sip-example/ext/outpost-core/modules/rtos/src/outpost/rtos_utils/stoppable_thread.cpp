/*
 * Copyright (c) 2014-2017, Fabian Greif
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Reinking, Janosch
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "stoppable_thread.h"

namespace outpost::rtos_utils
{

void
StoppableThread::doStop()
{
    mStop = true;
}

void
StoppableThread::run()
{
    while (!mStop)
    {
        step();
    }
}

}  // namespace outpost::rtos_utils
