#include <driver.h>

#include <iostream>
#include <sstream>

#include "gtest/gtest.h"

class ParseProcedures : public ::testing::Test
{
};

TEST_F(ParseProcedures, GeneratesValidProcedureCall)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;\n"
        "procedure f;\n"
        "begin\n"
        "end;\n"
        "\n"
        "begin\n"
        "f\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "f:\n"
        "enter.i #0\n"
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

TEST_F(ParseProcedures, GeneratesValidProcedureCallWithArguments)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;\n"
        "procedure f(a: integer);\n"
        "begin\n"
        "end;\n"
        "\n"
        "begin\n"
        "f(1)\n"
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
        "call.i #f\n"
        "incsp.i #4\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseProcedures, ValueOfProcedureCannotBeUsedInExpression)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;\n"
        "procedure f(a: integer);\n"
        "begin\n"
        "end;\n"
        "\n"
        "begin\n"
        "x:=1 + f(1)\n"
        "end.\n");

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_THROW(driver.parse(), std::runtime_error);
}

TEST_F(ParseProcedures, ProcedureCanBeUsedInIfElseStatement)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;\n"
        "procedure f(a: integer);\n"
        "begin\n"
        "end;\n"
        "\n"
        "begin\n"
        "if 1 then f(1) else write(1)\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "f:\n"
        "enter.i #0\n"
        "leave\n"
        "return\n"
        "L0:\n"
        "je.i #1,#0,#L1\n"
        "mov.i #1,4\n"
        "push.i #4\n"
        "call.i #f\n"
        "incsp.i #4\n"
        "jump.i #L2\n"
        "L1:\n"
        "write.i #1\n"
        "L2:\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseProcedures, ReturningValueFromProcedureShouldFail)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;\n"
        "procedure f(a: integer);\n"
        "begin\n"
        "f:=1"
        "end;\n"
        "\n"
        "begin\n"
        "f(1)\n"
        "end.\n");

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_THROW(driver.parse(), std::runtime_error);
}

TEST_F(ParseProcedures, ProcedureCanCallOtherProceduresAndFunctions)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;"
        "var y: array [0..10] of integer;\n"
        "\n"
        "function f(x: integer): integer;\n"
        "begin\n"
        "f := 2*x\n"
        "end;\n"
        "\n"
        "procedure w(x: integer);\n"
        "begin\n"
        "if x>0 then write(x) else write(0)\n"
        "end;\n"
        "\n"
        "procedure p(x: integer; y: array [0..10] of integer);\n"
        "begin\n"
        "while x > 0 do\n"
        "begin\n"
        "x := x - 1;\n"
        "w(f(y[x]))\n"
        "end\n"
        "end;\n"
        "\n"
        "begin\n"
        "read(x, y[0], y[1], y[2]);\n"
        "p(x, y)\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "f:\n"
        "enter.i #4\n"
        "mul.i #2,*BP+12,BP-4\n"
        "mov.i BP-4,*BP+8\n"
        "leave\n"
        "return\n"
        "w:\n"
        "enter.i #4\n"
        "jg.i *BP+8,#0,#L1\n"
        "mov.i #0,BP-4\n"
        "jump.i #L2\n"
        "L1:\n"
        "mov.i #1,BP-4\n"
        "L2:\n"
        "je.i BP-4,#0,#L3\n"
        "write.i *BP+8\n"
        "jump.i #L4\n"
        "L3:\n"
        "write.i #0\n"
        "L4:\n"
        "leave\n"
        "return\n"
        "p:\n"
        "enter.i #20\n"
        "L5:\n"
        "jg.i *BP+12,#0,#L7\n"
        "mov.i #0,BP-4\n"
        "jump.i #L8\n"
        "L7:\n"
        "mov.i #1,BP-4\n"
        "L8:\n"
        "je.i BP-4,#0,#L6\n"
        "sub.i *BP+12,#1,BP-8\n"
        "mov.i BP-8,*BP+12\n"
        "sub.i *BP+12,#0,BP-12\n"
        "mul.i BP-12,#4,BP-12\n"
        "add.i BP+8,BP-12,BP-16\n"
        "push.i BP-16\n"
        "push.i #BP-20\n"
        "call.i #f\n"
        "incsp.i #8\n"
        "push.i #BP-20\n"
        "call.i #w\n"
        "incsp.i #4\n"
        "jump.i #L5\n"
        "L6:\n"
        "leave\n"
        "return\n"
        "L0:\n"
        "sub.i #0,#0,48\n"
        "mul.i 48,#4,48\n"
        "add.i #4,48,52\n"
        "sub.i #1,#0,56\n"
        "mul.i 56,#4,56\n"
        "add.i #4,56,60\n"
        "sub.i #2,#0,64\n"
        "mul.i 64,#4,64\n"
        "add.i #4,64,68\n"
        "read.i 0\n"
        "read.i *52\n"
        "read.i *60\n"
        "read.i *68\n"
        "push.i #0\n"
        "push.i #4\n"
        "call.i #p\n"
        "incsp.i #8\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}