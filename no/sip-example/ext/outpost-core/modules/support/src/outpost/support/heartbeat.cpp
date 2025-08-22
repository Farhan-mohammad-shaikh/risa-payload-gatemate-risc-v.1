/*
 * Copyright (c) 2018, Annika Ofenloch
 * Copyright (c) 2018, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "heartbeat.h"

outpost::smpc::Topic<const outpost::support::Heartbeat> outpost::support::watchdogHeartbeat;
