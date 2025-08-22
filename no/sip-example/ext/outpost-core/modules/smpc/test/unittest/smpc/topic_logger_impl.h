/*
 * Copyright (c) 2015-2017, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SMPC_TOPIC_LOGGER_IMPL_H
#define OUTPOST_SMPC_TOPIC_LOGGER_IMPL_H

#include "topic_logger.h"

template <typename T>
unittest::smpc::TopicLogger<T>::TopicLogger(outpost::smpc::Topic<T>& topic) :
    mSubscription(topic, this, &TopicLogger::onReceive)
{
}

template <typename T>
void
unittest::smpc::TopicLogger<T>::clear()
{
    mItems.clear();
}

template <typename T>
const T&
unittest::smpc::TopicLogger<T>::getNext() const
{
    return mItems.front();
}

template <typename T>
void
unittest::smpc::TopicLogger<T>::dropNext()
{
    mItems.pop_front();
}

template <typename T>
bool
unittest::smpc::TopicLogger<T>::isEmpty() const
{
    return mItems.empty();
}

template <typename T>
void
unittest::smpc::TopicLogger<T>::onReceive(T* item)
{
    mItems.push_back(*item);
}

#endif
