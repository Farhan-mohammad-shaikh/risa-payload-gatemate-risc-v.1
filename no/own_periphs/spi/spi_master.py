from migen import *

from litex.build.vhd2v_converter import *
from litex.soc.interconnect.csr import *
from litex.gen import *

class CustomSpiMaster(LiteXModule):
	pads_layout = [("clk", 1), ("cs_n", 1), ("mosi", 1), ("miso", 1)]
	def __init__(self, platform, pads, clock_div=1, clock_domain="sys"):  
		# Assign the Pads
		if pads is None:
			pads = Record(self.pads_layout)
		self.pads = pads

		self.tx = CSRStorage(size=8, reset=0, description="Data to be sent via tx")
		self.rx = CSRStatus(size=8, reset=0, description="Data that has been received via rx")
		self.busy = CSRStatus(size=1, description="Master Busy Signal")

		self.control = CSRStorage(description="SPI Control.", fields=[
			CSRField("enable", size=1, offset=0, description="Initiate Communication", pulse=True),
			CSRField("cpol", size=1, offset=1, description="Clock Polarity Mode"),
			CSRField("cpha", size=1, offset=2, description="Clock Phase Mode"),
		])

		# Avoid conflics with the vhdl module, so we assign them synchronously
		rx = Signal(8)
		tx = Signal(8)
		enable = Signal()
		cpol = Signal()
		cpha = Signal()
		busy = Signal()

		self.comb += tx.eq(self.tx.storage)
		self.comb += rx.eq(self.rx.status)
		self.comb += enable.eq(self.control.fields.enable)
		self.comb += cpol.eq(self.control.fields.cpol)
		self.comb += cpha.eq(self.control.fields.cpha)

		self.ss_n = CSRStorage(size=1, description="Directly controls the Slave Select signal", reset=1)

		# Create an Artifical Clock that can be Slower than the actual Clock if needed
		spi_clk = Signal()
		spi_clk_counter = Signal(16, reset=0)

		self.sync += [
				If(spi_clk_counter == 0, 
					spi_clk.eq(1),
					spi_clk_counter.eq(clock_div)
				).Else(
					spi_clk.eq(0),
					spi_clk_counter.eq(spi_clk_counter - 1)
				)
		]

		# Assign Parameters
		self.parameters = dict(
			p_data_length = 8,
			i_clk 		= ClockSignal(clock_domain),
			i_div_clk	= spi_clk,
			i_reset_n 	= ~ResetSignal(),
			i_enable 	= enable,
			i_cpol 		= cpol,
			i_cpha 		= cpha,
			i_miso 		= self.pads.miso,
			i_tx   		= self.tx.storage,
			o_busy   	= self.busy.status,
			o_mosi		= self.pads.mosi,
			o_rx		= self.rx.status,
			o_sclk		= self.pads.clk,
		)
		self.comb += self.pads.cs_n.eq(self.ss_n.storage)

		# Convert to Verilog
		self.vhd2v_converter = VHD2VConverter(platform,
			top_entity = "spi_master",
			build_dir = os.path.abspath(os.path.dirname(__file__)),
			force_convert = True,
			params = self.parameters,
			add_instance = True,
		)

		self.vhd2v_converter.add_source(os.path.abspath(os.path.dirname(__file__)) + "/spi_master.vhd")
		