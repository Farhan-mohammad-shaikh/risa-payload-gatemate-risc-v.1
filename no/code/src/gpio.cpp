#include "gpio.h"
#include <generated/csr.h>

// Define the GPIO address lookup tables [from generated CSRs]
// Don't Change
const uint32_t GPIO_OE_ADDR[GPIO_PIN_COUNT] = {
    CSR_ICE40_IO_VIO_0_OE_ADDR, 
    CSR_ICE40_IO_VIO_1_OE_ADDR,
    CSR_ICE40_IO_VIO_2_OE_ADDR,
    CSR_ICE40_IO_VIO_3_OE_ADDR,
    CSR_ICE40_IO_VIO_4_OE_ADDR,
    CSR_ICE40_IO_VIO_5_OE_ADDR,
    CSR_ICE40_IO_VCORE_0_OE_ADDR,
    CSR_ICE40_IO_VCORE_1_OE_ADDR,
    CSR_ICE40_IO_VCORE_2_OE_ADDR,
    // CSR_ICE40_IO_VCORE_3_OE_ADDR,
    CSR_ICE40_IO_VCORE_4_OE_ADDR,
    // CSR_ICE40_IO_VCORE_5_OE_ADDR
    CSR_GATEMATE_DEBUG_3_OE_ADDR,
    CSR_GATEMATE_DEBUG_4_OE_ADDR,
    CSR_GATEMATE_DEBUG_5_OE_ADDR
};

const uint32_t GPIO_IN_ADDR[GPIO_PIN_COUNT] = {
    CSR_ICE40_IO_VIO_0_IN_ADDR,
    CSR_ICE40_IO_VIO_1_IN_ADDR,
    CSR_ICE40_IO_VIO_2_IN_ADDR,
    CSR_ICE40_IO_VIO_3_IN_ADDR,
    CSR_ICE40_IO_VIO_4_IN_ADDR,
    CSR_ICE40_IO_VIO_5_IN_ADDR,
    CSR_ICE40_IO_VCORE_0_IN_ADDR,
    CSR_ICE40_IO_VCORE_1_IN_ADDR,
    CSR_ICE40_IO_VCORE_2_IN_ADDR,
    // CSR_ICE40_IO_VCORE_3_IN_ADDR,
    CSR_ICE40_IO_VCORE_4_IN_ADDR,
    // CSR_ICE40_IO_VCORE_5_IN_ADDR
    CSR_GATEMATE_DEBUG_3_IN_ADDR,
    CSR_GATEMATE_DEBUG_4_IN_ADDR,
    CSR_GATEMATE_DEBUG_5_IN_ADDR
};

const uint32_t GPIO_OUT_ADDR[GPIO_PIN_COUNT] = {
    CSR_ICE40_IO_VIO_0_OUT_ADDR,
    CSR_ICE40_IO_VIO_1_OUT_ADDR,
    CSR_ICE40_IO_VIO_2_OUT_ADDR,
    CSR_ICE40_IO_VIO_3_OUT_ADDR,
    CSR_ICE40_IO_VIO_4_OUT_ADDR,
    CSR_ICE40_IO_VIO_5_OUT_ADDR,
    CSR_ICE40_IO_VCORE_0_OUT_ADDR,
    CSR_ICE40_IO_VCORE_1_OUT_ADDR,
    CSR_ICE40_IO_VCORE_2_OUT_ADDR,
    // CSR_ICE40_IO_VCORE_3_OUT_ADDR,
    CSR_ICE40_IO_VCORE_4_OUT_ADDR,
    // CSR_ICE40_IO_VCORE_5_OUT_ADDR
    CSR_GATEMATE_DEBUG_3_OUT_ADDR,
    CSR_GATEMATE_DEBUG_4_OUT_ADDR,
    CSR_GATEMATE_DEBUG_5_OUT_ADDR 
};

// gpioSetup function to set GPIO direction as input or output
/**
 * @brief Configures the direction of a GPIO pin as input or output.
 *
 * @param pin The GPIO pin to configure.
 * @param dir The direction to set:
 *            - `GPIO_INPUT` for input
 *            - `GPIO_OUTPUT` for output
 */
void gpioSetup(enum GPIO pin, uint32_t dir) {
    if (pin >= GPIO_PIN_COUNT) {
        // Error Handler Placeholder
        return;
    }
    // Write the value to the correct GPIO output enable address based on direction
    csr_write_simple(dir, GPIO_OE_ADDR[pin]);
}

/**
 * @brief Reads the current value of a GPIO pin.
 *
 * @param pin The GPIO pin to read from.
 * @return uint32_t The value of the pin ('GPIO_LOW' or 'GPIO_HIGH'). Returns 0 if the pin is invalid.
 */
uint32_t gpioRead(enum GPIO pin) {
    if (pin >= GPIO_PIN_COUNT) {
        // Error Handler Placeholder
        return 0;  // Return a default value on error
    }
    return csr_read_simple(GPIO_IN_ADDR[pin]);
}

/**
 * @brief Writes a value to a GPIO pin.
 *
 * @param pin The GPIO pin to write to.
 * @param value The value to write (0 or 1).
 */
void gpioWrite(enum GPIO pin, uint32_t value) {
    if (pin >= GPIO_PIN_COUNT) {
        // Error Handler Placeholder
        return;
    }
    csr_write_simple(value, GPIO_OUT_ADDR[pin]);
}

/**
 * @brief Toggles the current value of a GPIO pin.
 *
 * @param pin The GPIO pin to toggle.
 */
void gpioToggle(enum GPIO pin) {
    uint32_t current_val = csr_read_simple(GPIO_OUT_ADDR[pin]);
    csr_write_simple((current_val ^ 0x1), GPIO_OUT_ADDR[pin]);
}
