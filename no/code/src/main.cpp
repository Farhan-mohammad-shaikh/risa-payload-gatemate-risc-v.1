#include <irq.h>
#include <generated/csr.h>
#include "timer.h"
#include "serial.h"
#include "i2c.h"
#include "delay.h"
#include "spi.h"
#include "mx25r6435f.h"
#include "ice40prog.h"
#include "logging.h"
#include "sensorcontext.h"
#include "experimentmanager.h"
#include "ledCounterExperiment.h"
#include "sip_handler.h"
#include "memorycontext.h"

#include "riscvMatrixExperiment.h"
#include "uvVminPropExperiment.h"
#include "isfdExperiment.h"
#include "ice40FlashExperiment.h"

int main(void)
{	
	leds_out_write(0x01);

	/* Setup the Main Timer*/
	Timer timer0(TimerID::TIMER0);
	timer0.setUpperLimit(0xFFFFFFFF);
	timer0.setTime(0);
	timer0.start();

	leds_out_write(0x02);
	Serial log_serial(UARTDevice::UART_LOGGING);
	leds_out_write(0x03);
	
	/* Tell the universal delay code which timer to use*/
	delayUseTimer(&timer0);
	setupLogging(&log_serial);
	LOGINFO("HALLO");
	leds_out_write(0x04);
	
	log_serial.initInterrupts();
	leds_out_write(0x05);
	delayUS(1000000);
	leds_out_write(0x06);

	/* All Storage devices are encapsulated here */
	MemoryContext memory;
	memory.setupMemories();
	LOGINFO("Memories initialized");

	/* All Sensors are encapsulated here */
	SensorContext sensors;
	sensors.setupSensors();
	LOGINFO("Sensors initialized");

	/* ICE40 Programming */
	SPI ice40_spi(SPIDevice::ICE40);
	ice40_spi.init(1, 0);
	ICE40PROG ice40prog(memory.flash, ice40_spi);
	/*
	sensors.dac.setOutputVoltagerange(MAX_2V5);
	sensors.dac.setVoltage(1200);
	sensors.enableICE40OSC(true);
	sensors.enableICE40VCORE(true);
	sensors.enableICE40VIO(true);
	ice40prog.programm(CONFIG_OFFSETS::BLINKY_TEST);
	*/

	/* ICE40 UART */
	Serial iceUART(UARTDevice::UART_ICE40);
	iceUART.initInterrupts();

	/* Create experiments */
	//LedCounterExperiment experiment0(sensors, ice40prog, memory, iceUART);
	RiscvMatrixExperiment experiment1(sensors, ice40prog, memory, iceUART);
	UvVminPropExperiment experiment2(sensors, ice40prog, memory, iceUART);
	ISFDExperiment experiment3(sensors, ice40prog, memory, iceUART);
	ICE40FlashExperiment experiment4(sensors, ice40prog, memory, iceUART);

	/* Create Experiment manager*/
	ExperimentManager manager;
	uint8_t current_test = 0;
	manager.startExperiment(&experiment4);

	SIPHandler sip_handler(log_serial);

	SIPCommand command;
	leds_out_write(0x01);
	while(1)
	{ 
		bool exp_retval = manager.runCurrentExperiment();
		bool sip_retval = sip_handler.run(&command);

		if(exp_retval)
		{
			LOGINFO("Experiment is finished");
		}

		if(sip_retval)
		{
			SIPReponse response;
			static bool write_active = false;
			static uint32_t write_addr = 0;
			static bool write_flash = false;
			static uint32_t write_len = 0;
			static uint32_t write_index = 0;
			//command.printAsLog();
			switch(command.getFunctionCode())
			{
				case Command::RISA_INIT_COMMAND_ID:
				{
					/* OBC Asks us if we are Ready, we say ACK everytime currently */
					sip_handler.sendAck(command.getSequenceNum());
					break;
				}
				case Command::SHUTDOWN_COMMAND_ID:
				{
					/* What to do during shutdown ? */
					sip_handler.sendAck(command.getSequenceNum());
					break;
				}
				case Command::TEST_STATUS_COMMAND_ID:
				{
					uint8_t asked_id = command.getData()[0];

					if(asked_id != current_test)
					{
						/* What to do if this happens ?*/
					}

					uint8_t current_state = manager.cur_state;
					uint8_t test_data_size = 0;

					uint8_t responsearr[] = {current_state, test_data_size};
					response.build(command.getSequenceNum(), POWERED_ON, Response::TEST_STATUS_RESPONSE_ID, 
					responsearr, sizeof(responsearr));

					sip_handler.sendResponse(response);
					break;
				}
				case Command::TEST_START_COMMAND_ID:
				{
					uint8_t test_id = command.getData()[0];
					uint16_t test_duration = command.getData()[1] | command.getData()[2] << 8;
					uint16_t test_size = command.getData()[3] | command.getData()[4] << 8;

					/* Test parameter kann man weiterführen wie man will */
					uint16_t test_param = command.getData()[5] | command.getData()[6] << 8;

					switch(test_id)
					{
						case 0:
						{
							manager.startExperiment(&experiment1);
							break;
						}

						default:
						{
							sip_handler.sendNack(command.getSequenceNum());
							break;
						}
					}

					sip_handler.sendAck(command.getSequenceNum());
					break;
				}
				case Command::TESTDATA_COMMAND_ID:
				{
					uint8_t test_id = command.getData()[0];
					uint16_t test_size = command.getData()[1] | command.getData()[2] << 8;

					response.build(command.getSequenceNum(), POWERED_ON, Response::TESTDATA_RESPONSE_ID, (uint8_t*)memory.hyperram, test_size);
					sip_handler.sendResponse(response);
					break;
				}
				case Command::HOUSEKEEPINGDATA_COMMAND_ID:
				{
					/* Whats housekeeping data */
					break;
				}
				case Command::MEMORY_WRITE_INIT_COMMAND_ID:
				{
					if(!write_active)
					{
						/* First Byte is the Memory Type */
						write_flash = command.getData()[0];
						/* Next 3 Bytes are Memory Address */
						write_addr = command.getData()[1] | command.getData()[2] << 8 | command.getData()[3] << 16; 
						/* Next 3 Bytes is Length, maximum ? */
						write_len = command.getData()[4] | command.getData()[5] << 8 | command.getData()[6] << 16;

						if(write_flash)
						{
							LOGINFO("Starting write to Flash address %d, with length %d", write_addr, write_len);
						}
						else
						{
							LOGINFO("Starting write to Ram address %d, with length %d", write_addr, write_len);
						}
						
						/* We are now actively in writing mode */
						if(write_flash)
						{
							/* Gotta erase the flash before writing it */
							if(write_len < SECTOR_SIZE)
							{
								memory.flash.eraseSector(write_addr);
							}
							else if(write_len >= SECTOR_SIZE && write_len < BLOCK32K_SIZE)
							{
								memory.flash.eraseBlock32k(write_addr);
							}
							else if(write_len >= BLOCK32K_SIZE)
							{
								memory.flash.eraseBlock64K(write_addr);
							}
							else if(write_len >= BLOCK64K_SIZE)
							{
								uint8_t num_deletes = write_len / BLOCK64K_SIZE;
								for(uint32_t i = 0; i < num_deletes; i++)
								{
									memory.flash.eraseBlock64K(write_addr + (i*BLOCK64K_SIZE));
								}
							}
						}
						write_active = true;
						write_index = 0;
						sip_handler.sendAck(command.getSequenceNum());
					}
					else
					{
						/* Already busy in writing mode */
						sip_handler.sendNack(command.getSequenceNum());
						write_active = false;
					}
					break;
				}
				case Command::MEMORY_WRITE_DATA_COMMAND_ID:
				{
					LOGINFO("write_active: %d", write_active);
					LOGINFO("write_len: %d", write_len);
					LOGINFO("write_index: %d", write_index);
					if(write_active)
					{
						/* First 2 bytes indicate length */
						uint16_t len = command.getData()[0] | command.getData()[1] << 8;
						LOGINFO("len: %d", len);
						/* Maximum size per write is 256 Bytes*/
						if(len > 256) { sip_handler.sendNack(command.getSequenceNum()); write_active = false; break; }

						if(len > (write_len - write_index))
						{
							/* Sending too many Bytes Write aborted */
							sip_handler.sendNack(command.getSequenceNum());
							write_active = false;
							break;
						}
					
						if(write_flash)
						{
							LOGINFO("Writing to Flash address %d, with length %d", write_addr + write_index, len);
						}
						else
						{
							LOGINFO("Writing to Ram address %d, with length %d", write_addr + write_index, len);
						}

						/* Following bytes are the data */
						if(write_flash)
						{
							memory.flash.write(write_addr + write_index, command.getData() + 2, len);
							write_index += len;
						}
						else
						{
							for(uint32_t i = 0; i < len; i++)
							{
								memory.hyperram[write_addr + write_index++] = command.getData()[i+2];
							}
						}

						if(write_index >= write_len)
						{
							/* Sucessfully written all bytes */
							write_active = false;
						}

						sip_handler.sendAck(command.getSequenceNum());
						break;
					}
					else
					{
						/* Never had a Writing mode initialized */
						sip_handler.sendNack(command.getSequenceNum());
					}
					break;
				}
				case Command::MEMORY_DUMP_COMMAND_ID:
				{
					/* First Byte is the Memory Type */
					bool dump_flash = command.getData()[0];
					/* Next 3 Bytes are Memory Address */
					uint32_t dump_addr = command.getData()[1] | command.getData()[2] << 8 | command.getData()[3] << 16; 
					/* Next 2 Bytes is Length, maximum 4096 */
					uint32_t dump_len = command.getData()[4] | command.getData()[5] << 8;
					if(dump_len > 4096) { sip_handler.sendNack(command.getSequenceNum()); break; }

					if(dump_flash)
					{
						LOGINFO("Dumping from Flash address %d, with length %d", dump_addr, dump_len);
					}
					else
					{
						LOGINFO("Dumping from Ram address %d, with length %d", dump_addr, dump_len);
					}

					if(dump_flash)
					{
						uint8_t buf[dump_len];
						memory.flash.read(dump_addr, buf, dump_len);
						response.build(command.getSequenceNum(), POWERED_ON, MEMORY_DUMP_RESPONSE_ID, buf, dump_len);
						sip_handler.sendResponse(response);
					}
					else
					{
						response.build(command.getSequenceNum(), POWERED_ON, MEMORY_DUMP_RESPONSE_ID, (uint8_t*)memory.hyperram + dump_addr, dump_len);
						sip_handler.sendResponse(response);
					}
					break;
				}
			}
		}
	}
}
