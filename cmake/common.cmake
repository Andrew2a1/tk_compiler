function(set_common_properties TARGET)
    set_target_properties(${TARGET} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED TRUE
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    if (MSVC)
        target_compile_options(${APP_TARGET_NAME} PRIVATE /W4 $<IF:$<CONFIG:Debug>,/Zi,/O2>)
    else()
        target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Wpedantic $<$<CONFIG:RELEASE>:-O3> $<$<CONFIG:DEBUG>:-O0 -g>)
    endif()

    target_link_options(${TARGET} PRIVATE $<$<CONFIG:RELEASE>:-s>)
    install(TARGETS ${TARGET})
endfunction()

function(setup_tests TARGET)
    set(TEST_TARGET ${TARGET}_tests)
    find_package(GTest REQUIRED CONFIG)

    add_executable(${TEST_TARGET} ${ARGN})
    target_link_libraries(${TEST_TARGET} PRIVATE GTest::gtest GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(${TEST_TARGET})
    set_common_properties(${TEST_TARGET})
endfunction()
