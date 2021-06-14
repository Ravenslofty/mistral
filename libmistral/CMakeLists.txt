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
)

set(TOOLS ${CMAKE_CURRENT_SOURCE_DIR}/../tools)
set(DATA ${CMAKE_CURRENT_SOURCE_DIR}/../data)

find_package(PythonInterp 3.5 REQUIRED)

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
       COMMAND ${PYTHON_EXECUTABLE} ${TOOLS}/routes-to-bin.py ${DATA} ${CMAKE_CURRENT_SOURCE_DIR}/cv-rnodetypes.ipp ${die} ${CMAKE_CURRENT_BINARY_DIR}
       DEPENDS ${TOOLS}/routes-to-bin.py ${DATA}/${die}-r.txt.xz
    )

    add_custom_command(
       OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cvd-${die}-r.o
       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
       COMMAND ld -r -b binary -o cvd-${die}-r.o ${die}-r.bin
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
