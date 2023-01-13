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

TEST_F(ParseFunctions, GeneratesValidFunctionCall)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;\n"
        "function f(a: integer): integer;\n"
        "begin\n"
        "end;\n"
        "\n"
        "begin\n"
        "x:=f(1)"
        "end.\n");

    const std::string expected =
        "mov.i #1,4\n"
        "push.i #4\n"
        "push.i #8\n"
        "call.i #f\n"
        "incsp.i #8\n"
        "mov.i 8,0\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseFunctions, GeneratesValidFunctionCallWithConversions)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;\n"
        "function f(a: real): real;\n"
        "begin\n"
        "end;\n"
        "\n"
        "begin\n"
        "x:=f(x)"
        "end.\n");

    const std::string expected =
        "inttoreal.i 0,4\n"
        "push.i #4\n"
        "push.i #12\n"
        "call.i #f\n"
        "incsp.i #8\n"
        "realtoint.r 12,20\n"
        "mov.i 20,0\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseFunctions, GeneratesValidFunctionCallWithConversionsOfConstants)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;\n"
        "function f(a: real; b: integer): real;\n"
        "begin\n"
        "end;\n"
        "\n"
        "begin\n"
        "x:=f(1, 2.0)"
        "end.\n");

    const std::string expected =
        "mov.r #1,4\n"
        "push.i #4\n"
        "mov.i #2,12\n"
        "push.i #12\n"
        "push.i #16\n"
        "call.i #f\n"
        "incsp.i #12\n"
        "realtoint.r 16,24\n"
        "mov.i 24,0\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseFunctions, CallingFunctionWithInvalidNumberOfArgumentsShouldFail)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;\n"
        "function f(a: real; b: integer): real;\n"
        "begin\n"
        "end;\n"
        "\n"
        "begin\n"
        "x:=f(1, 2.0, 3)"
        "end.\n");

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_THROW(driver.parse(), std::runtime_error);
}

// TEST_F(ParseFunctions, GeneratesValidProcedureCall)
// {
//     std::istringstream input(
//         "program example(input, output);\n"
//         "var x: integer;\n"
//         "procedure f;\n"
//         "begin\n"
//         "end;\n"
//         "\n"
//         "begin\n"
//         "f\n"
//         "end.\n");

//     const std::string expected =
//         "call.i #f\n"
//         "exit\n";

//     std::ostringstream output;
//     Driver driver(output, input);

//     ASSERT_EQ(driver.parse(), 0);
//     ASSERT_EQ(output.str(), expected);
// }