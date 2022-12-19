#include <driver.h>

#include <filesystem>

#include "gtest/gtest.h"

class ParserSyntaxTest : public ::testing::Test
{
};

TEST_F(ParserSyntaxTest, ParsesEmptyButValidProgram)
{
    const auto input_filepath = std::filesystem::path{TEST_DATA_DIRECOTRY} / "simplest.txt";
    Driver driver;

    const int result = driver.parse(input_filepath);
    ASSERT_EQ(result, 0);
}

TEST_F(ParserSyntaxTest, NoInputGeneratesError)
{
    const auto input_filepath = std::filesystem::path{TEST_DATA_DIRECOTRY} / "empty.txt";
    Driver driver;

    const int result = driver.parse(input_filepath);
    ASSERT_NE(result, 0);
}

TEST_F(ParserSyntaxTest, ParsesCodeOneVariable)
{
    const auto input_filepath = std::filesystem::path{TEST_DATA_DIRECOTRY} / "one_var.txt";
    Driver driver;

    const int result = driver.parse(input_filepath);
    ASSERT_EQ(result, 0);
}

TEST_F(ParserSyntaxTest, ParsesCodeFromWeb)
{
    const auto input_filepath = std::filesystem::path{TEST_DATA_DIRECOTRY} / "simple_from_web.txt";
    Driver driver;

    const int result = driver.parse(input_filepath);
    ASSERT_EQ(result, 0);
}

TEST_F(ParserSyntaxTest, FailsOnCodeFromWebWithMissingSemicolon)
{
    const auto input_filepath = std::filesystem::path{TEST_DATA_DIRECOTRY} / "simple_from_web_with_error.txt";
    Driver driver;

    const int result = driver.parse(input_filepath);
    ASSERT_NE(result, 0);
}