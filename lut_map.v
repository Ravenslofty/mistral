module \$lut (A, Y);

parameter WIDTH = 1;
parameter LUT = 0;

input [WIDTH-1:0] A;
output Y;

generate
    if (WIDTH == 1) begin
        MISTRAL_LUT4 #(.LUT(LUT)) _TECHMAP_REPLACE_(
            .A(1'b1), .B(1'b1), .C(1'b1), .D(A[0]), .Q(Y)
        );
    end else
    if (WIDTH == 2) begin
        MISTRAL_LUT4 #(.LUT(LUT)) _TECHMAP_REPLACE_(
            .A(1'b1), .B(1'b1), .C(A[1]), .D(A[0]), .Q(Y)
        );
    end else
    if (WIDTH == 3) begin
        MISTRAL_LUT4 #(.LUT(LUT)) _TECHMAP_REPLACE_(
            .A(1'b1), .B(A[2]), .C(A[1]), .D(A[0]), .Q(Y)
        );
    end else
    if (WIDTH == 4) begin
        MISTRAL_LUT4 #(.LUT(LUT)) _TECHMAP_REPLACE_(
            .A(A[3]), .B(A[2]), .C(A[1]), .D(A[0]), .Q(Y)
        );
    end else
    if (WIDTH == 5) begin
        wire lut4_0_out, lut4_1_out;

        MISTRAL_LUT4 #(.LUT(LUT[15:0])) lut4_0(
            .A(A[3]), .B(A[2]), .C(A[1]), .D(A[0]), .Q(lut4_0_out)
        );
        MISTRAL_LUT4 #(.LUT(LUT[31:16])) lut4_1(
            .A(A[3]), .B(A[2]), .C(A[1]), .D(A[0]), .Q(lut4_1_out)
        );

        MISTRAL_MUX2 lut44_mux(
            .A(lut4_0_out), .B(lut4_1_out), .S(A[4]), .Q(Y)
        );
    end else
    if (WIDTH == 6) begin
        wire lut4_0_out;
        wire lut4_1_out;
        wire lut4_2_out;
        wire lut4_3_out;
        wire lut5_0_out;
        wire lut5_1_out;

        MISTRAL_LUT4 #(.LUT(LUT[15:0])) lut4_0(
            .A(A[3]), .B(A[2]), .C(A[1]), .D(A[0]), .Q(lut4_0_out)
        );
        MISTRAL_LUT4 #(.LUT(LUT[31:16])) lut4_1(
            .A(A[3]), .B(A[2]), .C(A[1]), .D(A[0]), .Q(lut4_1_out)
        );

        MISTRAL_MUX2 lut44_mux0(
            .A(lut4_0_out), .B(lut4_1_out), .S(A[4]), .Q(lut5_0_out)
        );

        MISTRAL_LUT4 #(.LUT(LUT[47:0])) lut4_2(
            .A(A[3]), .B(A[2]), .C(A[1]), .D(A[0]), .Q(lut4_2_out)
        );
        MISTRAL_LUT4 #(.LUT(LUT[63:48])) lut4_3(
            .A(A[3]), .B(A[2]), .C(A[1]), .D(A[0]), .Q(lut4_3_out)
        );

        MISTRAL_MUX2 lut44_mux1(
            .A(lut4_2_out), .B(lut4_3_out), .S(A[4]), .Q(lut5_1_out)
        );

        MISTRAL_MUX2 lut55_mux(
            .A(lut5_0_out), .B(lut5_1_out), .S(A[5]), .Q(Y)
        );
    end else begin
        wire _TECHMAP_FAIL_ = 1'b1;
    end
endgenerate
endmodule
