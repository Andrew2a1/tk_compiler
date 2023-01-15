project(compiler C CXX)

set(SOURCES
    src/driver.cpp
    src/symbol_table.cpp
    src/symbol_table_entry.cpp
    src/variable_type.cpp
)

set(HEADERS
    include/driver.h
    include/scanner.h
    include/symbol_table.h
    include/symbol_table_entry.h
    include/symbol_type.h
    include/variable_type.h
)

set(TESTS
    tests/parse_syntax.cpp
    tests/parse_declarations.cpp
    tests/parse_expressions.cpp
    tests/generate_output.cpp
    tests/real_numbers.cpp
    tests/boolean_operations.cpp
    tests/relation_operations.cpp
    tests/conditional_expressions.cpp
    tests/while_loop.cpp
    tests/arrays.cpp
    tests/calling_functions.cpp
    tests/procedures.cpp
    tests/functions.cpp
    tests/bubblesort.cpp
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
