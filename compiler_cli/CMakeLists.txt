project(compiler_cli C CXX)

set(SOURCES
    src/main.cpp
)

set(HEADERS
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE compiler)
set_common_properties(${PROJECT_NAME})
