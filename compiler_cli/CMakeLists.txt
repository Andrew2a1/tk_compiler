project(compiler_cli C CXX)

set(SOURCES
    src/main.cpp
)

set(HEADERS
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE compiler)
set_common_properties(${PROJECT_NAME})
