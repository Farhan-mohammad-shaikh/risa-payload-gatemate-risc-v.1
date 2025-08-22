/*
 * Copyright (c) 2014-2017, 2019, Fabian Greif
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

/**
 * \file
 * \author Fabian Greif
 *
 * \copyright German Aerospace Center (DLR)
 *
 * \brief FreeRTOS example for the STM32F4Discovery Board.
 *
 * Uses the xpcc library to provide the STM32F4xx hardware drivers.
 */

#include <xpcc/architecture.hpp>
#include <xpcc/processing/rtos.hpp>

#include <outpost/rtos/failure_handler.h>
#include <outpost/rtos/queue.h>
#include <outpost/rtos/thread.h>

using namespace xpcc::stm32;

typedef GpioOutputD13 LedOrange;  // User LED 3
typedef GpioOutputD12 LedGreen;   // User LED 4
typedef GpioOutputD14 LedRed;     // User LED 5
typedef GpioOutputD15 LedBlue;    // User LED 6

typedef GpioOutputA9 VBusPresent;      // green LED (LD7)
typedef GpioOutputD5 VBusOvercurrent;  // red LED (LD8)
typedef GpioInputA0 Button;
typedef GpioOutputA8 ClockOut;
typedef GpioOutputC9 SystemClockOut;

/// STM32F4 running at 168MHz (USB Clock at 48MHz) generated from the
/// external on-board 8MHz crystal
typedef SystemClock<Pll<ExternalCrystal<MHz8>, MHz168, MHz48>> DefaultSystemClock;

outpost::rtos::Queue<uint32_t> queue(100);

static void
failureHandler(outpost::rtos::FailureCode code)
{
    (void) code;

    LedRed::set();

    while (1)
    {
        // wait forever
    }
}

int
main(void)
{
    DefaultSystemClock::enable();

    LedOrange::setOutput(xpcc::Gpio::Low);
    LedGreen::setOutput(xpcc::Gpio::Low);
    LedRed::setOutput(xpcc::Gpio::Low);
    LedBlue::setOutput(xpcc::Gpio::Low);

    outpost::rtos::FailureHandler::setFailureHandlerFunction(&failureHandler);

    LedGreen::set();

    queue.send(1245);

    uint32_t value;
    queue.receive(value, outpost::time::Seconds(1));

    xpcc::rtos::Scheduler::schedule();
    LedRed::set();
}
