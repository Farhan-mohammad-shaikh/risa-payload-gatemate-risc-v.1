/*
 * Copyright (c) 2020, Jan-Gerd Mess
 * Copyright (c) 2021, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SWB_TESTING_SOFTWARE_BUS_H_
#define OUTPOST_SWB_TESTING_SOFTWARE_BUS_H_

#include <outpost/swb/message_handler.h>

namespace unittest
{
namespace swb
{
class TestingSoftwareBus
{
public:
    explicit TestingSoftwareBus(outpost::swb::MessageHandler& bus);

    bool
    singleMessage();

    void
    allMessages();

private:
    outpost::swb::MessageHandler& mBus;
};

}  // namespace swb
}  // namespace unittest

#endif /* EXT_OUTPOST_CORE_MODULES_SWB_TEST_OUTPOST_SWB_TESTING_SOFTWARE_BUS_H_ */
