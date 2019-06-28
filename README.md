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

The absolute maximum number of inputs an ALM can accept is six in normal mode, so let's
synthesise for a 6-input LUT architecture. We can do this using the `-lut 6` argument to `synth`.

We will also add `stat -width` to print the statistics screen with the width of each cell.

Now our script looks like this:

```
# filename: synth_cyclone.ys
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

Here we can see that Yosys has synthesised this to mostly 6-input LUTs, with smaller
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
synth -lut 6
read_verilog -lib cells_sim.v
techmap -map cells_map.v
stat -width
```

Note that Yosys is sensitive to the location of `cells_sim.v` and `cells_map.v`; if it can't find
it, try synthesising in the same directory as these files or use an absolute path to the file.

Running this synthesis script gives the following:

```
=== axilxbar ===

   Number of wires:               9518
   Number of wire bits:          16454
   Number of public wires:         514
   Number of public wire bits:    5818
   Number of memories:               0
   Number of memory bits:            0
   Number of processes:              0
   Number of cells:               6844
     $lut_2                        121
     $lut_3                        495
     $lut_4                        371
     $lut_5                       1158
     $lut_6                       2799
     MISTRAL_FF                   1900
```

Now that we know this, we can write a `techmap` pass for `$lut`. The ALM natively contains 3-input
and 4-input LUTs, so let's model those.

```
// append to filename: cells_sim.v
// based on the Trellis cells_sim LUTs
module MISTRAL_LUT3(input A, B, C, output Q);

parameter [7:0] LUT = 8'h000;

wire [3:0] s2 = C ? INIT[7:4] : INIT[3:0];
wire [1:0] s1 = B ? s2[3:2] : s2[1:0];
assign Q = A ? s1[1] : s1[0];

endmodule

module MISTRAL_LUT4(input A, B, C, D, output Q);

parameter [15:0] LUT = 16'h0000;

wire [7:0] s3 = D ? INIT[15:8] : INIT[7:0];
wire [3:0] s2 = C ? s3[7:4] : s3[3:0];
wire [1:0] s1 = B ? s2[3:2] : s2[1:0];
assign Q = A ? s1[1] : s1[0];

endmodule
```

And then we need to model a multiplexer to switch between LUT outputs.

```
// append to filename: cells_sim.v
module MISTRAL_MUX2(input A, B, S, output Q);

assign Q = S ? B : A;

endmodule
```

Finally, we can write the `techmap` pass, multiplexing together smaller LUTs into larger ones.

```
// append to filename: cells_map.v
module \$lut (A, Y);

parameter WIDTH = 0;
parameter LUT = 0;

input [WIDTH-1:0] A;
output Y;

generate
    if (WIDTH == 1) begin
        MISTRAL_LUT3 #(.LUT({8{LUT[0]}})) _TECHMAP_REPLACE_(
            .A(1'b1), .B(1'b1), .C(A[0]), .Q(Y)
        );
    end else
    if (WIDTH == 2) begin
        MISTRAL_LUT3 #(.LUT({4{LUT[1:0]}})) _TECHMAP_REPLACE_(
            .A(1'b1), .B(A[1]), .C(A[0]), .Q(Y)
        );
    end else
    if (WIDTH == 3) begin
        MISTRAL_LUT3 #(.LUT({2{LUT[3:0]}})) _TECHMAP_REPLACE_(
            .A(A[2]), .B(A[1]), .C(A[0]), .Q(Y)
        );
    end else
    if (WIDTH == 4) begin
        MISTRAL_LUT4 #(.LUT(LUT)) _TECHMAP_REPLACE_(
            .A(A[3]), .B(A[2]), .C(A[1]), .D(A[0]), .Q(Y)
        );
    end else
    if (WIDTH == 5) begin
        wire lut3_0_out, lut3_1_out;
        wire lut4_0_out, lut4_1_out;       
 
        MISTRAL_LUT3 #(.LUT(LUT[7:0])) lut3_0(
            .A(A[2]), .B(A[1]), .C(A[0]), .Q(lut3_0_out)
        );
        MISTRAL_LUT3 #(.LUT(LUT[15:8])) lut3_1(
            .A(A[2]), .B(A[1]), .C(A[0]), .Q(lut3_1_out)
        );
        MISTRAL_LUT4 #(.LUT(LUT[31:16])) lut4_1(
            .A(A[3]), .B(A[2]), .C(A[1]), .D(A[0]), .Q(lut4_0_out)
        );

        MISTRAL_MUX2 lut33_mux(
            .A(lut3_0_out), .B(lut3_1_out), .S(A[3]), .Q(lut4_1_out)
        );
        MISTRAL_MUX2 lut44_mux(
            .A(lut4_0_out), .B(lut4_1_out), .S(A[4]), .Q(Y)
        );
    end else
    if (WIDTH == 6) begin
        wire lut3_0_out;
        wire lut3_1_out;
        wire lut3_2_out;
        wire lut3_3_out;
        wire lut4_0_out;
        wire lut4_1_out;       
        wire lut4_2_out;
        wire lut4_3_out;
        wire lut5_0_out;
        wire lut5_1_out;
 
        MISTRAL_LUT3 #(.LUT(LUT[7:0])) lut3_0(
            .A(A[2]), .B(A[1]), .C(A[0]), .Q(lut3_0_out)
        );
        MISTRAL_LUT3 #(.LUT(LUT[15:8])) lut3_1(
            .A(A[2]), .B(A[1]), .C(A[0]), .Q(lut3_1_out)
        );
        MISTRAL_LUT4 #(.LUT(LUT[31:16])) lut4_1(
            .A(A[3]), .B(A[2]), .C(A[1]), .D(A[0]), .Q(lut4_0_out)
        );

        MISTRAL_MUX2 lut33_mux0(
            .A(lut3_0_out), .B(lut3_1_out), .S(A[3]), .Q(lut4_1_out)
        );
        MISTRAL_MUX2 lut44_mux0(
            .A(lut4_0_out), .B(lut4_1_out), .S(A[4]), .Q(lut5_0_out)
        );

        MISTRAL_LUT3 #(.LUT(LUT[39:32])) lut3_2(
            .A(A[2]), .B(A[1]), .C(A[0]), .Q(lut3_2_out)
        );
        MISTRAL_LUT3 #(.LUT(LUT[47:40])) lut3_3(
            .A(A[2]), .B(A[1]), .C(A[0]), .Q(lut3_3_out)
        );
        MISTRAL_LUT4 #(.LUT(LUT[63:48])) lut4_3(
            .A(A[3]), .B(A[2]), .C(A[1]), .D(A[0]), .Q(lut4_2_out)
        );

        MISTRAL_MUX2 lut33_mux1(
            .A(lut3_2_out), .B(lut3_3_out), .S(A[3]), .Q(lut4_3_out)
        );
        MISTRAL_MUX2 lut44_mux1(
            .A(lut4_0_out), .B(lut4_1_out), .S(A[4]), .Q(lut5_1_out)
        );
        
        MISTRAL_MUX2 lut55_mux(
            .A(lut5_0_out), .B(lut5_1_out), .S(A[5]), .Q(Y)
        );
    end else
        wire _TECHMAP_FAIL_ = 1'b1;
    end
endgenerate
endmodule
```

Setting `_TECHMAP_FAIL_` to `1'b1` in the `else` causes `techmap` to reject this pass if the pass
is asked to create a multiplexer wider than 6 inputs.

This results in the following:

```
=== axilxbar ===

   Number of wires:              52028
   Number of wire bits:          79815
   Number of public wires:         514
   Number of public wire bits:    5818
   Number of memories:               0
   Number of memory bits:            0
   Number of processes:              0
   Number of cells:              39466
     MISTRAL_FF                   1900
     MISTRAL_LUT3                14128
     MISTRAL_LUT4                 7127
     MISTRAL_MUX2                16311
```

The number of cells has shot way up, but that's acceptable for a first test. The problem here is
that the pass responsible for mapping to LUTs - `abc` - thinks LUT6s are as cheap as LUT4s, and
thus creates LUTs that are far too big. To fix this, we need to sidestep part of `synth` and handle
things ourselves.

```
# filename: synth_cyclone.v
synth -run :fine
opt -fast -full
memory_map
opt -full
techmap
opt -fast
abc -luts 1,1,1,2,6,12 -dress
opt -fast
read_verilog -lib cells_sim.v
techmap -map cells_map.v
stat -width
```

Here, we run `synth` until the `fine` label, then we run the contents of `fine` but pass custom
arguments to `abc`. The weights in `abc -luts` are meant to prefer using LUT3s over LUT4s, and
discourage unnecessarily large LUTs.

After this, we get:

```
=== axilxbar ===

   Number of wires:              33865
   Number of wire bits:          61025
   Number of public wires:         514
   Number of public wire bits:    5818
   Number of memories:               0
   Number of memory bits:            0
   Number of processes:              0
   Number of cells:              13485
     MISTRAL_FF                   1900
     MISTRAL_LUT3                 6189
     MISTRAL_LUT4                 4030
     MISTRAL_MUX2                 1366
```

Which is a lot more reasonable.
