from amaranth import *

class hps_general_purpose(Elaboratable):
    def __init__(self):
        self.o_gp_out = Signal(32)
        self.i_gp_in = Signal(32)

    def elaborate(self, platform):
        m = Module()
        m.submodules.i = Instance("cyclonev_hps_interface_mpu_general_purpose", i_gp_in = self.i_gp_in, o_gp_out = self.o_gp_out)
        return m

