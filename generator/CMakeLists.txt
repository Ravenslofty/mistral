project(generator)

set(generator_SOURCES
    prefixtree.cc
    io.cc
    nodes.cc
    routes.cc
    rdriver.cc
    lines.cc
    drivers.cc
    p2r.cc
    generator.cc
    ../libmistral/bdz-ph.cc
)

add_executable(generator
    ${generator_SOURCES}
)

add_dependencies(generator tools)

target_include_directories(generator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../libmistral ${CMAKE_CURRENT_BINARY_DIR}/../tools)

target_link_libraries(generator PUBLIC lzma)
