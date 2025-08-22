from migen import *

import os
from litex.build.vhd2v_converter import *
from litex.soc.interconnect.csr import *
from litex.soc.interconnect.csr_eventmanager import *
from litex.gen import *

class CustomUart(LiteXModule):
	pads_layout = [("tx", 1), ("rx", 1)]

	def add_sources(self, platform):
		sdir = os.path.abspath(os.path.dirname(__file__)) + "/comp/"
		with open(os.path.join(os.path.dirname(__file__), 'source_list.txt')) as f:
			for line in f:
				srcfile = os.path.join(sdir, line.strip())
				self.vhd2v_converter.add_source(srcfile)

	def __init__(self, platform, pads, sys_clk_freq, baud_rate, clock_domain="sys"):  
		# Assign the Pads
		if pads is None:
			pads = Record(self.pads_layout)
		self.pads = pads

		# Create User Registers
		self.tx_data = CSRStorage(size=8, reset=0, description="input data to be transmitted over UART")
		self.rx_data = CSRStatus(size=8, reset=0, description="output data received via UART")

		self.control = CSRStorage(description="UART Control.", fields=[
			CSRField("DIN_VLD", size=1, offset=0, description="input data (tx_data) are valid, this bit will be cleared automaticaly", pulse=True),
		])

		self.status = CSRStatus(description="UART Status.", fields=[
			CSRField("DIN_RDY", size=1, offset=0, description="transmitter is ready and valid input data will be accepted for transmiting"),
			CSRField("DOUT_VLD", size=1, offset=1, description="output data (DOUT) are valid (is assert only for one clock cycle)"),
			CSRField("FE", size=1, offset=2, description="Frame_Error stop bit was invalid (is assert only for one clock cycle)"),
			CSRField("PE", size=1, offset=3, description="Parity_Error parity bit was invalid (is assert only for one clock cycle)"),
		])
		
		dout_vld_sig = Signal()

		rx_data_raw = Signal(8)
		rx_data_buffer = Signal(8)
		self.sync += If(dout_vld_sig, rx_data_buffer.eq(rx_data_raw))
		self.comb += self.rx_data.status.eq(rx_data_buffer)

		# Setup an Interrupt for when input data is received
		self.submodules.ev = EventManager()
		self.ev.rx = EventSourceProcess(edge="rising")
		self.ev.finalize()

		self.comb += self.ev.rx.trigger.eq(dout_vld_sig == 1)
		self.comb += self.status.fields.DOUT_VLD.eq(dout_vld_sig)

		verilog_path = os.path.abspath(os.path.dirname(__file__)) + "/UART.v"

		# If VHDL hasnt been converted yet, we convert it, else use the same verilog
		if not os.path.exists(verilog_path):
			# Assign Parameters
			self.parameters = dict(
				p_CLK_FREQ = int(sys_clk_freq),
				p_BAUD_RATE = baud_rate,
				p_PARITY_BIT = "none",
				p_USE_DEBOUNCER = False,

				i_CLK = ClockSignal(clock_domain),
				i_RST = ResetSignal(),
				o_UART_TXD = pads.tx,
				i_UART_RXD = pads.rx,

				i_DIN = self.tx_data.storage,
				i_DIN_VLD = self.control.fields.DIN_VLD, 
				o_DIN_RDY = self.status.fields.DIN_RDY,

				o_DOUT = rx_data_raw,
				o_DOUT_VLD = dout_vld_sig,
				o_FRAME_ERROR = self.status.fields.FE,
				o_PARITY_ERROR = self.status.fields.PE,
			)

			# Convert to Verilog
			self.vhd2v_converter = VHD2VConverter(platform,
				top_entity = "UART",
				build_dir = os.path.abspath(os.path.dirname(__file__)),
				params = self.parameters,
				force_convert = True,
				add_instance = True,
				work_package = "uart",
			)

			self.add_sources(platform)
		else:
			self.specials += Instance("UART",
				i_CLK = ClockSignal(clock_domain),
				i_RST = ResetSignal(),
				o_UART_TXD = pads.tx,
				i_UART_RXD = pads.rx,

				i_DIN = self.tx_data.storage,
				i_DIN_VLD = self.control.fields.DIN_VLD, 
				o_DIN_RDY = self.status.fields.DIN_RDY,

				o_DOUT = rx_data_raw,
				o_DOUT_VLD = dout_vld_sig,
				o_FRAME_ERROR = self.status.fields.FE,
				o_PARITY_ERROR = self.status.fields.PE,
			)

			platform.add_source(verilog_path)
