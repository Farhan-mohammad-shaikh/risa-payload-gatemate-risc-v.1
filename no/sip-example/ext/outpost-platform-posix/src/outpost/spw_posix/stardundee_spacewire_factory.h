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

#ifndef OUTPOST_HAL_POSIX_STARDUNDEE_DEVICE_H_
#define OUTPOST_HAL_POSIX_STARDUNDEE_DEVICE_H_

#include <stdint.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <device_list.hpp>
#pragma GCC diagnostic pop

#include "spacewire_brick_mk3.h"
#include "stardundee_spacewire.h"

namespace outpost
{
namespace spw_posix
{
class StarDundeeDeviceFactory
{
public:
    StarDundeeDeviceFactory();
    virtual ~StarDundeeDeviceFactory();

    /**
     * If using nullptr as serial number will take the first of the type it finds
     */
    static stardundee::com::starsystem::general::Device*
    getDevice(DeviceType type, const char* serialNumber);

    static void
    printDevices();

private:
    static stardundee::com::starsystem::general::DeviceList deviceList;
};

}  // namespace spw_posix
}  // namespace outpost

#endif /* OUTPOST_HAL_POSIX_STARDUNDEE_DEVICE_H_ */
