project(compiler C CXX)

set(SOURCES
    src/driver.cpp
    src/symbol_table.cpp
)

set(HEADERS
    include/driver.h
    include/symbol_table.h
)

set(TESTS
    tests/parse_syntax.cpp
    tests/parse_declarations.cpp
    tests/parse_expressions.cpp
    tests/generate_output.cpp
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
