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

wire [Y_WIDTH:0] INTERNAL_CARRY;

genvar i;
generate for (i = 0; i < Y_WIDTH; i = i + 2) begin:slice
    // Hard logic for internal addition.
    MISTRAL_CARRY carry(
        .A(AA[i]),
        .B(BB[i]),
        .C(AA[i+1]),
        .D(BB[i+1]),
        .Cin(INTERNAL_CARRY[i]),
        .Cout(INTERNAL_CARRY[i+2]),
        .S(Y[i]),
        .T(Y[i+1])
    );
end
endgenerate

assign X = AA ^ BB;

endmodule
