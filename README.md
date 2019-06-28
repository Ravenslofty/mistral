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

At the heart of any synthesis script should be the `synth` command, which is a premade 
general-purpose script for synthesis to unknown architectures. We'll put this in our script.

```
# filename: synth_cyclone.ys
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
information on what to synthesise to, has converted the design to its internal cells. Yosys has two
types of internal cell, which I'll name "abstract" (generally named `$foo`) and "concrete" (generally
named `$_FOO_`).

The cells produced by Yosys include basic boolean functions, such as `$_AND_`/`$_OR_`/`$_NOT_` etc,
but also some more complex functions like `a and (not b)` (`$_ANDNOT_`), and `(a and b) nor c`
(`$_AOI3_` for "3-input and-or-invert"). There is also `$_DFF_P_`, which I'll get back to later.

Unfortunately, the Cyclone V is not made up of logic gates like this. It instead uses look-up tables
(LUTs) that are a function of the input bits, and these can be split up into multiple modes.

The absolute maximum number of inputs an ALM can accept is seven in extended mode, so let's
synthesise for a 7-input LUT architecture. We can do this using the `-lut 7` argument to `synth`.

We will also add `stat -width` to print the statistics screen with the width of each cell.

Now our script looks like this:

```
# filename: synth_cyclone.ys
synth -lut 7
stat -width
```

This produces:

```
=== axilxbar ===

   Number of wires:               3454
   Number of wire bits:          10390
   Number of public wires:         514
   Number of public wire bits:    5818
   Number of memories:               0
   Number of memory bits:            0
   Number of processes:              0
   Number of cells:               6480
     $_DFF_P_                     1900
     $lut_2                         68
     $lut_3                        482
     $lut_4                        180
     $lut_5                        724
     $lut_6                       1158
     $lut_7                       1968
```

Here we can see that Yosys has synthesised this to mostly 6-input and 7-input LUTs, with smaller
LUTs being used less often. These LUTs being bigger and more flexible means Yosys needs less of
them to produce the same result.

## Techmap

Let's revisit the concrete gate here: `$_DFF_P_`, as a small example of what we need to do for the
LUTs. A `$_DFF_P_` cell is a single D flip-flop (DFF) that is triggered on the positive edge of a 
clock cycle. An ALM in the Cyclone V contains four DFFs that are significantly more powerful, but
we don't need that power just yet.

To convert a DFF cell to a different cell, we'll use the `techmap` pass. This takes in a template
written in Verilog, and uses it to map an internal cell to a specified one.

First, we need a basic model of a flip-flop in Verilog for simulation purposes.

```
// filename: cells_sim.v
module MISTRAL_FF(input D, CLK, output reg Q);

always @(posedge CLK)
    Q <= D;

endmodule
```

Then we need to write a `techmap` pass to convert a `$_DFF_P_` to a `MISTRAL_FF`.

```
// filename: cells_map.v
module \$_DFF_P_ (input D, C, output Q); MISTRAL_FF _TECHMAP_REPLACE_(.D(D), .CLK(C), .Q(Q)); endmodule
```

By naming the module after the cell we want to replace, `techmap` knows we want to replace that cell.
Naming the created cell `_TECHMAP_REPLACE_` means that `techmap` directly substitutes the old cell
with the new one, and is best used for direct 1:1 replacements.

We then need our script to read the replacement cells, but we don't want it to look inside the
replacement as that would lead to recursion. This can be done either by marking the simulation cell
as `(* blackbox *)` or better by using the `-lib` argument to `read_verilog`.

Finally, we need to invoke `techmap` after `synth` to map the relevant gate.

```
# filename: synth_cyclone.ys
synth -lut 7
read_verilog -lib cells_sim.v
techmap -map cells_map.v
stat -width
```

Note that Yosys is sensitive to the location of `cells_sim.v` and `cells_map.v`; if it can't find
it, try synthesising in the same directory as these files or use an absolute path to the file.

Running this synthesis script gives the following:

```
=== axilxbar ===

   Number of wires:               9154
   Number of wire bits:          16090
   Number of public wires:         514
   Number of public wire bits:    5818
   Number of memories:               0
   Number of memory bits:            0
   Number of processes:              0
   Number of cells:               6480
     $lut_2                         68
     $lut_3                        482
     $lut_4                        180
     $lut_5                        724
     $lut_6                       1158
     $lut_7                       1968
     MISTRAL_FF                   1900
```
