module MISTRAL_FF(input D, CLK, AC, output reg Q);

always @(posedge CLK or posedge AC)
    if (AC)
        Q <= 0;
    else
        Q <= D;

endmodule

module MISTRAL_LUT4(input A, B, C, D, output Q);

parameter [15:0] LUT = 16'h0000;

wire [7:0] s3 = D ? LUT[15:8] : LUT[7:0];
wire [3:0] s2 = C ? s3[7:4] : s3[3:0];
wire [1:0] s1 = B ? s2[3:2] : s2[1:0];
assign Q = A ? s1[1] : s1[0];

endmodule

module MISTRAL_MUX2(input A, B, S, output Q);

assign Q = S ? B : A;

endmodule

module MISTRAL_CARRY(input A, B, C, D, Cin, output S, T, Cout);

wire carry;
assign {carry, S} = A + B + Cin;
assign {Cout, T} = C + D + carry;

endmodule
