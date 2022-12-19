project(compiler_cli C CXX)

set(SOURCES
    src/main.cpp
)

set(HEADERS
)

find_package(CLI11 CONFIG REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE compiler CLI11::CLI11)
set_common_properties(${PROJECT_NAME})
