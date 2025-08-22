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

#ifndef UNITTEST_SMPC_TESTING_SUBSCRIPTION_H
#define UNITTEST_SMPC_TESTING_SUBSCRIPTION_H

#include <outpost/smpc/subscription.h>

namespace unittest
{
namespace smpc
{
class TestingSubscription : public outpost::smpc::Subscription
{
public:
    using Subscription::connectSubscriptionsToTopics;
    using Subscription::releaseAllSubscriptions;
};

}  // namespace smpc
}  // namespace unittest

#endif  // UNITTEST_SMPC_TESTING_SUBSCRIPTION_H
