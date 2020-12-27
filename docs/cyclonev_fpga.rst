The Cyclone V FPGA
==================

The FPGAs
---------

The Cyclone V is a series of FPGAs produced initially by Altera, now
Intel.  It is based on a series of seven dies with varying levels of
capability, which is then derived into more than 400 SKUs with
variations in speed, temperature range, and enabled internal hardware.

As pretty much every FPGA out there, the dies are organized in grids.

.. figure:: floorplans.*
   :width: 100%

   Floor plan of the seven die types


The FPGA, structurally, is a set of logic blocks of different types
communicating with each other either through direct links or through a
large routing network that spans the whole grid.

Some of the logic blocks take visible floor space.  Specifically, the
notches on the left are the space taken by the high speed serial
interfaces (hssi and pma3).  Also, the top-right corner in the sx50f
and sx120f variants is used to fit the hps, a dual-core arm.


Bitstream stucture
------------------

The bitstream is built from three rams:

* Option ram
* Peripheral ram
* Configuration ram

The option ram is composed of 32 blocks of 40 bits, of which only 12
are actually used.  It includes the global configurations for the
chip, such as the jtag user id, the programming voltage, the internal
oscillator configuration, etc.

The peripheral ram stores the configuration of all the blocks situated
on the borders of the chip, e.g. everything outside of labs, mlabs,
dsps and m10ks.  It is built of 13 to 16 blocks of bits that are sent
through shift registers to the tiles.

The configuration ram stores the configuration of the labs, mlabs,
dsps and m10ks, plus all the routing configuration.  It also includes
the programmable inverters which allows inverting essentially all the
inputs to the peripheral blocks.  It is organised as a rectangle of
bits.

+--------+---------+--------+-------------+
| Die    | Tiles   | Pram   | Cram        |
+========+=========+========+=============+
| e50f   | 55x46   | 51101  | 4958x3928   |
+--------+---------+--------+-------------+
| gx25f  | 49x40   | 54083  | 3856x3412   |
+--------+---------+--------+-------------+
| gt75f  | 69x62   | 90162  | 6006x5304   |
+--------+---------+--------+-------------+
| gt150f | 90x82   | 113922 | 7605x7024   |
+--------+---------+--------+-------------+
| gt300f | 122x116 | 130828 | 10038x9948  |
+--------+---------+--------+-------------+
| sx50f  | 69x62   | 80505  | 6006x5304   |
+--------+---------+--------+-------------+
| sx120f | 90x82   | 99574  | 7605x7024   |
+--------+---------+--------+-------------+



Logic blocks
------------

The logic blocks are of two categories, the inner blocks and the
peripheral blocks.  To a first approximation all the inner blocks are
configured through configuration ram, and the peripheral blocks
through the peripheral ram.  It only matters where it comes to partial
reconfiguration, because only the configuration ram can be dynamically
modified.  We do not yet support it though.

The inner blocks are:

* lab: a logic blocks group with 20 LUTs with 5 inputs and 40 Flip-Flops.
* mlab: a lab that can be reconfigured as 64*20 bits of ram
* dsp: a flexible multiply-add block
* m10k: a block of 10240 bits of dual-ported memory

The peripheral blocks are:

* gpio: general-purpose i/o, a block that controls up to 4 package pins
* dqs16: a block that manage differential input/output for 4 gpio blocks, e.g. up to 16 pins
* fpll: a fractional PLL
* cmux: the clock muxes that drive the clock part of the routing network
* ctrl: the control block with things like jtag
* hssi: the high speed serial interfaces
* hip: the pcie interfaces
* cbuf: a clock buffer for the dqs16
* dll: a delay-locked loop for the dqs16
* serpar: TODO
* lvl: TODO
* term: termination control blocks
* pma3: manages the channels of the hssi
* hmc: hardware memory controller, a block managing sdr/ddr ram interfaces
* hps: a series of 37 blocks managing the interface with the integrated dual-core arm

All of these blocks are configured similarly, through the setup of
block muxes.  They can be of 4 types:
* Boolean
* Symbolic, where the choice is between alphanumeric states
* Numeric, where the choice is between a fixed set of numeric value
* Ram, where a series of bits can be set to any value

Configuring that part of the FPGA consists of configuring the muxes
associated to each block.


Routing network
---------------

A massive routing network is present all over the FPGA.  It has two
almost-disjoint parts.  The data network has a series of inputs,
connected to the outputs of all the blocks, and a series of outputs
that go to data inputs of the blocks.  The clock network consists of
16 global clocks signals that cover the whole FPGA, up to 88 regional
clocks that cover an half of the FPGA, and when an hssi is present a
series of horizontal peripheral clocks that are driven by the serial
communications.  Global and regional clock signals are driven by
dedicated cmux blocks (not the fpll in particular, but they do have
dedicated connections to the cmuxes).

These two networks join on data/clock muxes, which allow peripheral
blocks to select for their clock-like inputs which network the signal
should come from.


Programmable inverters
----------------------

Essentially every output of the routing network that enters a
peripheral block can optionally be inverted by activating the
associated configuration bit.

