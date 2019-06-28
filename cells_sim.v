module MISTRAL_FF(input D, CLK, output reg Q);

always @(posedge CLK)
    Q <= D;

endmodule

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

module MISTRAL_MUX2(input A, B, S, output Q);

assign Q = S ? B : A;

endmodule
