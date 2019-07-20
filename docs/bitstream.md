Cyclone V bitstream format (.rbf)
=================================

First (settings?) packet
------------------------

128 0xFF bytes - possibly for synchronisation?
`6A 6A 6A 6A` - sync word
Series of 32-bit little-endian register writes, but the top 16 bits are all set.
CRC16-MODBUS of the register writes

Data packets
------------

914 bytes of payload
CRC16-MODBUS of the payload

Terminating packet
------------------
434 0xFF bytes
