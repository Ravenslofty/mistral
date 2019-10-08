#!/bin/bash

# shellcheck disable=1091,2154
source ../lib/quartus.sh

test -n "$INITIALISED" || initialise

build() {
    TARGET=$1
    LUT=$2

    # Don't recompile object files that have already been built.
    test -f "out/$TARGET-$LUT.rbf" && return 0

    mkdir -p "$TARGET-$LUT"
    cp test.vqm "$TARGET-$LUT"
    cd "$TARGET-$LUT" || (echo "can't cd to $TARGET-$LUT"; exit 1)

cat > "$TARGET.qsf" <<EOF
set_global_assignment -name FAMILY "Cyclone V"
set_global_assignment -name DEVICE $target_device
set_global_assignment -name TOP_LEVEL_ENTITY top
set_global_assignment -name NUM_PARALLEL_PROCESSORS 1
set_global_assignment -name VERILOG_FILE "test.vqm"
set_global_assignment -name PROJECT_OUTPUT_DIRECTORY obj
set_global_assignment -name GENERATE_RBF_FILE ON
set_location_assignment $TARGET -to lut
set_location_assignment IOOBUF_X11_Y0_N2 -to x~output
set_location_assignment PIN_L7 -to x
set_instance_assignment -name IO_STANDARD "2.5 V" -to x
set_location_assignment IOIBUF_X10_Y0_N58 -to a~input
set_location_assignment PIN_N4 -to a
set_instance_assignment -name IO_STANDARD "2.5 V" -to a
set_location_assignment IOIBUF_X11_Y0_N52 -to b~input
set_location_assignment PIN_R1 -to b
set_instance_assignment -name IO_STANDARD "2.5 V" -to b
set_location_assignment IOIBUF_X11_Y0_N18 -to c~input
set_location_assignment PIN_M7 -to c
set_instance_assignment -name IO_STANDARD "2.5 V" -to c
set_location_assignment IOIBUF_X10_Y0_N92 -to d~input
set_location_assignment PIN_N1 -to d
set_instance_assignment -name IO_STANDARD "2.5 V" -to d
set_location_assignment IOIBUF_X10_Y0_N41 -to e~input
set_location_assignment PIN_P4 -to e
set_instance_assignment -name IO_STANDARD "2.5 V" -to e
set_location_assignment IOIBUF_X10_Y0_N75 -to f~input
set_location_assignment PIN_P1 -to f
set_instance_assignment -name IO_STANDARD "2.5 V" -to f
set_location_assignment LABCELL_X1_Y34_N3 -to ~QUARTUS_CREATED_GND~I
set_global_assignment -name ROUTING_BACK_ANNOTATION_FILE $TARGET.rcf
EOF

cat > "lut.vh" <<EOF
\`define LUT_MASK 64'h$LUT
EOF

cat > "$TARGET.rcf" <<EOF
### Routing Constraints File: $TARGET.rcf
### Written on:                         Tue Oct 08 09:33:35 2019
### Written by:                         Version 18.1.1 Build 646 04/11/2019 SJ Lite Edition

section global_data {
        rcf_written_by = "Quartus Prime 18.1 Build 646";
        device = $target_device;
    }

signal_name = lut {
        LE_BUFFER:X1Y1S0I0;
        R6:X2Y1S0I1;
        R3:X7Y1S0I0;
        R3:X9Y1S0I1;
        C2:X10Y0S0I26;
        LOCAL_INTERCONNECT:X11Y0S0I23;
        BLOCK_INPUT_MUX_PASSTHROUGH:X11Y0S0I44;
        IO_RE:X11Y0S0B:IOC~INPUT~NIOD0OUT[0];
        IO_OUTPUT_PATH_RE:X11Y0S8B:IOOUTPUT~INPUT~IODOUT[0];
        IO_OUTPUT_PATH_RE:X11Y0S8B:IOOUTPUT~RE~RAW_OUT_PRE_DCHAIN_MUX[0];
        IO_OUTPUT_PATH_RE:X11Y0S8B:IOOUTPUT~OUTPUT~RAW_OUT[0];
        IO_RE:X11Y0S0B:IOC~RE~OPATH_PLLDIN_MUX[0];
        IO_OUTPUT_BUFFER_INPUT:X11Y0S2B:IOOBUF~INPUT~BUFFER_IN[0];
        dest = ( x~output, I );
    }
signal_name = x~output {
        IO_OUTPUT_BUFFER_OUTPUT:X11Y0S2B:IOOBUF~OUTPUT~BUFFER_OUT[0];
        IO_PAD_INPUT:X11Y0S0B:IOPAD~INPUT~PAD_IN[0];
        dest = ( x, PADIN );
    }
signal_name = a~input {
        IO_INPUT_BUFFER_OUTPUT:X10Y0S58B:IOIBUF~OUTPUT~BUFFER_OUT[0];
        IO_RE:X10Y0S57B:IOC~RE~DIN[0];
        IO_INPUT_PATH_RE:X10Y0S70B:IOINPUT~INPUT~BUFFER_IN_RAW[0];
        IO_INPUT_PATH_RE:X10Y0S70B:IOINPUT~RE~T3_DCHAIN_TOP[0];
        IO_INPUT_PATH_RE:X10Y0S70B:IOINPUT~RE~CDATAIN_MUX[0];
        IO_INPUT_PATH_RE:X10Y0S70B:IOINPUT~OUTPUT~CDATAIN[0];
        IO_RE:X10Y0S57B:IOC~OUTPUT~CDATA0IN[0];
        VIO_BUFFER:X10Y0S0I24;
        C2:X10Y1S0I11;
        R6:X5Y1S0I32;
        R6:X0Y1S0I199;
        LOCAL_INTERCONNECT:X1Y1S0I5;
        BLOCK_INPUT_MUX:X1Y1S0I0;
        dest = ( lut, DATAA ), route_port = DATAE;
    }
signal_name = a {
        IO_PAD_OUTPUT:X10Y0S57B:IOPAD~OUTPUT~PAD_OUT[0];
        IO_INPUT_BUFFER_INPUT:X10Y0S58B:IOIBUF~INPUT~DATA[0];
        dest = ( a~input, I );
    }
signal_name = b~input {
        IO_INPUT_BUFFER_OUTPUT:X11Y0S52B:IOIBUF~OUTPUT~BUFFER_OUT[0];
        IO_RE:X11Y0S51B:IOC~RE~DIN[0];
        IO_INPUT_PATH_RE:X11Y0S64B:IOINPUT~INPUT~BUFFER_IN_RAW[0];
        IO_INPUT_PATH_RE:X11Y0S64B:IOINPUT~RE~T3_DCHAIN_BOTTOM[0];
        IO_INPUT_PATH_RE:X11Y0S64B:IOINPUT~RE~CDATAIN_MUX[1];
        IO_INPUT_PATH_RE:X11Y0S64B:IOINPUT~OUTPUT~CDATAIN[1];
        IO_RE:X11Y0S51B:IOC~OUTPUT~CDATA1IN[0];
        VIO_BUFFER:X11Y0S0I3;
        C2:X10Y1S0I10;
        R6:X5Y1S0I28;
        R6:X0Y1S0I145;
        LOCAL_INTERCONNECT:X1Y1S0I8;
        BLOCK_INPUT_MUX:X1Y1S0I1;
        dest = ( lut, DATAB ), route_port = DATAF;
    }
signal_name = b {
        IO_PAD_OUTPUT:X11Y0S51B:IOPAD~OUTPUT~PAD_OUT[0];
        IO_INPUT_BUFFER_INPUT:X11Y0S52B:IOIBUF~INPUT~DATA[0];
        dest = ( b~input, I );
    }
signal_name = c~input {
        IO_INPUT_BUFFER_OUTPUT:X11Y0S18B:IOIBUF~OUTPUT~BUFFER_OUT[0];
        IO_RE:X11Y0S17B:IOC~RE~DIN[0];
        IO_INPUT_PATH_RE:X11Y0S30B:IOINPUT~INPUT~BUFFER_IN_RAW[0];
        IO_INPUT_PATH_RE:X11Y0S30B:IOINPUT~RE~T3_DCHAIN_BOTTOM[0];
        IO_INPUT_PATH_RE:X11Y0S30B:IOINPUT~RE~CDATAIN_MUX[1];
        IO_INPUT_PATH_RE:X11Y0S30B:IOINPUT~OUTPUT~CDATAIN[1];
        IO_RE:X11Y0S17B:IOC~OUTPUT~CDATA1IN[0];
        VIO_BUFFER:X11Y0S0I23;
        C2:X11Y1S0I4;
        R6:X6Y1S0I21;
        R6:X0Y1S0I120;
        LOCAL_INTERCONNECT:X1Y1S0I0;
        BLOCK_INPUT_MUX:X1Y1S0I5;
        dest = ( lut, DATAC ), route_port = DATAD;
    }
signal_name = c {
        IO_PAD_OUTPUT:X11Y0S17B:IOPAD~OUTPUT~PAD_OUT[0];
        IO_INPUT_BUFFER_INPUT:X11Y0S18B:IOIBUF~INPUT~DATA[0];
        dest = ( c~input, I );
    }
signal_name = d~input {
        IO_INPUT_BUFFER_OUTPUT:X10Y0S92B:IOIBUF~OUTPUT~BUFFER_OUT[0];
        IO_RE:X10Y0S91B:IOC~RE~DIN[0];
        IO_INPUT_PATH_RE:X10Y0S104B:IOINPUT~INPUT~BUFFER_IN_RAW[0];
        IO_INPUT_PATH_RE:X10Y0S104B:IOINPUT~RE~T3_DCHAIN_TOP[0];
        IO_INPUT_PATH_RE:X10Y0S104B:IOINPUT~RE~CDATAIN_MUX[0];
        IO_INPUT_PATH_RE:X10Y0S104B:IOINPUT~OUTPUT~CDATAIN[0];
        IO_RE:X10Y0S91B:IOC~OUTPUT~CDATA0IN[0];
        VIO_BUFFER:X10Y0S0I2;
        C2:X9Y1S0I11;
        R3:X7Y1S0I21;
        R6:X1Y1S0I35;
        LOCAL_INTERCONNECT:X1Y1S0I10;
        BLOCK_INPUT_MUX:X1Y1S0I2;
        dest = ( lut, DATAD ), route_port = DATAA;
    }
signal_name = d {
        IO_PAD_OUTPUT:X10Y0S91B:IOPAD~OUTPUT~PAD_OUT[0];
        IO_INPUT_BUFFER_INPUT:X10Y0S92B:IOIBUF~INPUT~DATA[0];
        dest = ( d~input, I );
    }
signal_name = e~input {
        IO_INPUT_BUFFER_OUTPUT:X10Y0S41B:IOIBUF~OUTPUT~BUFFER_OUT[0];
        IO_RE:X10Y0S40B:IOC~RE~DIN[0];
        IO_INPUT_PATH_RE:X10Y0S53B:IOINPUT~INPUT~BUFFER_IN_RAW[0];
        IO_INPUT_PATH_RE:X10Y0S53B:IOINPUT~RE~T3_DCHAIN_TOP[0];
        IO_INPUT_PATH_RE:X10Y0S53B:IOINPUT~RE~CDATAIN_MUX[0];
        IO_INPUT_PATH_RE:X10Y0S53B:IOINPUT~OUTPUT~CDATAIN[0];
        IO_RE:X10Y0S40B:IOC~OUTPUT~CDATA0IN[0];
        VIO_BUFFER:X10Y0S0I18;
        C2:X10Y1S0I8;
        R6:X5Y1S0I34;
        R6:X0Y1S0I193;
        LOCAL_INTERCONNECT:X1Y1S0I35;
        BLOCK_INPUT_MUX:X1Y1S0I3;
        dest = ( lut, DATAE ), route_port = DATAB;
    }
signal_name = e {
        IO_PAD_OUTPUT:X10Y0S40B:IOPAD~OUTPUT~PAD_OUT[0];
        IO_INPUT_BUFFER_INPUT:X10Y0S41B:IOIBUF~INPUT~DATA[0];
        dest = ( e~input, I );
    }
signal_name = f~input {
        IO_INPUT_BUFFER_OUTPUT:X10Y0S75B:IOIBUF~OUTPUT~BUFFER_OUT[0];
        IO_RE:X10Y0S74B:IOC~RE~DIN[0];
        IO_INPUT_PATH_RE:X10Y0S87B:IOINPUT~INPUT~BUFFER_IN_RAW[0];
        IO_INPUT_PATH_RE:X10Y0S87B:IOINPUT~RE~T3_DCHAIN_TOP[0];
        IO_INPUT_PATH_RE:X10Y0S87B:IOINPUT~RE~CDATAIN_MUX[0];
        IO_INPUT_PATH_RE:X10Y0S87B:IOINPUT~OUTPUT~CDATAIN[0];
        IO_RE:X10Y0S74B:IOC~OUTPUT~CDATA0IN[0];
        VIO_BUFFER:X10Y0S0I8;
        C2:X9Y1S0I1;
        R6:X4Y1S0I38;
        R6:X0Y1S0I205;
        LOCAL_INTERCONNECT:X1Y1S0I26;
        BLOCK_INPUT_MUX:X1Y1S0I4;
        dest = ( lut, DATAF ), route_port = DATAC;
    }
signal_name = f {
        IO_PAD_OUTPUT:X10Y0S74B:IOPAD~OUTPUT~PAD_OUT[0];
        IO_INPUT_BUFFER_INPUT:X10Y0S75B:IOIBUF~INPUT~DATA[0];
        dest = ( f~input, I );
    }
EOF

    $QUARTUS_MAP --rev="$TARGET" "$TARGET"
    $QUARTUS_FIT --rev="$TARGET" "$TARGET"
    $QUARTUS_ASM --rev="$TARGET" "$TARGET"
    $QUARTUS_CPF -c "obj/$TARGET.sof" "../out/$TARGET-$LUT.rbf"

    # Quartus will complain if we try to use a field which does not exist; detect and skip it.
    ILLEGAL_POS=$(grep -F "Error (171016): Can't place node" "obj/$TARGET.fit.rpt" && echo "yes" || echo "no")

    cd ..
    # Clean up after Quartus because they won't clean up after themselves.
    rm -rf "$TARGET-$LUT" /tmp/alt-*.dir

    # test "$ILLEGAL_POS" = "yes" || exit 1
}

echo "$1: $2"
build "$1" "$2" > /dev/null
