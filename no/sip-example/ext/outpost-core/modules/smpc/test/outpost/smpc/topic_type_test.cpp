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

#include <outpost/smpc/topic.h>

#include <unittest/harness.h>

/**
 * Check that it is possible to compile topic with selected types.
 */
TEST(TopicTypeTest, testInteger)
{
    outpost::smpc::Topic<const uint16_t> keepAlive;

    keepAlive.publish(123);

    uint16_t apid = 234;
    keepAlive.publish(apid);
}

TEST(TopicTypeTest, testArray)
{
    static const size_t numberOfSunSensors = 10;

    outpost::smpc::Topic<const uint16_t[numberOfSunSensors]> sunSensors;

    uint16_t values[10] = {};

    sunSensors.publish(values);
}

TEST(TopicTypeTest, testPowerStruct)
{
    struct PowerState
    {
        bool device1;
        bool device2;
    };

    outpost::smpc::Topic<const PowerState> powerState;

    PowerState p;
    p.device1 = true;
    p.device2 = false;

    powerState.publish(p);
}
