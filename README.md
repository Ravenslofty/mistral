# Rough Retargeting Yosys

## Prerequisites

You'll want Yosys itself; I am using Yosys 0.8+531 (git commit d4f77d40).

Gather all the information you can about your target; the more, the better.

As an example, I'm going to use the publicly available [Cyclone V Device Handbook][handbook].
Intel have not released any documentation of the Cyclone V bitstream, and reverse engineering
is outside the scope of this guide.

[handbook]: https://www.intel.com/content/dam/www/programmable/us/en/pdfs/literature/hb/cyclone-v/cv_5v2.pdf

## First steps

Yosys accepts a command list either by writing to its standard input (not recommended for serious
scripting), passing commands as an argument string through its through its `-p` argument, or
passing commands in a file through its `-s` argument. I'm going to use the latter.

Let's call our script `synth_cyclone.ys`.

At the heart of any synthesis script should be the `synth` command, which is a premade 
general-purpose script for synthesis to unknown architectures. We'll put this in our script.

```
synth
```

And then we need something to synthesise; I've found the [axilxbar Yosys benchmark][bench] to be a
good tradeoff between complexity and synthesis time, taking about ten seconds to synthesise on my
machine.

[bench]: https://github.com/YosysHQ/yosys-bench/tree/master/verilog/benchmarks_large/wb2axip

Then we can run `yosys -s path/to/synth_cyclone.ys axilxbar.v`, which produces these statistics:

```
=== axilxbar ===

   Number of wires:              13780
   Number of wire bits:          20716
   Number of public wires:         514
   Number of public wire bits:    5818
   Number of memories:               0
   Number of memory bits:            0
   Number of processes:              0
   Number of cells:              16806
     $_ANDNOT_                    2590
     $_AND_                       1286
     $_AOI3_                       405
     $_AOI4_                       902
     $_DFF_P_                     1900
     $_MUX_                       4021
     $_NAND_                       807
     $_NOR_                        181
     $_NOT_                       1074
     $_OAI3_                      1022
     $_OAI4_                       461
     $_ORNOT_                      670
     $_OR_                        1383
     $_XNOR_                        20
     $_XOR_                         84
```

The most important thing to observe is the cell count. Here, Yosys, lacking any more specific
information on what to synthesise to, has converted the design to its internal cells. These include
basic boolean functions, such as `$_AND_`/`$_OR_`/`$_NOT_` etc, but also some more complex functions
like `a and (not b)` (`$_ANDNOT_`), and `(a and b) nor c` (`$_AOI3_` for "3-input and-or-invert").

There is also `$_DFF_P_`, which I'll get back to later.

Unfortunately, the Cyclone V is not made up of logic gates like this. It instead uses look-up tables
(LUTs) that are a function of the input bits, and these can be split up into multiple modes.

The absolute maximum number of inputs an ALM can accept is six, so let's synthesise for a 6-input
LUT architecture. We can do this using the `-lut 6` argument to `synth`.

We will also add `stat -width` to print the statistics screen with the width of each cell.

Now our script looks like this:

```
synth -lut 6
stat -width
```

This produces:

```
=== axilxbar ===

   Number of wires:               3818
   Number of wire bits:          10754
   Number of public wires:         514
   Number of public wire bits:    5818
   Number of memories:               0
   Number of memory bits:            0
   Number of processes:              0
   Number of cells:               6844
     $_DFF_P_                     1900
     $lut_2                        121
     $lut_3                        495
     $lut_4                        371
     $lut_5                       1158
     $lut_6                       2799
```

Here we can see that Yosys has synthesised this to mostly 6-input LUTs, with smaller LUTs being
used less often. These LUTs being bigger and more flexible means Yosys needs less of them to
produce the same result.
