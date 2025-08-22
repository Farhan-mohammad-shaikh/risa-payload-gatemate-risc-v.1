/*
 * Copyright (c) 2021, Jan-Gerd Mess
 * Copyright (c) 2021, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SPW_POSIX_SPACEWIRE_BRICK_MK3_H_
#define OUTPOST_SPW_POSIX_SPACEWIRE_BRICK_MK3_H_

#include "stardundee_spacewire.h"

namespace outpost
{
namespace spw_posix
{
using stardundee::com::starsystem::deviceconfig::mk2devices::brickmk3::BrickMk3LinkPort;

class SpacewireBrickMk3 : public StardundeeSpacewire
{
public:
    SpacewireBrickMk3(const char* serialNumber);
    virtual ~SpacewireBrickMk3();

    virtual bool
    isValid() override;

    virtual bool
    configure() override;

    virtual void
    close() override;

    virtual bool
    up(outpost::time::Duration timeout) override;

    virtual void
    down(outpost::time::Duration timeout = outpost::time::Duration::zero()) override;

    virtual bool
    isUp() override;

    virtual void
    flushReceiveBuffer() override;

    void
    updateLinkPort();

protected:
    bool
    configureLinkSpeed();

    BrickMk3LinkPort*
    getLinkPort(uint8_t port);

    inline void
    freeLinkPort(BrickMk3LinkPort* port)
    {
        if (port != nullptr)
        {
            delete port;
        }
    }
};

}  // namespace spw_posix
}  // namespace outpost

#endif /* EXT_OUTPOST_PLATFORM_POSIX_SRC_OUTPOST_SPW_POSIX_SPACEWIRE_BRICK_MK3_H_ \
        */
