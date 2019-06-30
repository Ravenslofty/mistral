// TODO: shared arithmetic mode
module \$alu (A, B, CI, BI, X, Y, CO);

parameter A_SIGNED = 0;
parameter B_SIGNED = 0;
parameter A_WIDTH = 2;
parameter B_WIDTH = 2;
parameter Y_WIDTH = 2;

input [A_WIDTH-1:0] A;
input [B_WIDTH-1:0] B;
output [Y_WIDTH-1:0] X, Y;

input CI, BI;
output [Y_WIDTH-1:0] CO;

wire [Y_WIDTH-1:0] AA, BB;

\$pos #(.A_SIGNED(A_SIGNED), .A_WIDTH(A_WIDTH), .Y_WIDTH(Y_WIDTH)) A_conv (.A(A), .Y(AA));
\$pos #(.A_SIGNED(B_SIGNED), .A_WIDTH(B_WIDTH), .Y_WIDTH(Y_WIDTH)) B_conv (.A(BI ? ~B : B), .Y(BB));

localparam A_HIGH_LUT = 16'b1010_1010_1010_1010;
localparam B_HIGH_LUT = 16'b1100_1100_1100_1100;
localparam C_HIGH_LUT = 16'b1111_0000_1111_0000;
localparam D_HIGH_LUT = 16'b1111_1111_0000_0000;

wire [Y_WIDTH:0] CARRY;

assign CARRY[0] = CI;
assign CARRY[Y_WIDTH] = CO;

genvar i;
generate for (i = 0; i < Y_WIDTH; i = i + 2) begin
    MISTRAL_CARRY carry(
        .A(AA[i]),
        .B(BB[i]),
        .C(AA[i+1]),
        .D(BB[i+1]),
        .Cin(CARRY[i]),
        .Cout(CARRY[i+2]),
        .S(Y[i]),
        .T(Y[i+1])
    );
end
endgenerate

assign X = AA ^ BB;

endmodule
