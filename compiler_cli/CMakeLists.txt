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

# Acceptance tests

#file(GLOB TEST_FILES "tests/*.pas")
foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(PURE_NAME ${TEST_FILE} NAME_WE)
    get_filename_component(TEST_DIR ${TEST_FILE} DIRECTORY)

    set(OUT_DIR ${TEST_DIR}/out)
    make_directory(${OUT_DIR})

    add_test(NAME ${PROJECT_NAME}_tests_${PURE_NAME}
        COMMAND ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME} ${TEST_FILE} ${OUT_DIR}/${PURE_NAME}.s
        )
endforeach()



