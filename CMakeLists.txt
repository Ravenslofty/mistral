cmake_minimum_required(VERSION 3.10)

project(Mistral VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-Wall)

add_subdirectory(tools)
add_subdirectory(generator)
add_subdirectory(libmistral)
add_subdirectory(mistral-cv)
