from migen import *

from litex.build.vhd2v_converter import *
from litex.soc.interconnect.csr import *
from litex.gen import *

class BlinkModule(LiteXModule):
	def __init__(self, platform, pads, clock_domain="sys"):     
		self.enable = CSRStorage(size=1, reset=0, description="1 Turns the LED on, 0 Turns it off")
		led = Signal()
		div = Signal(16)
		div.eq(100)

		self.parameters = dict(
			i_clk 	= ClockSignal("sys"),
			i_en	= self.enable.storage,
			i_div	= 100,
			o_led	= led,
		)

		self.vhd2v_converter = VHD2VConverter(platform,
			top_entity = "blink",
			build_dir = os.path.abspath(os.path.dirname(__file__)),
			force_convert = True,
			params = self.parameters,
			add_instance = True,
		)
		self.vhd2v_converter.add_source("/home/haron/Desktop/no/own_periphs/blink.vhd")
		self.comb += pads.eq(led)
		