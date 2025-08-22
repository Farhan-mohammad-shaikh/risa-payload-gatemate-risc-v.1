/*
 * Copyright (c) 2023, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/hal_posix/can_bus_port.h>

#include <iostream>
#include <sstream>

#include <cassert>

using namespace outpost::posix;
using namespace outpost::hal;

/**
 * Before you can run the example you have to execute the following commands:
 * 
 * sudo apt-get install can-utils
 * - set up vcan0
 *   sudo ip link add dev vcan0 type vcan
 *   sudo ip link set vcan0 up
 * 
 * - set up vcan1
 *   sudo ip link add dev vcan1 type vcan
 *   sudo ip link set vcan1 up
 * 
 * - connect vcan0 and vcan1
 *   sudo modprobe can-gw
 *   sudo cangw -A -s vcan0 -d vcan1 -e
 *   sudo cangw -A -s vcan1 -d vcan0 -e
 */


static std::string
canFrameToString(outpost::hal::CanBus::CanFrame frame)
{
    std::ostringstream oss;
    oss << "  ID: 0x" << std::hex << frame.getId() << std::dec << std::endl;
    oss << "  Data Length: " << static_cast<int>(frame.getLength()) << std::endl;
    oss << "  Data: ";
    for (int i = 0; i < frame.getLength(); ++i)
    {
        oss << std::hex << static_cast<int>(frame.getData()[i]) << " ";
    }
    return oss.str();
}

// ---------------------------------------------------------------------------
int main(int /* argc */, char const ** /* argv */)
{
    // the reader must read first otherwise the data is lost
    std::cout << "open vcan1 to read ... ";
    CanBusPort reader("vcan1");
    auto ret = reader.open();
    if (ret == CanBus::ReturnCode::success)
    {
        std::cout << "success" << std::endl;
    }
    else
    {
        std::cout << "failure" << std::endl;
    }

    std::cout << "open vcan0 to write ... ";
    CanBusPort writer("vcan0");
    ret = writer.open();
    if (ret == CanBus::ReturnCode::success)
    {
        std::cout << "success" << std::endl;
    }
    else
    {
        std::cout << "failure" << std::endl;
    }
    assert(ret == CanBus::ReturnCode::success);

    uint8_t buffer[] = {0x2A, 0x2B};
    outpost::hal::CanBus::CanFrame frame(0U, outpost::asSlice(buffer));

    ret = writer.write(frame);
    if (ret == CanBus::ReturnCode::success)
    {
        std::cout << "send: " << std::endl << canFrameToString(frame) << std::endl;
    }
    else
    {
        std::cout << "failed to write can bus!" << std::endl;
    }

    writer.close();

    CanBus::CanFrame inputFrame = CanBus::CanFrame::makeRequestFrame(0U, 2, false);
    ret = reader.read(inputFrame);

    if (ret == CanBus::ReturnCode::success)
    {
        std::cout << "received: " << std::endl << canFrameToString(inputFrame) << std::endl;
    }
    else
    {
        std::cout << "failed to read can bus!" << std::endl;
    }

    reader.close();

    assert(inputFrame.getData()[0] == (0x2A));
    assert(inputFrame.getData()[1] == (0x2B));

    return 0;
}