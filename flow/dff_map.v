module \$_DFF_P_ (input D, C, output Q); MISTRAL_FF _TECHMAP_REPLACE_(.D(D), .CLK(C), .AC(1'b0), .Q(Q)); endmodule
module \$_DFF_N_ (input D, C, output Q); MISTRAL_FF _TECHMAP_REPLACE_(.D(D), .CLK(~C), .AC(1'b0), .Q(Q)); endmodule

module \$_DFF_PP0_ (input D, C, R, output Q); MISTRAL_FF _TECHMAP_REPLACE_(.D(D), .CLK(C), .AC(R), .Q(Q)); endmodule
module \$_DFF_PP1_ (input D, C, R, output Q); MISTRAL_FF _TECHMAP_REPLACE_(.D(~D), .CLK(C), .AC(R), .Q(~Q)); endmodule
module \$_DFF_PN0_ (input D, C, R, output Q); MISTRAL_FF _TECHMAP_REPLACE_(.D(D), .CLK(C), .AC(~R), .Q(Q)); endmodule
module \$_DFF_PN1_ (input D, C, R, output Q); MISTRAL_FF _TECHMAP_REPLACE_(.D(~D), .CLK(C), .AC(~R), .Q(~Q)); endmodule
module \$_DFF_NP0_ (input D, C, R, output Q); MISTRAL_FF _TECHMAP_REPLACE_(.D(D), .CLK(~C), .AC(R), .Q(Q)); endmodule
module \$_DFF_NP1_ (input D, C, R, output Q); MISTRAL_FF _TECHMAP_REPLACE_(.D(~D), .CLK(~C), .AC(R), .Q(~Q)); endmodule
module \$_DFF_NN0_ (input D, C, R, output Q); MISTRAL_FF _TECHMAP_REPLACE_(.D(D), .CLK(~C), .AC(~R), .Q(Q)); endmodule
module \$_DFF_NN1_ (input D, C, R, output Q); MISTRAL_FF _TECHMAP_REPLACE_(.D(~D), .CLK(~C), .AC(~R), .Q(~Q)); endmodule
