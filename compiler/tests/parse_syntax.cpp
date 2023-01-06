#include <driver.h>

#include <iostream>
#include <sstream>

#include "gtest/gtest.h"

class ParserSyntaxTest : public ::testing::Test
{
};

TEST_F(ParserSyntaxTest, ParsesEmptyButValidProgram)
{
    std::istringstream input(
        "program example(input);\n"
        "begin\n"
        "end.");

    Driver driver(std::cout, input);
    ASSERT_EQ(driver.parse(), 0);
}

TEST_F(ParserSyntaxTest, NoInputGeneratesError)
{
    std::istringstream input("");
    Driver driver(std::cout, input);
    ASSERT_NE(driver.parse(), 0);
}

TEST_F(ParserSyntaxTest, ParsesCodeOneVariable)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var a: integer;\n"
        "begin\n"
        "end.");

    Driver driver(std::cout, input);
    ASSERT_EQ(driver.parse(), 0);
}

TEST_F(ParserSyntaxTest, ParsesCodeFromWeb)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x, y: integer;\n"
        "var g,h:integer;\n"
        "\n"
        "begin\n"
        "read(x,y);\n"
        "h:=1;\n"
        "g:=x+y*h;\n"
        "write(g)\n"
        "end.\n");

    Driver driver(std::cout, input);
    ASSERT_EQ(driver.parse(), 0);
}

TEST_F(ParserSyntaxTest, FailsOnCodeFromWebWithMissingSemicolon)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x, y: integer;\n"
        "var g,h:integer;\n"
        "\n"
        "begin\n"
        "read(x,y)\n"
        "h:=1;\n"
        "g:=x+y*h;\n"
        "write(g)\n"
        "end.\n");

    Driver driver(std::cout, input);
    ASSERT_NE(driver.parse(), 0);
}
