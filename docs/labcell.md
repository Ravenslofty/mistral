# Logic Array Block Cell (LABCELL) notes

## What it does
A Logic Array Block Cell contains the combinational look-up tables (LUTs) that are used to generate logic functions.

## CSV format
`LUT bit number (1-indexed)`, `LABCELL index (0-indexed, increases in sixes)`, `LUT bit in bitstream relative to start of LABCELL tile (in bits)`

## Quartus quirks
Quartus internally maps two virtual look-up tables to a single physical look-up table. For example, the virtual LUTs N0 and N3 map to the same physical LUT. Presumably, this lets Quartus map multiple independent LUTs to a single physical LUT.

These virtual LUTs have an index that increases in threes for some harebrained reason, but since the physical LUTs indexes increase in sixes, that's the data stored here. In other words, `if (index % 6 == 3) index -= 3`.