#include <driver.h>

#include <iostream>
#include <sstream>

#include "gtest/gtest.h"

class ParseFunctions : public ::testing::Test
{
};

TEST_F(ParseFunctions, ParseProcedureSyntax)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x, y: integer;\n"
        "var g,h:real;\n"
        "\n"
        "procedure f;\n"
        "begin\n"
        "end;\n"
        "\n"
        "begin\n"
        "end.\n");

    const std::string expected = "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseFunctions, ParseFunctionSyntax)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x, y: integer;\n"
        "var g,h:real;\n"
        "\n"
        "function f(a: real): integer;\n"
        "begin\n"
        "end;\n"
        "\n"
        "begin\n"
        "end.\n");

    const std::string expected = "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}
