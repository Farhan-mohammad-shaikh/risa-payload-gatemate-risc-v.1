/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#include <rtems.h>

// For device driver prototypes
#include <bsp.h>

// Configuration information
// #define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

// ----------------------------------------------------------------------------
// Tasks
#define CONFIGURE_MAXIMUM_TASKS 4
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_EXTRA_TASK_STACKS (3 * RTEMS_MINIMUM_STACK_SIZE)

// Configure start task
#define CONFIGURE_INIT_TASK_ENTRY_POINT task_system_init

#ifdef __cplusplus
extern "C"
{
#endif
    // Forward declaration needed for task table
    rtems_task task_system_init(rtems_task_argument);
#ifdef __cplusplus
}
#endif

extern const char* bsp_boot_cmdline;
#define CONFIGURE_INIT_TASK_ARGUMENTS ((rtems_task_argument) & bsp_boot_cmdline)

#define CONFIGURE_MICROSECONDS_PER_TICK 1000
#define CONFIGURE_TICKS_PER_TIMESLICE 20

// ----------------------------------------------------------------------------
// Mutex/Semaphores
// C++ requires at least one Semaphore for the constructor calls and the
// initialization of static member variables.
#define CONFIGURE_MAXIMUM_SEMAPHORES 5

// ----------------------------------------------------------------------------
// Timer support
#define CONFIGURE_MAXIMUM_TIMERS 4
#define CONFIGURE_MAXIMUM_PERIODS 4

#define CONFIGURE_MAXIMUM_DRIVERS 4

// ----------------------------------------------------------------------------
#include <rtems/confdefs.h>

// Add Timer and UART Driver
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GPTIMER

#endif  // SYSTEM_H
