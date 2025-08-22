#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>  

// Define GPIO input/output direction constants
#define GPIO_INPUT  0x00
#define GPIO_OUTPUT 0x01

// Define GPIO logic level constants
#define GPIO_LOW  0x0
#define GPIO_HIGH 0x1

// Define the total GPIO pin count 
#define GPIO_PIN_COUNT 14 

// Enum for GPIO pins
enum GPIO {
    ice40_io_vio_0, 
    ice40_io_vio_1, 
    ice40_io_vio_2,
    ice40_io_vio_3,
    ice40_io_vio_4,
    ice40_io_vio_5,
    ice40_io_vcore_0,
    ice40_io_vcore_1,
    ice40_io_vcore_2,
    //ice40_io_vcore_3,
    ice40_io_vcore_4,
    //,ice40_io_vcore_5
    gatemate_debug_3,
    gatemate_debug_4,
    gatemate_debug_5
};

// Lookup tables for GPIO addresses
extern const uint32_t GPIO_OE_ADDR[GPIO_PIN_COUNT];
extern const uint32_t GPIO_IN_ADDR[GPIO_PIN_COUNT];
extern const uint32_t GPIO_OUT_ADDR[GPIO_PIN_COUNT];

// Function declarations
void gpioSetup(enum GPIO pin, uint32_t dir);
uint32_t gpioRead(enum GPIO pin);
void gpioWrite(enum GPIO pin, uint32_t value);
void gpioToggle(enum GPIO pin);

#endif  // GPIO_H
