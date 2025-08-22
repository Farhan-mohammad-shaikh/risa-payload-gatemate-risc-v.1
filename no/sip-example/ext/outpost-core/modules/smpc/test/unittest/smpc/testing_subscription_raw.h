/*
 * Copyright (c) 2014-2017, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef UNITTEST_SMPC_TESTING_SUBSCRIPTION_RAW_H
#define UNITTEST_SMPC_TESTING_SUBSCRIPTION_RAW_H

#include <outpost/smpc/subscription_raw.h>

namespace unittest
{
namespace smpc
{
class TestingSubscriptionRaw : public outpost::smpc::SubscriptionRaw
{
public:
    using SubscriptionRaw::connectSubscriptionsToTopics;
    using SubscriptionRaw::releaseAllSubscriptions;
};

}  // namespace smpc
}  // namespace unittest

#endif  // UNITTEST_SMPC_TESTING_SUBSCRIPTION_RAW_H
