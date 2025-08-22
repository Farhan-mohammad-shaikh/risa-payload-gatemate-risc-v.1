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

#ifndef OUTPOST_SMPC_SUBSCRIBER_H
#define OUTPOST_SMPC_SUBSCRIBER_H

namespace outpost
{
namespace smpc
{
/**
 * Empty base class for every class that should receive data from
 * a topic.
 *
 * This class is needed to provide a common base class to cast to when
 * using the member functions in the Subscriber class.
 *
 * \see Subscriber
 * \see Topic
 *
 * \ingroup smpc
 * \author  Fabian Greif
 */
class Subscriber
{
};

}  // namespace smpc
}  // namespace outpost

#endif
