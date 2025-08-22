/*
 * Copyright (c) 2017, Jan Sommer
 * Copyright (c) 2017, Fabian Greif
 * Copyright (c) 2021, Jan-Gerd Mess
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2023, Mess, Jan-Gerd
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "spacewire_brick_mk4.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <port.hpp>
#pragma GCC diagnostic pop
#include <iostream>

namespace outpost
{
namespace spw_posix
{
using namespace hal;

SpacewireBrickMk4::SpacewireBrickMk4(const char* serialNumber) :
    StardundeeSpacewire(serialNumber, DeviceType::BRICK_MK4)
{
}

bool
SpacewireBrickMk4::isValid()
{
    bool result = StardundeeSpacewire::isValid();
    return result && mConfiguration != nullptr;
}

bool
SpacewireBrickMk4::configure()
{
    return configureLinkSpeed();
}

bool
SpacewireBrickMk4::configureLinkSpeed()
{
    bool result = false;
    BrickMk4LinkPort* linkPort = getLinkPort(mPort);

    stardundee::com::starsystem::deviceconfig::mk2devices::pcie::ClockRateParams params;
    if (linkPort != nullptr)
    {
        if (1 == linkPort->GetTransmitClock(params))
        {
            switch (mLinkSpeed)
            {
                case LinkSpeed::MBPS_20:
                    params.SetDivisor(20);
                    params.SetMultiplier(2);
                    break;
                case LinkSpeed::MBPS_50:
                    params.SetDivisor(8);
                    params.SetMultiplier(2);
                    break;
                case LinkSpeed::MBPS_100:
                    params.SetDivisor(4);
                    params.SetMultiplier(2);
                    break;
                case LinkSpeed::MBPS_200:
                    params.SetDivisor(2);
                    params.SetMultiplier(2);
                    break;
                case LinkSpeed::MBPS_10:
                default:
                    params.SetDivisor(40);
                    params.SetMultiplier(2);
                    break;
            }
            if (1 == linkPort->SetTransmitClock(params))
            {
                result = true;
            }
            else
            {
                std::cout << "Set LinkSpeed failed" << std::endl;
            }
        }
        else
        {
            std::cout << "Failed get TransmitCLock" << std::endl;
        }
    }
    else
    {
        std::cout << "Link Port nullptr" << std::endl;
    }
    freeLinkPort(linkPort);
    return result;
}

void
SpacewireBrickMk4::close()
{
    down();
    if (mChannel.IsOpen())
    {
        mChannel.CloseChannel();
    }
}

bool
SpacewireBrickMk4::up(outpost::time::Duration timeout)
{
    bool result = false;
    BrickMk4LinkPort* linkPort = getLinkPort(mPort);

    if (linkPort != nullptr)
    {
        outpost::time::Duration timeWaited = outpost::time::Duration::zero();
        outpost::time::Duration retryTime = outpost::time::Milliseconds(100);
        while (!isUp() && timeWaited <= timeout)
        {
            if (1 == linkPort->StartLink())
            {
                result = true;
            }
            else if (timeout > timeWaited)
            {
                if (timeout != outpost::time::Duration::zero())
                {
                    outpost::rtos::Thread::sleep(retryTime);
                }
            }
            timeWaited += retryTime;
        }
        result = isUp();
    }
    else
    {
        result = false;
    }
    freeLinkPort(linkPort);
    return result;
}

void
SpacewireBrickMk4::down(outpost::time::Duration)
{
    BrickMk4LinkPort* linkPort = getLinkPort(mPort);
    if (linkPort != nullptr)
    {
        linkPort->StopLink();
        freeLinkPort(linkPort);
    }
}

bool
SpacewireBrickMk4::isUp()
{
    bool result = false;
    BrickMk4LinkPort* linkPort = getLinkPort(mPort);
    if (isValid() && linkPort != nullptr)
    {
        stardundee::com::starsystem::deviceconfig::LinkStatus* status =
                dynamic_cast<stardundee::com::starsystem::deviceconfig::LinkStatus*>(
                        linkPort->GetStatus());
        if (status != nullptr)
        {
            result = (status->GetLinkState()
                      == stardundee::com::starsystem::deviceconfig::LinkStatus::LINK_STATE_RUN);
            delete status;
        }
    }
    freeLinkPort(linkPort);
    return result;
}

void
SpacewireBrickMk4::flushReceiveBuffer()
{
}

SpacewireBrickMk4::~SpacewireBrickMk4()
{
    this->close();
    delete mConfiguration;
}

stardundee::com::starsystem::deviceconfig::mk2devices::brickmk4::BrickMk4LinkPort*
SpacewireBrickMk4::getLinkPort(uint8_t port)
{
    if (isValid())
    {
        return dynamic_cast<
                stardundee::com::starsystem::deviceconfig::mk2devices::brickmk4::BrickMk4LinkPort*>(
                mConfiguration->GetPort(port));
    }
    return nullptr;
}

}  // namespace spw_posix
}  // namespace outpost
