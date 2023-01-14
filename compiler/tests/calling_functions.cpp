#include <driver.h>

#include <iostream>
#include <sstream>

#include "gtest/gtest.h"

class ParseFunctionsCalling : public ::testing::Test
{
};

TEST_F(ParseFunctionsCalling, ParseProcedureSyntax)
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

    const std::string expected =
        "jump.i #L0\n"
        "f:\n"
        "enter.i #0\n"
        "leave\n"
        "return\n"
        "L0:\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseFunctionsCalling, ParseFunctionSyntax)
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

    const std::string expected =
        "jump.i #L0\n"
        "f:\n"
        "enter.i #0\n"
        "leave\n"
        "return\n"
        "L0:\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseFunctionsCalling, GeneratesValidFunctionCall)
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
        "jump.i #L0\n"
        "f:\n"
        "enter.i #0\n"
        "leave\n"
        "return\n"
        "L0:\n"
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

TEST_F(ParseFunctionsCalling, GeneratesValidFunctionCallWithConversions)
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
        "jump.i #L0\n"
        "f:\n"
        "enter.i #0\n"
        "leave\n"
        "return\n"
        "L0:\n"
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

TEST_F(ParseFunctionsCalling, GeneratesValidFunctionCallWithConversionsOfConstants)
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
        "jump.i #L0\n"
        "f:\n"
        "enter.i #0\n"
        "leave\n"
        "return\n"
        "L0:\n"
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

TEST_F(ParseFunctionsCalling, CallingFunctionWithInvalidNumberOfArgumentsShouldFail)
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

TEST_F(ParseFunctionsCalling, DifferentTypesOfArrayItemArgumentShouldFail)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: array[1..10] of integer;\n"
        "function f(a: array[1..10] of real): real;\n"
        "begin\n"
        "end;\n"
        "\n"
        "begin\n"
        "x:=f(x)"
        "end.\n");

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_THROW(driver.parse(), std::runtime_error);
}

TEST_F(ParseFunctionsCalling, DifferentLengthOfArrayItemArgumentsShouldFail)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: array[1..10] of integer;\n"
        "function f(a: array[1..4] of integer): real;\n"
        "begin\n"
        "end;\n"
        "\n"
        "begin\n"
        "x:=f(x)"
        "end.\n");

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_THROW(driver.parse(), std::runtime_error);
}

TEST_F(ParseFunctionsCalling, ArrayArgumentShouldPass)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: array[1..10] of integer;\n"
        "var y: real;\n"
        "function f(a: array[1..10] of integer): real;\n"
        "begin\n"
        "end;\n"
        "\n"
        "begin\n"
        "y:=f(x)"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "f:\n"
        "enter.i #0\n"
        "leave\n"
        "return\n"
        "L0:\n"
        "push.i #0\n"
        "push.i #48\n"
        "call.i #f\n"
        "incsp.i #8\n"
        "mov.r 48,40\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseFunctionsCalling, UseResultOfFunction)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;\n"
        "function f(a: integer; b: integer): integer;\n"
        "begin\n"
        "end;\n"
        "\n"
        "begin\n"
        "x:=1 + f(x, x)*2"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "f:\n"
        "enter.i #0\n"
        "leave\n"
        "return\n"
        "L0:\n"
        "push.i #0\n"
        "push.i #0\n"
        "push.i #4\n"
        "call.i #f\n"
        "incsp.i #12\n"
        "mul.i 4,#2,8\n"
        "add.i #1,8,12\n"
        "mov.i 12,0\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}
