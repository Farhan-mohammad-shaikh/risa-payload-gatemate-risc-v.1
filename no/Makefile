BUILD_DIR?=build/colognechip_gatemate_evb
SOC_DIR=/home/lucas/Dokumente/litex/litex/litex/soc
CODE_DIR=code/src
CRT_DIR=/home/lucas/Dokumente/litex/litex/litex/soc/cores/cpu/vexriscv

include $(BUILD_DIR)/software/include/generated/variables.mak
include common.mak

OBJECTS += $(CRT_DIR)/crt0.o $(CODE_DIR)/main.o $(CODE_DIR)/spi.o $(CODE_DIR)/ice40prog.o $(CODE_DIR)/dac60501.o $(CODE_DIR)/tmp117.o $(CODE_DIR)/pac1942.o \
$(CODE_DIR)/i2c.o $(CODE_DIR)/timer.o $(CODE_DIR)/serial.o $(CODE_DIR)/delay.o $(CODE_DIR)/logging.o $(CODE_DIR)/mx25r6435f.o \
$(CODE_DIR)/experimentmanager.o $(CODE_DIR)/sensorcontext.o $(CODE_DIR)/crc16.o $(CODE_DIR)/sip_handler.o $(CODE_DIR)/memorycontext.o $(CODE_DIR)/gpio.o $(CODE_DIR)/riscvMatrixExperiment.o $(CODE_DIR)/uvVminPropExperiment.o $(CODE_DIR)/isfdExperiment.o $(CODE_DIR)/ice40FlashExperiment.o

all: demo.bin

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

	chmod -x $@
	make softclean
    #make clean
vpath %.a $(PACKAGES:%=../%)
	
demo.elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -T $(CODE_DIR)/linker.ld -N -o $@ \
		$(OBJECTS) \
		$(PACKAGES:%=-L$(BUILD_DIR)/software/%) \
		-Wl,--whole-archive \
		-Wl,--gc-sections \
		-Wl,-Map,$@.map \
		$(LIBS:lib%=-l%)

ifneq ($(OS),Windows_NT)
	chmod -x $@
endif

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

VPATH = $(BIOS_DIRECTORY):$(BIOS_DIRECTORY)/cmds:$(CPU_DIRECTORY)

%.o: %.cpp
	$(compilexx)

%.o: %.c
	$(compile)

%.o: %.S
	$(assemble)

doc:
	./colognechip_gatemate_evb.py --build --doc --cpu-variant=minimal --integrated-rom-init=demo.bin --cpu-type=vexriscv --integrated-sram-size=0x0009000
load:
	make all
	./colognechip_gatemate_evb.py --build --integrated-rom-init=demo.bin --load --cpu-variant=minimal --cpu-type=vexriscv --integrated-sram-size=0x0010000
flash:
	make all
	./colognechip_gatemate_evb.py --build --integrated-rom-init=demo.bin --flash --cpu-variant=minimal --cpu-type=vexriscv --integrated-sram-size=0x0010000

pgm:
	openFPGALoader -c gatemate_pgm build/colognechip_gatemate_evb/gateware/colognechip_gatemate_evb_00.cfg.bit

flash-pgm:
	openFPGALoader -c gatemate_pgm -f ice40_fw.bin --verify

dump:
	openFPGALoader -c gatemate_pgm --dump-flash --file-size 15073 bin --offset 1048576
																															  
softclean:
	$(RM) $(CODE_DIR)/$(OBJECTS) $(CODE_DIR)/demo.elf $(CODE_DIR)/demo.elf.map $(CODE_DIR)/*.d

clean:
	$(RM) $(OBJECTS) demo.elf demo.bin .*~ *~

.PHONY: all clean
