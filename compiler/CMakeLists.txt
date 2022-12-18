project(compiler C CXX)

set(SOURCES
    src/test.cpp
    src/lexer.cpp
    src/parser.cpp
)

set(HEADERS
    include/parser.h
)

set(TESTS
    tests/test.cpp
)

setup_flex_bison(
    gen/lexer.l
    gen/parser.y
    src/lexer.cpp
    src/parser.cpp
    include/parser.h
)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC include)

set_common_properties(${PROJECT_NAME})
setup_tests(${PROJECT_NAME} ${TESTS})
