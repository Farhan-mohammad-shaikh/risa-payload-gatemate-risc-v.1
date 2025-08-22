/*
 * Copyright (c) 2019, 2021, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/hal/space_wire_multi_protocol_handler.h>
#include <outpost/rtos.h>
#include <outpost/support/heartbeat.h>

#include <unittest/hal/spacewire_stub.h>
#include <unittest/harness.h>

TEST(SpaceWireMultiProtocolHandlerTest, construct)
{
    outpost::rtos::SystemClock clock;
    unittest::hal::SpaceWireStub spw(4500);
    outpost::hal::SpaceWireMultiProtocolHandler<2> spwmp(
            spw, 100, outpost::support::parameter::HeartbeatSource::default0, clock);
    outpost::hal::SpaceWireMultiProtocolHandlerInterface& ref = spwmp;
    (void) ref;
}
