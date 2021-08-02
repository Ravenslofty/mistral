add_executable(mistral-cv main.cc)

target_link_libraries(mistral-cv PUBLIC mistral)
target_link_libraries(mistral-cv PUBLIC lzma)
target_include_directories(mistral-cv PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/libmistral" "${CMAKE_BINARY_DIR}/libmistral")

install(TARGETS mistral-cv DESTINATION bin)

