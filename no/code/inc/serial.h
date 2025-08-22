#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <generated/csr.h>
#include <hw/common.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <irq.h>

enum UARTDevice
{
	UART_LOGGING,
	UART_OBC,
	UART_ICE40,
	UART_DEVICE_COUNT,
};

class Serial
{
public:
	/**
	 * @brief Creates and Initializes the UART Interface
	 */
	Serial(UARTDevice device);

	/**
	 * @brief Enables the Interrupt Service routine during rx events
	 */
	void initInterrupts();

	/**
	 * @brief Resets the Interrupt
	 */
	void resetInterrupt();

	/**
	 * @brief Writes a single byte into the UART Interface, (blocking operation)
	 * 
	 * @param byte the byte thats to be written
	 */
	void write(uint8_t byte);

	/**
	 * @brief Writes a multiple bytes into the UART Interface, (blocking operation)
	 * 
	 * @param bytes the bytes that have to be written
	 * @param length the length of the given array
	 */
	void write(uint8_t* bytes, uint32_t length);

	/**
	 * @brief Reads a single byte from the UART Interface, (blocking operation)
	 * 
	 * @retval the byte received from the UART
	 */
	uint8_t read();

	/**
	 * @brief Reads a single byte from the UART Interface, (non-blocking)
	 * 
	 * @retval the byte received from the UART
	 */
	uint8_t readNonBlocking();

	/**
	 * @brief Tells you how many bytes are pending in the Buffer
	 * 
	 * @retval the amount of bytes in the buffer
	 */
	uint32_t bytesPending();

	/**
	 * @brief Checks if the Buffer of the UART is empty
	 * 
	 * @retval true if its empty, false if not
	 */
	bool isEmpty();

	/**
	 * @brief Writes a whole string into the UART Interface, (blocking operation)
	 */
	void writeString(char* src);

	/**
	 * @brief printf but for the Serial, works the same, uses vsprintf
	 */
	void printf(const char* format, ...);

	void printStatus();

	/**
	 * @brief Keeps all instances that use interrupts
	 */
	static Serial* interrupt_instances[CONFIG_CPU_INTERRUPTS];
	static uint32_t instance_mask;

private:
	/**
	 * @brief Interrupt service routine for the UART Receive signal
	 */
	static void uartIsr();

	uint32_t uart_base_addr;
	uint32_t uart_interrupt;

	/* Registers*/
	static constexpr uint32_t UART_TX_OFFSET 			= 0x00;
	static constexpr uint32_t UART_RX_OFFSET 			= 0x04;
	static constexpr uint32_t UART_CONTROL_OFFSET 		= 0x08;
	static constexpr uint32_t UART_STATUS_OFFSET 		= 0x0C;
	static constexpr uint32_t UART_EV_STATUS_OFFSET 	= 0x10;
	static constexpr uint32_t UART_EV_PENDING_OFFSET 	= 0x14;
	static constexpr uint32_t UART_EV_ENABLE_OFFSET 	= 0x18;

	/* Offsets */
	static constexpr uint32_t DIN_VLD_OFFSET 	= 0;
	static constexpr uint32_t DIN_RDY_OFFSET 	= 0;
	static constexpr uint32_t DOUT_VLD_OFFSET 	= 1;
	static constexpr uint32_t FE_OFFSET 		= 2;
	static constexpr uint32_t PE_OFFSET 		= 3;
	static constexpr uint32_t EV_RX 			= 0;

	/* Must be power of two */
	static constexpr uint32_t UART_BUF_SIZE = 1 << 8; 

	uint8_t uart_buffer[UART_BUF_SIZE] = {0};
	uint8_t write_head = 0;
	uint8_t read_head = 0;
};

#endif /* _SERIAL_H_ */
