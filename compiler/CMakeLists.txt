project(compiler C CXX)

set(SOURCES
    src/symbol.c
    src/error.c
)

set(HEADERS
    include/symbol.h
    include/global.h
)

set(TESTS
    tests/test.cpp
)

set(FLEX_BISON_FILES
    gen/lexer.l
    gen/parser.y
)

setup_flex_bison(
    ${FLEX_BISON_FILES}
    lexer.cpp
    parser.cpp
    parser.h
)

add_library(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${BISON_${PARSER_TARGET}_OUTPUTS}
    ${FLEX_${SCANNER_TARGET}_OUTPUTS}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${GENERATED_SOURCES_DIR})

set_common_properties(${PROJECT_NAME})
setup_tests(${PROJECT_NAME} ${TESTS})
