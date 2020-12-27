CycloneV internals description
==============================

Routing network
---------------

The routing network follows a single-driver structure: a number of
inputs are grouped together in one place, one is selected through the
configuration, then it is amplified and used to drive a metal line.
There is also usually one bit configuration to disable the driver,
which can be all-off (probably leaving the line floating) or a
specific combination to select vcc.  The drivers correspond to a 2d
pattern in the configuration ram.  There are 70 different patterns,
configured by 1 to 18 bits and mixing 1 to 44 inputs.

The network itself can be split in two parts: the data network and the
clock network.

The data network is a grid of connections.  Horizontal lines (H14, H6
and H3, numbered by the number of tiles they span) and vertical lines
(V12, V4 and V2) helped by wire muxes (WM) connect to each over to
ensure routing over the whole surface.  Then at the tile level
tile-data dispatch (TD) nodes allow to select between the available
signals.

Generic output (GOUT) nodes then select between TD nodes to connect to
logic blocks inputs.  Logic block outputs go to Generic Input (GIN)
nodes which feed in the connections.  In addition a dedicated network,
the Loopback dispatch (LD) connects some of the outputs from the
labs/mlabs to their inputs for fast local data routing.

The clock network is more of a top-down structure.  The top structures
are Global clocks (GCLK), Regional clocks (RCLK) and Peripheral clocks
(PCLK).  They're all driven by specialized logic blocks we call Clock
Muxes (cmux).  There are two horizontal cmux in the middle of the top
and bottom borders, each driving 4 GCLK and 20 RCLK, two vertical in
the middle of the left and right borders each driving 4 GCLK and 12
RCLK, and 3 to 4 in the corners driving 6 RCLK each.  The dies
including an HPS (sx50f and sx120f) are missing the top-right cmux
plus some of the middle-of-border-driven RCLK.  That gives a total of
16 GCLK and 66 to 88 RCLK.  In addition PCLK start from HSSI blocks to
distribute serial clocks to the network.

The GCLK span the whole grid.  A RCLK spans half the grid.  A PCLK
spans a number of tiles horizontally to its right.

The second level is Sector clocks, SCLK, which spans small rectangular
zones of tiles and connect from GCLK, RCLK and PCLK.  The on the third
level, connecting from SCLK, is Horizontal clocks (HCLK) spanning
10-15 horizontal tiles and Border clocks (BCLK) rooted regularly on
the top and bottom borders.  Finally Tile clocks (TCLK) connect from
HCLK and BCLK and distribute the clocks within a tile.

In addition the PMUX nodes at the entrance of plls select between
SCLKs, and the GCLKFB and RCLKFB bring back feedback signals from the
cmux to the pll.

Inner blocks directly connect to TCLK and have internal muxes to
select between clock and data inputs for their control.  Peripheral
blocks tend to use a secondary structure composed from a TDMUX that
selects one TD between multiple ones followed by a DCMUX that selects
between the TDMUX and a TCLK so that their clock-like inputs can be
driven from either a clock or a data signal.

Most GOUT and DCMUX connected to inputs to peripheral blocks are also
provided with an optional inverter.


Inner logic blocks
------------------

LAB
^^^

The LABs are the main combinatorial and register blocks of the FPGA.
A LAB tile includes 10 sub-blocks with 64 bits of LUT splitted in 6
parts, four Flip-Flops, two 1-bit adders and a lot of routing logic.
In addition a common control subblock selects and dispatches clock,
enable, clear, etc signals.


MLAB
^^^^

A MLAB is a lab that can optionally be turned into a 640-bits RAM or
ROM.  The wiring is identical to the LAB, only some additional muxes
are provided to select the RAM/ROM mode.

TODO: address/data wiring in RAM/ROM mode.


DSP
^^^

The DSP blocks provide a multiply-adder with either three 9x9, two
18x18 or one 27x27 multiply, and the 64-bits accumulator.  Its large
number of inputs and output makes it span two tiles vertically.

TODO: everything, GOUT/GIN/DCMUX mapping is done


M10K
^^^^

The M10K blocks provide 10240 (256*40) bits of dual-ported rom or ram.

TODO: everything, GOUT/GIN/DCMUX mapping is done


Peripheral logic blocks
-----------------------

GPIO
^^^^

The GPIO blocks connect the FPGA with the exterior through the package
pins.  Each block controls 4 pads, which are connected to up to 4
pins.

TODO: everything, GOUT/GIN/DCMUX mapping is done


DQS16
^^^^^

The DQS16 blocks handle differential signaling protocols.  Each
supervises 4 GPIO blocks for a total of 16 signals, hence their name.

TODO: everything


FPLL
^^^^

The Fractional PLL blocks synthesize 9 frequencies from an input with integer or fractional ratios.

TODO: everything, GOUT/GIN/DCMUX mapping is done


CMUX
^^^^

The Clock mux blocks drive the GCLK and the RCLK.

TODO: fpll feedback lines


CTRL
^^^^

The Control block gives access to a number of anciliary functions of the FPGA.

TODO: everything, GOUT/GIN/DCMUX mapping is done


HSSI
^^^^

The High speed serial interface blocks control the
serializing/deserializing capabilities of the FPGA.

TODO: everything


HIP
^^^

The PCIe Hard-IP blocks control the PCIe interfaces of the FPGA.

TODO: everything


DLL
^^^

The Delay-Locked loop does phase control for the DQS16.

TODO: everything


SERPAR
^^^^^^

Unclear yet.

TODO: everything


LVL
^^^

The Leveling Delay Chain does something linked to the DQS16.

TODO: everything


TERM
^^^^

The TERM blocks control the On-Chip Termination circuitry

TODO: everything


PMA3
^^^^

The PMA3 blocks control triplets of channels used with the HSSI.

TODO: everything


HMC
^^^

The Hardware memory controller controls sets of GPIOs to implement
modern SDR and DDR memory interfaces.  In the sx dies one of them is
taken over by the HPS.  They can be bypassed in favor of direct access
to the GPIOs.

TODO: everything, and in particular the hmc-input -> GPIO input
mapping when bypassed.


HPS
^^^

The interface between the FPGA and the Hard processor system is done
through 37 specialized blocks or 28 different types.

TODO: everything.  GOUT/GIN/DCMUX mapping is done except for HPS_CLOCKS.

HPS_BOOT
""""""""

HPS_CLOCKS
""""""""""

HPS_CLOCKS_RESETS
"""""""""""""""""

HPS_CROSS_TRIGGER
"""""""""""""""""

HPS_DBG_APB
"""""""""""

HPS_DMA
"""""""

HPS_FPGA2HPS
""""""""""""

HPS_FPGA2SDRAM
""""""""""""""

HPS_HPS2FPGA
""""""""""""

HPS_HPS2FPGA_LIGHT_WEIGHT
"""""""""""""""""""""""""

HPS_INTERRUPTS
""""""""""""""

HPS_JTAG
""""""""

HPS_LOAN_IO
"""""""""""

HPS_MPU_EVENT_STANDBY
"""""""""""""""""""""

HPS_MPU_GENERAL_PURPOSE
"""""""""""""""""""""""

HPS_PERIPHERAL_CAN
""""""""""""""""""
(2 blocks)

HPS_PERIPHERAL_EMAC
"""""""""""""""""""
(2 blocks)

HPS_PERIPHERAL_I2C
""""""""""""""""""
(4 blocks)

HPS_PERIPHERAL_NAND
"""""""""""""""""""

HPS_PERIPHERAL_QSPI
"""""""""""""""""""

HPS_PERIPHERAL_SDMMC
""""""""""""""""""""

HPS_PERIPHERAL_SPI_MASTER
"""""""""""""""""""""""""
(2 blocks)

HPS_PERIPHERAL_SPI_SLAVE
""""""""""""""""""""""""
(2 blocks)

HPS_PERIPHERAL_UART
"""""""""""""""""""
(2 blocks)

HPS_PERIPHERAL_USB
""""""""""""""""""
(2 blocks)

HPS_STM_EVENT
"""""""""""""

HPS_TEST
""""""""

HPS_TPIU_TRACE
""""""""""""""

