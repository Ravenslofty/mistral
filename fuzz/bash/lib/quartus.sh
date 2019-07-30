#!/bin/bash

set -e

initialise() {
    # shellcheck disable=1091
    source CONFIG || (echo "$PWD/CONFIG does not exist; please create it"; exit 1)

    # $quartus_path is defined in CONFIG; sanity check it.
    # shellcheck disable=2154
    test -n "$quartus_path" ||
        (echo "CONFIG does not set \$quartus_path; please set it to the bin64 directory of your Quartus installation."; exit 1)
    test -e "$quartus_path" ||
        (echo "\$quartus_path points to a nonexistent path in CONFIG; please set it to the bin64 directory of your Quartus installation."; exit 1)
    test -d "$quartus_path" ||
        (echo "\$quartus_path is not a directory; please set it to the bin64 directory of your Quartus installation."; exit 1)

    # $extension is defined in CONFIG.
    # shellcheck disable=2154
    QUARTUS_SH="$quartus_path/quartus_sh$extension"
    QUARTUS_MAP="$quartus_path/quartus_map$extension"
    QUARTUS_FIT="$quartus_path/quartus_fit$extension"
    QUARTUS_ASM="$quartus_path/quartus_asm$extension"
    QUARTUS_CPF="$quartus_path/quartus_cpf$extension"

    (test -f "$QUARTUS_SH" && test -f "$QUARTUS_MAP" && test -f "$QUARTUS_FIT" && test -f "$QUARTUS_ASM") ||
        (echo "\$quartus_path does not look like a valid Quartus installation; please set it to the bin64 directory of your Quartus installation."; exit 1)

    export QUARTUS_SH QUARTUS_MAP QUARTUS_FIT QUARTUS_ASM QUARTUS_CPF

    export target_device

    INITIALISED=1
    export INITIALISED
}
