module MISTRAL_FF(input D, CLK, output reg Q);

always @(posedge CLK)
    Q <= D;

endmodule
