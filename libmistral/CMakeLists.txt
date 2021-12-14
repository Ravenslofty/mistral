set(COMPRESSION_LEVEL 1 CACHE STRING "Internal data compression level (0=no compression, 1=good, 6=smaller but slow)")

list(APPEND mistral_SOURCES
    cv-bmux-data.cc
    cv-bmux.cc
    cv-files.cc
    cv-inv.cc
    cv-opt.cc
    cv-pkg.cc
    cv-rmux.cc
    cv-rpats.cc
    cyclonev.cc

    bdz-ph.cc
    mistral-analogsim.cc
)

set(TOOLS ${CMAKE_CURRENT_SOURCE_DIR}/../tools)
set(DATA ${CMAKE_CURRENT_SOURCE_DIR}/../data)

find_package(PythonInterp 3.5 REQUIRED)

# Block muxes data

add_custom_command(
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cv-bmux-data.cc
   COMMAND ${PYTHON_EXECUTABLE} ${TOOLS}/mux_to_source.py mux ${DATA} > ${CMAKE_CURRENT_BINARY_DIR}/cv-bmux-data.cc
   DEPENDS ${TOOLS}/mux_to_source.py ${DATA}/cbuf-mux.txt ${DATA}/cmuxc-mux.txt ${DATA}/cmuxh-mux.txt ${DATA}/cmuxv-mux.txt ${DATA}/dll-mux.txt ${DATA}/dqs16-mux.txt ${DATA}/dsp-mux.txt ${DATA}/fpll-mux.txt ${DATA}/gpio-mux.txt ${DATA}/hip-mux.txt ${DATA}/hmc-mux.txt ${DATA}/hps_clocks-mux.txt ${DATA}/hssi-mux.txt ${DATA}/lab-mux.txt ${DATA}/lvl-mux.txt ${DATA}/m10k-mux.txt ${DATA}/mlab-mux.txt ${DATA}/opt-mux.txt ${DATA}/pma3-c-mux.txt ${DATA}/pma3-mux.txt ${DATA}/serpar-mux.txt ${DATA}/term-mux.txt
)
list(APPEND mistral_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/cv-bmux-data.cc)

# Models generation

add_custom_command(
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cv-models.cc
   COMMAND ${PYTHON_EXECUTABLE} ${TOOLS}/models_to_source.py ${DATA} > ${CMAKE_CURRENT_BINARY_DIR}/cv-models.cc
   DEPENDS ${TOOLS}/models_to_source.py ${DATA}/models.txt
)
list(APPEND mistral_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/cv-models.cc)


# C++ source generation/inclusion
foreach(die e50f gx25f gt75f gt150f gt300f sx50f sx120f)

    # per-die hand-made information

    list(APPEND mistral_SOURCES cvd-${die}.cc)


    # peripheral blocks information

    add_custom_command(
       OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-blk.cc
       COMMAND ${PYTHON_EXECUTABLE} ${TOOLS}/blocks_to_source.py ${DATA} ${die} > ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-blk.cc
       DEPENDS ${TOOLS}/blocks_to_source.py ${DATA}/${die}-pram.txt ${DATA}/pma3-cram.txt ${DATA}/${die}-1.txt ${DATA}/${die}-iob.txt ${DATA}/${die}-hmc.txt ${DATA}/${die}-fpll.txt ${DATA}/${die}-ctrl.txt ${DATA}/${die}-dll.txt ${DATA}/${die}-cmux.txt ${DATA}/${die}-hps.txt
    )
    list(APPEND mistral_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-blk.cc)


    # packages information

    add_custom_command(
       OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-pkg.cc
       COMMAND ${PYTHON_EXECUTABLE} ${TOOLS}/pkg_to_source.py ${DATA} ${die} > ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-pkg.cc
       DEPENDS ${TOOLS}/pkg_to_source.py ${DATA}/${die}-pkg.txt
    )
    list(APPEND mistral_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-pkg.cc)


    # inverters information

    add_custom_command(
       OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-inv.cc
       COMMAND ${PYTHON_EXECUTABLE} ${TOOLS}/inv_to_source.py ${DATA} ${die} > ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-inv.cc
       DEPENDS ${TOOLS}/inv_to_source.py ${DATA}/${die}-inv.txt
    )
    list(APPEND mistral_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-inv.cc)

endforeach()


# Binary routing/timing information generation

foreach(die e50f gx25f gt75f gt150f gt300f sx50f sx120f)
    add_custom_command(
       OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${die}-r.bin
       COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../generator/generator ${DATA} ${die} ${CMAKE_CURRENT_BINARY_DIR} ${COMPRESSION_LEVEL}
       DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/../generator/generator ${DATA}/${die}-r.txt.xz ${DATA}/${die}-l.txt.xz
    )

    add_custom_command(
       OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-r.o
       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
       COMMAND ld -r -b binary -o cvd-${die}-r.o ${die}-r.bin
       DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${die}-r.bin
    )

    list(APPEND mistral_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-r.o)

endforeach()


# Binary global information generation

add_custom_command(
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/global.bin
   COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../generator/generator ${DATA} - ${CMAKE_CURRENT_BINARY_DIR} ${COMPRESSION_LEVEL}
   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/../generator/generator ${DATA}/driver-models.txt.xz
)

add_custom_command(
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cv-global.o
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   COMMAND ld -r -b binary -o cv-global.o global.bin
   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/global.bin
)

list(APPEND mistral_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/cv-global.o)


add_library(mistral ${mistral_SOURCES})
add_dependencies(mistral tools)
add_dependencies(mistral generator)
target_include_directories(mistral PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/../tools ${CMAKE_CURRENT_SOURCE_DIR} /toto)


if(NOT MISTRAL_DONT_INSTALL)

install(TARGETS mistral DESTINATION lib)

install(FILES cyclonev.h DESTINATION include)
install(FILES mistral-analogsim.h DESTINATION include)
install(FILES cv-blocktypes.ipp DESTINATION include)
install(FILES cv-rnodetypes.ipp DESTINATION include)

endif()
