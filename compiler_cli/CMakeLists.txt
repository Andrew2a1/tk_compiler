project(compiler_cli C CXX)

set(SOURCES
    src/main.cpp
    src/cli_utils.cpp
)

set(HEADERS
    include/cli_utils.h
)

find_package(CLI11 CONFIG REQUIRED)
find_package(tabulate CONFIG REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE compiler CLI11::CLI11 tabulate::tabulate)
set_common_properties(${PROJECT_NAME})
