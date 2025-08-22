#include "serial.h"

#if 1

uint32_t Serial::instance_mask = 0;
Serial* Serial::interrupt_instances[CONFIG_CPU_INTERRUPTS] = {nullptr};

Serial::Serial(UARTDevice device) : write_head(0), read_head(0)
{
	switch(device)
	{
#ifdef CSR_UART_LOGGING_BASE
		case UART_LOGGING: 
		uart_base_addr = CSR_UART_LOGGING_BASE; 
		uart_interrupt = UART_LOGGING_INTERRUPT;
		break;
#endif
#ifdef CSR_UART_OBC_BASE
		case UART_OBC: 
		uart_base_addr = CSR_UART0_BASE;
		uart_interrupt = UART_INTERRUPT;
		break;
#endif

#ifdef CSR_UART_ICE40_BASE
		case UART_ICE40: 
		uart_base_addr = CSR_UART_ICE40_BASE;
		uart_interrupt = UART_ICE40_INTERRUPT;
		break;
#endif
		default:

		break;
	}
}

void Serial::uartIsr()
{
#if 1
	/* Get all Interrupts that happend */
	uint32_t irqs = irq_pending() & irq_getmask();

	/* Execute their Functions one by one */
	while(irqs)
	{
		const uint32_t irq = __builtin_ctz(irqs);
		
		if(interrupt_instances[irq])
		{
			Serial* irq_source = interrupt_instances[irq];

			irq_source->resetInterrupt();
			irq_source->uart_buffer[irq_source->write_head] = csr_read_simple(irq_source->uart_base_addr + UART_RX_OFFSET);

			/* Write head rolls over after reaching uart_buffer_size*/
			irq_source->write_head = (irq_source->write_head + 1) & (UART_BUF_SIZE - 1);
		}

		irqs &= ~(1 << irq);
	}
#endif
}

void Serial::initInterrupts()
{
	irq_setie(0);

	/* Add to table of UART Interrupts so the ISR Knows what Object to call*/
	interrupt_instances[uart_interrupt] = this;
	instance_mask |= (1 << uart_interrupt);

	/* Clear the Pending Interrupts */
	csr_write_simple(uart_interrupt, uart_base_addr + UART_EV_PENDING_OFFSET);

	/* Enable Interrupts and Attach Function to it */
	csr_write_simple(1 << EV_RX, uart_base_addr + UART_EV_ENABLE_OFFSET);
	irq_attach(uart_interrupt, uartIsr);
	irq_setmask(irq_getmask() | (1 << uart_interrupt));

	irq_setie(1);
}

void Serial::resetInterrupt()
{
	csr_write_simple(csr_read_simple(uart_base_addr + UART_EV_PENDING_OFFSET), uart_base_addr + UART_EV_PENDING_OFFSET);
}

void Serial::write(uint8_t byte)
{
	csr_write_simple(byte, uart_base_addr + UART_TX_OFFSET);
	csr_write_simple(1 << DIN_VLD_OFFSET, uart_base_addr + UART_CONTROL_OFFSET);

	while(!(csr_read_simple(uart_base_addr + UART_STATUS_OFFSET) & (1 << DIN_RDY_OFFSET)));
}

void Serial::write(uint8_t *bytes, uint32_t length)
{
	for(uint32_t i = 0; i < length; i++)
	{
		write(bytes[i]);
	}
}

uint8_t Serial::read()
{
	uint8_t result = 0x00;

	while(isEmpty());

	result = uart_buffer[read_head];
	read_head = (read_head + 1) & (UART_BUF_SIZE - 1);

	return result;
}

uint8_t Serial::readNonBlocking()
{	
	uint8_t result = 0x00;
	static uint32_t timeout = 0;
	while(isEmpty() && timeout < 0x124F80){ //100ms timeout
		__asm__("nop");
		timeout++;
	}

	if(timeout < 0x124F80){
		result = uart_buffer[read_head];
		read_head = (read_head + 1) & (UART_BUF_SIZE - 1);
	}

	return result;
}

uint32_t Serial::bytesPending()
{
	if(write_head >= read_head)
	{
		return write_head - read_head;
	}
	else
	{
		return (UART_BUF_SIZE - read_head) + write_head;
	}
}

bool Serial::isEmpty()
{
	return (read_head == write_head);
}

void Serial::writeString(char* src)
{
	for(uint32_t i = 0; i < strlen(src); i++)
	{
		write(src[i]);
	}
}

void Serial::printf(const char* format, ...)
{
    char buffer[256];

    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    writeString(buffer);
}

void Serial::printStatus()
{
	uint8_t din_rdy 	= csr_read_simple(uart_base_addr + UART_STATUS_OFFSET) & DIN_RDY_OFFSET;
	uint8_t dout_vld 	= csr_read_simple(uart_base_addr + UART_STATUS_OFFSET) & DOUT_VLD_OFFSET;
	uint8_t fe 			= csr_read_simple(uart_base_addr + UART_STATUS_OFFSET) & FE_OFFSET;
	uint8_t pe 			= csr_read_simple(uart_base_addr + UART_STATUS_OFFSET) & PE_OFFSET;

	this->printf( 
	"din_rdy 	= 	%d	\n"
	"dout_vld 	= 	%d 	\n"
	"fe 		= 	%d 	\n"
	"pe 		= 	%d 	\n"
	, din_rdy, dout_vld, fe, pe); 
}

#endif