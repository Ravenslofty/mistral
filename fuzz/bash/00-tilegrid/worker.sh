#!/bin/bash

# shellcheck disable=1091,2154
source ../lib/quartus.sh

test -n "$INITIALISED" || initialise

build() {
    TARGET=$1
    LUT=$2

    mkdir -p "$TARGET"
    cp test.vqm "$TARGET"
    cd "$TARGET" || (echo "can't cd to $TARGET"; exit 1)

cat > "$TARGET.qsf" <<EOF
set_global_assignment -name FAMILY "Cyclone V"
set_global_assignment -name DEVICE $target_device
set_global_assignment -name TOP_LEVEL_ENTITY top
set_global_assignment -name NUM_PARALLEL_PROCESSORS 1
set_global_assignment -name VERILOG_FILE "test.vqm"
set_global_assignment -name PROJECT_OUTPUT_DIRECTORY obj
set_location_assignment $TARGET -to lut
EOF

cat > "lut.vh" <<EOF
\`define LUT_MASK 64'h$LUT
EOF

    $QUARTUS_MAP --rev="$TARGET" "$TARGET"
    $QUARTUS_FIT --rev="$TARGET" "$TARGET"
    $QUARTUS_ASM --rev="$TARGET" "$TARGET"
    $QUARTUS_CPF --convert "obj/$TARGET.sof" "../out/$TARGET-$LUT.rbf"

    # Quartus will complain if we try to use a field which does not exist; detect and skip it.
    ILLEGAL_POS=$(grep -F "Error (171016): Can't place node" && echo "yes")

    cd ..
    rm -rf "$TARGET"

    test -n "$ILLEGAL_POS" && exit 1
}

wrap_build() {
    pushd .
    echo "$1: $2"
    build "$1" "$2" > /dev/null
    popd
}

mkdir -p out
wrap_build "$1" "FFFFFFFFFFFFFFFF" 
wrap_build "$1" "FFFFFFFF00000000"
wrap_build "$1" "FFFF0000FFFF0000"
wrap_build "$1" "FF00FF00FF00FF00"
wrap_build "$1" "F0F0F0F0F0F0F0F0"
wrap_build "$1" "CCCCCCCCCCCCCCCC"
wrap_build "$1" "5555555555555555"
