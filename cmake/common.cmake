function(set_common_properties TARGET)
    set_target_properties(${TARGET} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED TRUE
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    target_include_directories(${TARGET} PUBLIC include)

    if (MSVC)
        target_compile_options(${TARGET} PRIVATE /W4 $<IF:$<CONFIG:Debug>,/Zi,/O2>)
    else()
        target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Wpedantic $<$<CONFIG:RELEASE>:-O3> $<$<CONFIG:DEBUG>:-O0 -g>)
        target_link_options(${TARGET} PRIVATE $<$<CONFIG:RELEASE>:-s>)
    endif()

    install(TARGETS ${TARGET})
endfunction()

function(setup_tests TARGET)
    set(TEST_TARGET ${TARGET}_tests)
    find_package(GTest REQUIRED CONFIG)

    add_executable(${TEST_TARGET} ${ARGN})
    target_link_libraries(${TEST_TARGET} PRIVATE ${TARGET} GTest::gtest GTest::gtest_main)
    target_compile_definitions(${TEST_TARGET} PRIVATE TEST_DATA_DIRECOTRY="${CMAKE_CURRENT_SOURCE_DIR}/tests/test_data")
    set_common_properties(${TEST_TARGET})

    include(GoogleTest)
    gtest_discover_tests(${TEST_TARGET})
endfunction()
