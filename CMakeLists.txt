project(tk_compiler)
cmake_minimum_required(VERSION 3.20)

include(CTest)
enable_testing()

include(cmake/conan.cmake)
include(cmake/common.cmake)
include(cmake/flex_bison.cmake)

setup_conan()

add_subdirectory(compiler)
add_subdirectory(compiler_cli)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
