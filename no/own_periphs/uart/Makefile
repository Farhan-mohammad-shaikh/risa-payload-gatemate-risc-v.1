all:
  # analyze vhdl
	make analyze
	
  # Synthesize
	yosys -m ghdl -p 'ghdl --std=08 -frelaxed uart; synth_ice40 -json synth.json'

  # P&R
	nextpnr-ice40 --up5k --pcf constraint.pcf --asc result.asc --json synth.json

  # Generate Bitstream
	icepack result.asc result.bin

analyze:
	ghdl -a --std=08 comp/uart_parity.vhd comp/uart_clk_div.vhd comp/uart_tx.vhd comp/uart_rx.vhd comp/uart_debouncer.vhd comp/uart.vhd

# Programm the chip
burn:
	iceprog result.bin
	make clean

clean:
	rm -rf result.asc result.bin synth.json *.o *.cf

elaborate:
	ghdl -e --std=08 -frelaxed blink

run:
	ghdl -r --std=08 -frelaxed blink --wave=waveform.ghw --vcd=waveform.vcd

sim:
	make analyze
	make elaborate
	make run
	make clean

# Convert VHDL to Verilog
conv:
	yosys -m ghdl -p 'ghdl --std=08 -frelaxed uart; write_verilog uart.v'