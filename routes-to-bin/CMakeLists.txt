project(routes-to-bin)

set(routes-to-bin_SOURCES
    prefixtree.cc
    io.cc
    rnode.cc
    routes.cc
    lines.cc
    routes-to-bin.cc
    ../libmistral/bdz-ph.cc
)

add_executable(routes-to-bin
    ${routes-to-bin_SOURCES}
)

target_include_directories(routes-to-bin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../libmistral)

target_link_libraries(routes-to-bin PUBLIC lzma)
