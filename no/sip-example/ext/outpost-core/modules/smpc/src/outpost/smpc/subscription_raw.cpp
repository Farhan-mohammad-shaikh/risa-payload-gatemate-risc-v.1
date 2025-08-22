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

#include "subscription_raw.h"

outpost::smpc::SubscriptionRaw* outpost::smpc::SubscriptionRaw::listOfAllSubscriptions = 0;

outpost::smpc::SubscriptionRaw::~SubscriptionRaw()
{
    removeFromList(&SubscriptionRaw::listOfAllSubscriptions, this);

    releaseAllSubscriptions();
    connectSubscriptionsToTopics();
}

void
outpost::smpc::SubscriptionRaw::connectSubscriptionsToTopics()
{
    TopicRaw::clearSubscriptions();

    for (SubscriptionRaw* it = listOfAllSubscriptions; it != 0; it = it->getNext())
    {
        it->mNextTopicSubscription = it->mTopic->mSubscriptions;
        it->mTopic->mSubscriptions = it;
    }
}

void
outpost::smpc::SubscriptionRaw::releaseAllSubscriptions()
{
    for (SubscriptionRaw* it = listOfAllSubscriptions; it != 0; it = it->getNext())
    {
        it->mNextTopicSubscription = 0;
    }

    TopicRaw::clearSubscriptions();
}
