#include <driver.h>

#include <iostream>
#include <sstream>

#include "gtest/gtest.h"

class ParserGenerateOutputTest : public ::testing::Test
{
};

TEST_F(ParserGenerateOutputTest, GeneratesExitForEmptyProgram)
{
    std::istringstream input(
        "program example(input);\n"
        "begin\n"
        "end.\n");

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), "exit\n");
}

TEST_F(ParserGenerateOutputTest, DoesNotGenerateInstructionsOnlyForVarDeclaration)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;\n"
        "var g,h,j:integer;\n"
        "begin\n"
        "end.");

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), "exit\n");
}

TEST_F(ParserGenerateOutputTest, GeneratesMovOnVariableAssignment)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var a: integer;\n"
        "var g,h,j:integer;\n"
        "begin\n"
        "a := 3\n"
        "end.");

    const std::string expected_data =
        "mov.i #3,0\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected_data);
}

TEST_F(ParserGenerateOutputTest, GeneratesValidOutputForManyOperations)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x, y: integer;\n"
        "var g,h:integer;\n"
        "\n"
        "begin\n"
        "read(x,y);\n"
        "h:=1;\n"
        "g:=x+y*h mod 2;\n"
        "h:=g and 5 or 2 div 1;\n"
        "write(g, h);\n"
        "write(x)\n"
        "end.\n");

    std::string expected_data =
        "read.i 0\n"
        "read.i 4\n"
        "mov.i #1,12\n"
        "mul.i 4,12,16\n"
        "mod.i 16,#2,20\n"
        "add.i 0,20,24\n"
        "mov.i 24,8\n"
        "and.i 8,#5,28\n"
        "div.i #2,#1,32\n"
        "or.i 28,32,36\n"
        "mov.i 36,12\n"
        "write.i 8\n"
        "write.i 12\n"
        "write.i 0\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected_data);
}