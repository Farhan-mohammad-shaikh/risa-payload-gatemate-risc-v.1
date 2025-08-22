/*
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

#include "stardundee_spacewire_factory.h"

#include <stdio.h>

#include <iostream>

namespace outpost
{
namespace spw_posix
{
StarDundeeDeviceFactory::StarDundeeDeviceFactory()
{
}

StarDundeeDeviceFactory::~StarDundeeDeviceFactory()
{
}

stardundee::com::starsystem::general::DeviceList StarDundeeDeviceFactory::deviceList;

stardundee::com::starsystem::general::Device*
StarDundeeDeviceFactory::getDevice(DeviceType type, const char* pSerialNumber)
{
    deviceList.Populate();
    stardundee::com::starsystem::general::Device* device = nullptr;

    for (size_t i = 0; i < deviceList.GetDeviceCount() && device == nullptr; i++)
    {
        char serialNumber[stardundee::com::starsystem::general::Device::STR_DEVICE_SERIAL_LEN];
        size_t nameLen = deviceList[i]->GetDeviceSerialNumber(serialNumber);
        if (pSerialNumber == nullptr || 0 == strncmp(serialNumber, pSerialNumber, nameLen - 1))
        {
            stardundee::com::starsystem::deviceconfig::DeviceConfiguration* conf =
                    deviceList[i]->GetDeviceConfiguration();
            stardundee::com::starsystem::deviceconfig::DeviceIdentifierInfo* info =
                    conf->GetDeviceIdentificationInfo();

            if (static_cast<uint8_t>(info->GetChipType()) == static_cast<uint8_t>(type))
            {
                device = deviceList[i];
            }
            delete info;
            delete conf;
        }
    }
    return device;
}

void
StarDundeeDeviceFactory::printDevices()
{
    deviceList.Populate();

    for (size_t i = 0; i < deviceList.GetDeviceCount(); i++)
    {
        char serialNumber[stardundee::com::starsystem::general::Device::STR_DEVICE_SERIAL_LEN + 1];
        size_t nameLen = deviceList[i]->GetDeviceSerialNumber(serialNumber);
        if (nameLen > stardundee::com::starsystem::general::Device::STR_DEVICE_SERIAL_LEN)
        {
            nameLen = stardundee::com::starsystem::general::Device::STR_DEVICE_SERIAL_LEN;
        }
        serialNumber[nameLen] = 0;
        std::cout << "Device:" << serialNumber << std::endl;

        stardundee::com::starsystem::deviceconfig::DeviceConfiguration* conf =
                deviceList[i]->GetDeviceConfiguration();
        stardundee::com::starsystem::deviceconfig::DeviceIdentifierInfo* info =
                conf->GetDeviceIdentificationInfo();

        std::cout << "Type:" << static_cast<int>(info->GetChipType()) << std::endl;
        std::cout << "Config:" << conf << std::endl;

        std::cout << "Link:"
                  << dynamic_cast<stardundee::com::starsystem::deviceconfig::mk2devices::brickmk4::
                                          BrickMk4LinkPort*>(conf->GetPort(2))
                  << std::endl;
        delete info;
        delete conf;
    }
}

}  // namespace spw_posix
}  // namespace outpost
