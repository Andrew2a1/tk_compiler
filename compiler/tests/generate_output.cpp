#include <driver.h>

#include <filesystem>
#include <fstream>
#include <sstream>

#include "gtest/gtest.h"

class ParserGenerateOutputTest : public ::testing::Test
{
};

TEST_F(ParserGenerateOutputTest, GeneratesExitForEmptyProgram)
{
    Driver driver;

    const auto input_filepath = std::filesystem::path{TEST_DATA_DIRECOTRY} / "simplest.txt";
    const auto output_path = std::filesystem::temp_directory_path() / "output.s";

    const int result = driver.parse(input_filepath, output_path);
    ASSERT_EQ(result, 0);

    std::ifstream generated_code(output_path, std::ios::in);
    std::string output_data;

    std::getline(generated_code, output_data);
    ASSERT_EQ(output_data, "exit");
}

TEST_F(ParserGenerateOutputTest, DoesNotGenerateInstructionsOnlyForVarDeclaration)
{
    Driver driver;

    const auto input_filepath = std::filesystem::path{TEST_DATA_DIRECOTRY} / "four_var.txt";
    const auto output_path = std::filesystem::temp_directory_path() / "output.s";

    const int result = driver.parse(input_filepath, output_path);
    ASSERT_EQ(result, 0);

    std::ifstream generated_code(output_path, std::ios::in);
    std::string output_data;

    std::getline(generated_code, output_data);
    ASSERT_EQ(output_data, "exit");
}

TEST_F(ParserGenerateOutputTest, GeneratesMovOnVariableAssignment)
{
    Driver driver;

    const auto input_filepath = std::filesystem::path{TEST_DATA_DIRECOTRY} / "one_var_const.txt";
    const auto output_path = std::filesystem::temp_directory_path() / "output.s";

    const int result = driver.parse(input_filepath, output_path);
    ASSERT_EQ(result, 0);

    std::ifstream generated_code(output_path, std::ios::in);
    std::string output_data;

    std::getline(generated_code, output_data);
    ASSERT_EQ(output_data, "mov.i #3,0");

    std::getline(generated_code, output_data);
    ASSERT_EQ(output_data, "exit");
}

TEST_F(ParserGenerateOutputTest, GeneratesValidOutputForManyOperations)
{
    const std::string expected =
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

    Driver driver;

    const auto input_filepath = std::filesystem::path{TEST_DATA_DIRECOTRY} / "all_simple.txt";
    const auto output_path = std::filesystem::temp_directory_path() / "output.s";
    const int result = driver.parse(input_filepath, output_path);

    ASSERT_EQ(result, 0);

    std::ifstream generated_code(output_path, std::ios::in);
    std::stringstream buffer;
    buffer << generated_code.rdbuf();

    ASSERT_EQ(buffer.str(), expected);
}