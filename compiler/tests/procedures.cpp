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

TEST_F(ParseProcedures, ProcedureCanBeUsedInIf)
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
