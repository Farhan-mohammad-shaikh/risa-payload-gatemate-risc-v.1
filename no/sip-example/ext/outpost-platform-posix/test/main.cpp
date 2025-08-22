/*
 * Copyright (c) 2017, Jan Sommer
 * Copyright (c) 2017, Fabian Greif
 * Copyright (c) 2021, Jan-Gerd Mess
 * Copyright (c) 2021, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <gtest/gtest.h>

#include <unittest/configurable_event_listener.h>

int
main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    unittest::registerConfigurableEventListener();

    return RUN_ALL_TESTS();
}
