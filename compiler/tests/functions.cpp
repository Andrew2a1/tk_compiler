#include <driver.h>

#include <iostream>
#include <sstream>

#include "gtest/gtest.h"

class ParseFunctions : public ::testing::Test
{
};

TEST_F(ParseFunctions, ReturnValueShouldGenerateMovInstruction)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x, y: integer;\n"
        "var g,h:real;\n"
        "\n"
        "function a(b: integer): integer;\n"
        "begin\n"
        "a := b\n"
        "end; \n"
        "\n"
        "begin\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "a:\n"
        "enter.i #0\n"
        "mov.i *BP+12,*BP+8\n"
        "leave\n"
        "return\n"
        "L0:\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseFunctions, PassingArrayAsArgumentShouldGenerateValidCode)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: array[1..10] of integer;\n"
        "\n"
        "function f(a: array[1..10] of integer): integer;\n"
        "begin\n"
        "f := a[1]\n"
        "end; \n"
        "\n"
        "begin\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "f:\n"
        "enter.i #8\n"
        "sub.i #1,#1,BP-4\n"
        "mul.i BP-4,#4,BP-4\n"
        "add.i BP+12,BP-4,BP-8\n"
        "mov.i *BP-8,*BP+8\n"
        "leave\n"
        "return\n"
        "L0:\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseFunctions, ProcedureShouldCanModifyArrayItems)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: array[1..10] of integer;\n"
        "\n"
        "procedure f(a: array[1..10] of integer);\n"
        "begin\n"
        "a[1] := 2\n"
        "end; \n"
        "\n"
        "begin\n"
        "f(x);\n"
        "write(x[1])\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "f:\n"
        "enter.i #8\n"
        "sub.i #1,#1,BP-4\n"
        "mul.i BP-4,#4,BP-4\n"
        "add.i BP+8,BP-4,BP-8\n"
        "mov.i #2,*BP-8\n"
        "leave\n"
        "return\n"
        "L0:\n"
        "push.i #0\n"
        "call.i #f\n"
        "incsp.i #4\n"
        "sub.i #1,#1,40\n"
        "mul.i 40,#4,40\n"
        "add.i #0,40,44\n"
        "write.i *44\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseFunctions, GeneratesValidIfStatementsInsideProcedure)
{
    std::istringstream input(
        "program example(input, output);\n"
        "procedure f;\n"
        "begin\n"
        "if 1 then write(1) else write(0)\n"
        "end; \n"
        "\n"
        "begin\n"
        "f\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "f:\n"
        "enter.i #0\n"
        "je.i #1,#0,#L1\n"
        "write.i #1\n"
        "jump.i #L2\n"
        "L1:\n"
        "write.i #0\n"
        "L2:\n"
        "leave\n"
        "return\n"
        "L0:\n"
        "call.i #f\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseFunctions, CanAccessGlobalVariableFromFunction)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;\n"
        "procedure f;\n"
        "begin\n"
        "write(x)\n"
        "end; \n"
        "\n"
        "begin\n"
        "f\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "f:\n"
        "enter.i #0\n"
        "write.i 0\n"
        "leave\n"
        "return\n"
        "L0:\n"
        "call.i #f\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}
