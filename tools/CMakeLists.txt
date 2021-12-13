find_package(PythonInterp 3.5 REQUIRED)

set(DATA ${CMAKE_CURRENT_SOURCE_DIR}/../data)

# Drivers enum

add_custom_command(
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cv-drivertypes.ipp
   COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/drivers_to_source.py ${DATA} driver-enum > ${CMAKE_CURRENT_BINARY_DIR}/cv-drivertypes.ipp
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/drivers_to_source.py ${DATA}/driver-types.txt
)
list(APPEND tools_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cv-drivertypes.ipp)

# Shapes enum

add_custom_command(
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cv-shapetypes.ipp
   COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/drivers_to_source.py ${DATA} shape-enum > ${CMAKE_CURRENT_BINARY_DIR}/cv-shapetypes.ipp
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/drivers_to_source.py ${DATA}/driver-types.txt
)
list(APPEND tools_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cv-shapetypes.ipp)

# Ports enum

add_custom_command(
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cv-porttypes.ipp
   COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/porttypes_to_source.py ${DATA} > ${CMAKE_CURRENT_BINARY_DIR}/cv-porttypes.ipp
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/porttypes_to_source.py ${DATA}/e50f-p2p.txt ${DATA}/e50f-p2r.txt ${DATA}/gt150f-p2p.txt ${DATA}/gt150f-p2r.txt ${DATA}/gt300f-p2p.txt ${DATA}/gt300f-p2r.txt ${DATA}/gt75f-p2p.txt ${DATA}/gt75f-p2r.txt ${DATA}/gx25f-p2p.txt ${DATA}/gx25f-p2r.txt ${DATA}/sx120f-p2p.txt ${DATA}/sx120f-p2r.txt ${DATA}/sx50f-p2p.txt ${DATA}/sx50f-p2r.txt
)
list(APPEND tools_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cv-porttypes.ipp)


# Blocks muxes header

add_custom_command(
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cv-bmuxtypes.ipp
   COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/mux_to_source.py enum ${DATA} > ${CMAKE_CURRENT_BINARY_DIR}/cv-bmuxtypes.ipp
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/mux_to_source.py ${DATA}/cbuf-mux.txt ${DATA}/cmuxc-mux.txt ${DATA}/cmuxh-mux.txt ${DATA}/cmuxv-mux.txt ${DATA}/dll-mux.txt ${DATA}/dqs16-mux.txt ${DATA}/dsp-mux.txt ${DATA}/fpll-mux.txt ${DATA}/gpio-mux.txt ${DATA}/hip-mux.txt ${DATA}/hmc-mux.txt ${DATA}/hps_clocks-mux.txt ${DATA}/hssi-mux.txt ${DATA}/lab-mux.txt ${DATA}/lvl-mux.txt ${DATA}/m10k-mux.txt ${DATA}/mlab-mux.txt ${DATA}/opt-mux.txt ${DATA}/pma3-c-mux.txt ${DATA}/pma3-mux.txt ${DATA}/serpar-mux.txt ${DATA}/term-mux.txt
)
list(APPEND tools_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cv-bmuxtypes.ipp)

add_custom_target(tools DEPENDS ${tools_DEPENDS})

if(NOT MISTRAL_DONT_INSTALL)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cv-bmuxtypes.ipp DESTINATION include)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cv-drivertypes.ipp DESTINATION include)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cv-shapetypes.ipp DESTINATION include)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cv-porttypes.ipp DESTINATION include)

endif()
