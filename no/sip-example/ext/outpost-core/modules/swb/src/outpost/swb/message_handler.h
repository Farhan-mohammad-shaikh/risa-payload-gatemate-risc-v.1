/*
 * Copyright (c) 2021, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SWB_MESSAGE_HANDLER_H_
#define OUTPOST_SWB_MESSAGE_HANDLER_H_

namespace unittest
{
namespace swb
{
class TestingSoftwareBus;
}
}  // namespace unittest

namespace outpost
{
namespace swb
{
/**
 * For unit testing, class allows to handle single or all messages
 * queued to the bus distributer without needing to start the thread;
 * hence preventing race condition in the unit test.
 *
 * Putting this as its own file to prevent relations to other header files,
 * especially from default/outpost/parameter/support.h as this is not in
 * the unit test path and different projects may have different version.
 */
class MessageHandler
{
    friend class unittest::swb::TestingSoftwareBus;

protected:
    MessageHandler() = default;
    virtual ~MessageHandler() = default;

    /**
     * Will handle all messages queued,
     * use in case no running thread is wanted in order to
     * remove race conditions
     */
    virtual void
    handleAllMessages() = 0;

    /**
     * Will handle a single message if queued
     * @return true if a message was handled, false otherwise
     */
    virtual bool
    handleSingleMessage() = 0;
};

}  // namespace swb
}  // namespace outpost

#endif /* MODULES_SWB_SRC_OUTPOST_SWB_MESSAGE_HANDLER_H_ */
