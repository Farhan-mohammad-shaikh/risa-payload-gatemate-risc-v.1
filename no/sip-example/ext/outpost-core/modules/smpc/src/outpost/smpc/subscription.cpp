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

#include "subscription.h"

outpost::smpc::Subscription* outpost::smpc::Subscription::listOfAllSubscriptions = 0;

outpost::smpc::Subscription::~Subscription()
{
    removeFromList(&Subscription::listOfAllSubscriptions, this);

    // TODO
    releaseAllSubscriptions();
    connectSubscriptionsToTopics();
}

void
outpost::smpc::Subscription::connectSubscriptionsToTopics()
{
    // Reset the lists in the topics
    TopicBase::clearSubscriptions();

    for (Subscription* it = Subscription::listOfAllSubscriptions; it != 0; it = it->getNext())
    {
        it->mNextTopicSubscription = it->mTopic->mSubscriptions;
        it->mTopic->mSubscriptions = it;
    }
}

void
outpost::smpc::Subscription::releaseAllSubscriptions()
{
    for (Subscription* it = Subscription::listOfAllSubscriptions; it != 0; it = it->getNext())
    {
        it->mNextTopicSubscription = 0;
    }

    TopicBase::clearSubscriptions();
}
