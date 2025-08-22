/*
 * Copyright (c) 2014-2017, Fabian Greif
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2022, Tobias Pfeffer
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef UNITTEST_SMPC_TOPIC_LOGGER_H
#define UNITTEST_SMPC_TOPIC_LOGGER_H

#include <outpost/smpc/subscription.h>
#include <outpost/smpc/topic.h>

#include <unittest/smpc/testing_subscription.h>

#include <list>

namespace unittest
{
namespace smpc
{
template <typename T>
class TopicLogger
{
public:
    explicit TopicLogger(outpost::smpc::Topic<T>& topic);

    void
    clear();

    const T&
    getNext() const;

    void
    dropNext();

    bool
    isEmpty() const;

private:
    typedef typename std::remove_const<T>::type NonConstType;

    void
    onReceive(T* item);

    outpost::smpc::Subscription mSubscription;
    std::list<NonConstType> mItems;
};

}  // namespace smpc
}  // namespace unittest

#include "topic_logger_impl.h"

#endif
