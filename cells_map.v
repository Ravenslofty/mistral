module \$_DFF_P_ (input D, C, output Q); MISTRAL_FF _TECHMAP_REPLACE_(.D(D), .CLK(C), .Q(Q)); endmodule

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
    end else begin
        wire _TECHMAP_FAIL_ = 1'b1;
    end
endgenerate
endmodule
