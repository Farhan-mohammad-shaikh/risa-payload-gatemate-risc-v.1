#!/usr/bin/env python3

#
# This file is part of LiteX-Boards.
#
# Copyright (c) 2023 Gwenhael Goavec-merou<gwenhael.goavec-merou@trabucayre.com>
# SPDX-License-Identifier: BSD-2-Clause

from migen import *

from litex.gen import *

from own_periphs import colognechip_gatemate_evb

from litex.build.io import CRG

from litex.soc.cores import uart
from litex.soc.cores.timer import *
from litex.soc.cores.gpio import *
from litex.soc.cores.hyperbus import *
from litex.soc.cores.bitbang import I2CMaster
from litex.soc.cores.spi.spi_master import SPIMaster
from own_periphs.spi.spi_master import CustomSpiMaster
from own_periphs.uart.uart import CustomUart

from litex.soc.cores.clock.colognechip import GateMatePLL
from litex.soc.integration.soc_core import *
from litex.soc.integration.builder import *
from litex.soc.integration.soc import SoCRegion

from litex.build.generic_platform import Pins

from litex.soc.cores.led import LedChaser

# CRG ----------------------------------------------------------------------------------------------

class _CRG(LiteXModule):
	def __init__(self, platform, sys_clk_freq):
		self.rst    = Signal()
		rst_n       = Signal()
		self.cd_sys = ClockDomain()

		# # #

		# Clk / Rst
		clk10 = platform.request("clk10")
		self.rst = 0

		self.specials += Instance("CC_USR_RSTN", o_USR_RSTN = rst_n)

		# PLL
		self.pll = pll = GateMatePLL(perf_mode="economy")
		self.comb += pll.reset.eq(~rst_n | self.rst)
		pll.register_clkin(clk10, 10e6)
		pll.create_clkout(self.cd_sys, sys_clk_freq)

# BaseSoC ------------------------------------------------------------------------------------------

class BaseSoC(SoCCore):
	def __init__(self, sys_clk_freq=8e6,
		with_led_chaser = False,
		**kwargs):
		platform = colognechip_gatemate_evb.Platform()

		# USBUART PMOD as Serial--------------------------------------------------------------------
		platform.add_extension(colognechip_gatemate_evb.usb_pmod_io("PMODB"))
		kwargs["uart_name"] = "usb_uart"

		# CRG --------------------------------------------------------------------------------------
		self.crg = _CRG(platform, sys_clk_freq)

		# SoCCore ----------------------------------------------------------------------------------
		SoCCore.__init__(self, platform, sys_clk_freq, ident="LiteX SoC on GateMate EVB", **kwargs)

		# Add Timer
		self.timer1 = Timer();
		#self.timer2 = Timer();
	
		# Leds -------------------------------------------------------------------------------------
		self.leds = GPIOOut(pads = platform.request_all("user_led_n"))

		# User Button
		#self.btn = GPIOIn(pads = platform.request("mybtn"))

		# Uart
		self.uart_logging = CustomUart(self.platform, platform.request("uart_logging"), sys_clk_freq, 38400)
		self.irq.add("uart_logging", use_loc_if_exists=True)

		#self.uart_obc = CustomUart(self.platform, platform.request("uart_obc"), sys_clk_freq, 9600)
		#self.irq.add("uart_obc", use_loc_if_exists=True)

		self.uart_ice40 = CustomUart(self.platform, platform.request("uart_ice40"), sys_clk_freq, 38400)
		self.irq.add("uart_ice40", use_loc_if_exists=True)
		
		# I2C
		self.i2c0 = I2CMaster(pads=platform.request("i2c0"))
		self.i2c1 = I2CMaster(pads=platform.request("i2c1"))
		
		# ICE40 Voltage Control
		self.ice40_power_fauld = GPIOIn(pads=platform.request("power_fauld_ice40"))
		self.ice40_core_en = GPIOOut(pads=platform.request("core_en_ice40"))
		self.ice40_vio_en = GPIOOut(pads=platform.request("vio_en_ice40"))
		self.ice40_osc_en = GPIOOut(pads=platform.request("osc_en_ice40"))

		# Programming ICE40
		self.ice40 = CustomSpiMaster(self.platform, platform.request("spi_ice40"), 10)
		self.ice40_cp = GPIOTristate(pads=platform.request("creset_ice40"))
		self.ice40_cd = GPIOIn(pads=platform.request("cdone_ice40"))

		# Flash
		self.flash = CustomSpiMaster(self.platform, platform.request("spi_flash"), 10)

		# Hyperram
		self.hyperram = HyperRAM(platform.request("hyperram"), sys_clk_freq=sys_clk_freq)
		self.bus.add_slave("hyperram", slave=self.hyperram.bus, region=SoCRegion(origin=0x20000000, size=8 * 1000000, mode="rwx"))
	
        # ICE40_VIO - GateMate IO EAST Interface
		self.ice40_io_vio_0 = GPIOTristate(pads=platform.request("ice40_io_vio_0"))
		self.ice40_io_vio_1 = GPIOTristate(pads=platform.request("ice40_io_vio_1"))
		self.ice40_io_vio_2 = GPIOTristate(pads=platform.request("ice40_io_vio_2"))
		self.ice40_io_vio_3 = GPIOTristate(pads=platform.request("ice40_io_vio_3"))
		self.ice40_io_vio_4 = GPIOTristate(pads=platform.request("ice40_io_vio_4"))
		self.ice40_io_vio_5 = GPIOTristate(pads=platform.request("ice40_io_vio_5"))
        # ICE40_VCORE - GateMate IO SOUTH Interface
		self.ice40_io_vcore_0 = GPIOTristate(pads=platform.request("ice40_io_vcore_0"))
		self.ice40_io_vcore_1 = GPIOTristate(pads=platform.request("ice40_io_vcore_1"))
		self.ice40_io_vcore_2 = GPIOTristate(pads=platform.request("ice40_io_vcore_2"))
        #self.ice40_io_vcore_3 = GPIOTristate(pads=platform.request("ice40_io_vcore_3"))
		self.ice40_io_vcore_4 = GPIOTristate(pads=platform.request("ice40_io_vcore_4"))
        #self.ice40_io_vcore_5 = GPIOTristate(pads=platform.request("ice40_io_vcore_5"))
		# self.debugs=GPIOOut(pads=platform.request_all("user_debug_n"))
		self.gatemate_debug_4 = GPIOTristate(pads=platform.request("gatemate_debug_4"))
		self.gatemate_debug_5 = GPIOTristate(pads=platform.request("gatemate_debug_5"))
		self.gatemate_debug_3 = GPIOTristate(pads=platform.request("gatemate_debug_3"))

# Build --------------------------------------------------------------------------------------------

def main():
	from litex.build.parser import LiteXArgumentParser
	parser = LiteXArgumentParser(platform=colognechip_gatemate_evb.Platform, description="LiteX SoC on Gatemate EVB")
	parser.add_target_argument("--sys-clk-freq", default=8e6, type=float, help="System clock frequency.")
	parser.add_target_argument("--flash",        action="store_true",      help="Flash bitstream.")
	args = parser.parse_args()

	soc = BaseSoC(
		sys_clk_freq = args.sys_clk_freq,
		**parser.soc_argdict)

	builder = Builder(soc, **parser.builder_argdict)
	if args.build:
		builder.build(**parser.toolchain_argdict)

	if args.load:
		prog = soc.platform.create_programmer()
		prog.load_bitstream(builder.get_bitstream_filename(mode="sram"))

	if args.flash:
		from litex.build.openfpgaloader import OpenFPGALoader
		prog = OpenFPGALoader("gatemate_evb_spi")
		prog.flash(0, builder.get_bitstream_filename(mode="flash"))

if __name__ == "__main__":
	main()
