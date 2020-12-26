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


Bitstream stucture
------------------

The bitstream is build from three rams:

* Option ram
* Peripheral ram
* Configuration ram

