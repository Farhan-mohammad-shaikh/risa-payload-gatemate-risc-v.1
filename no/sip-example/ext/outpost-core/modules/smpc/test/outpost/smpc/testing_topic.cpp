/*
 * Copyright (c) 2014-2017, Fabian Greif
 * Copyright (c) 2022, Tobias Pfeffer
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/smpc/subscription.h>
#include <outpost/smpc/topic.h>

#include <stdio.h>

namespace outpost
{
namespace smpc
{
class TestingTopicBase
{
public:
    void
    dumpConnectedSubscriptions(const outpost::smpc::TopicBase& base) const;
};

using namespace outpost::smpc;

void
TestingTopicBase::dumpConnectedSubscriptions(const outpost::smpc::TopicBase& base) const
{
    printf("topic %p\n", reinterpret_cast<const void*>(this));

    for (Subscription* topic = base.mSubscriptions; topic != 0;
         topic = topic->mNextTopicSubscription)
    {
        printf("- %p\n", reinterpret_cast<void*>(topic));
    }
}

}  // namespace smpc
}  // namespace outpost
