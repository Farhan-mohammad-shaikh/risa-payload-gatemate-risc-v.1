from migen import *

from litex.soc.interconnect.csr import *
from litex.gen import *

class BlinkModule(LiteXModule):
    def __init__(self, pads, clock_domain="sys"):     
        self.enable = CSRStorage(size=1, reset=0, description="1 Turns the LED on, 0 Turns it off")

        blinkcounter = Signal(16, reset=0)
        led = Signal()

        sync = getattr(self.sync, clock_domain)

        sync += [
            If(self.enable.storage,
                If(blinkcounter == 0, 
                    led.eq(~led),
                    blinkcounter.eq(100)
                ).Else(
                    blinkcounter.eq(blinkcounter - 1)
                )
            )
        ]

        self.comb += pads.eq(led)
        