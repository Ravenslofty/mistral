list(APPEND mistral_SOURCES
    cv-bmux-data.cc
    cv-bmux.cc
    cv-files.cc
    cv-inv.cc
    cv-models.cc
    cv-opt.cc
    cv-pkg.cc
    cv-rmux.cc
    cv-rpats.cc
    cyclonev.cc
)

set(TOOLS ${CMAKE_CURRENT_SOURCE_DIR}/../tools)
set(DATA ${CMAKE_CURRENT_SOURCE_DIR}/../data)

# C++ source generation/inclusion
foreach(die e50f gx25f gt75f gt150f gt300f sx50f sx120f)

    # per-die hand-made information

    list(APPEND mistral_SOURCES cvd-${die}.cc)


    # peripheral blocks information

    add_custom_command(
       OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-blk.cc
       COMMAND python3 ${TOOLS}/blocks_to_source.py ${DATA} ${die} > ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-blk.cc
       DEPENDS ${TOOLS}/blocks_to_source.py ${DATA}/${die}-pram.txt ${DATA}/pma3-cram.txt ${DATA}/${die}-1.txt ${DATA}/${die}-iob.txt ${DATA}/${die}-hmc.txt ${DATA}/${die}-fpll.txt ${DATA}/${die}-ctrl.txt ${DATA}/${die}-dll.txt ${DATA}/${die}-cmux.txt ${DATA}/${die}-hps.txt
    )
    list(APPEND mistral_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-blk.cc)


    # packages information

    add_custom_command(
       OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-pkg.cc
       COMMAND python3 ${TOOLS}/pkg_to_source.py ${DATA} ${die} > ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-pkg.cc
       DEPENDS ${TOOLS}/pkg_to_source.py ${DATA}/${die}-pkg.txt
    )
    list(APPEND mistral_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-pkg.cc)


    # inverters information

    add_custom_command(
       OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-inv.cc
       COMMAND python3 ${TOOLS}/inv_to_source.py ${DATA} ${die} > ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-inv.cc
       DEPENDS ${TOOLS}/inv_to_source.py ${DATA}/${die}-inv.txt
    )
    list(APPEND mistral_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-inv.cc)

endforeach()


# Binary routing/timing information generation

execute_process(COMMAND "${CMAKE_CXX_COMPILER}" "-dumpmachine" OUTPUT_VARIABLE machine ERROR_VARIABLE machine)

set(GDATA ${CMAKE_CURRENT_SOURCE_DIR}/../gdata)

foreach(die e50f gx25f gt75f gt150f gt300f sx50f sx120f)
    add_custom_command(
       OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${die}-r.bin
       COMMAND ${TOOLS}/routes-to-bin.py ${DATA} ${CMAKE_CURRENT_SOURCE_DIR}/cv-rnodetypes.ipp ${die} ${CMAKE_CURRENT_BINARY_DIR}
       DEPENDS ${TOOLS}/routes-to-bin.py ${DATA}/${die}-r.txt.xz
    )

    add_custom_command(
       OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-r.o
       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
       COMMAND objcopy ${die}-r.bin cvd-${die}-r.o --input-target binary --output-target ${machine}
       DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${die}-r.bin
    )

    list(APPEND mistral_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-r.o)

endforeach()


add_library(mistral ${mistral_SOURCES})

target_include_directories(mistral PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

if(NOT MISTRAL_DONT_INSTALL)

install(TARGETS mistral DESTINATION lib)

install(FILES cyclonev.h DESTINATION include)
install(FILES cv-blocktypes.ipp DESTINATION include)
install(FILES cv-bmuxtypes.ipp DESTINATION include)
install(FILES cv-bmuxtypes.ipp DESTINATION include)
install(FILES cv-porttypes.ipp DESTINATION include)
install(FILES cv-rnodetypes.ipp DESTINATION include)

endif()
