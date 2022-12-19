#include <driver.h>

#include <CLI/CLI.hpp>
#include <filesystem>

int main(int argc, char *argv[])
{
    std::filesystem::path input_filename, output_filename;
    bool trace_scanning = false;
    bool trace_parsing = false;

    CLI::App app("Compiler project for subset of Pascal language", argv[0]);
    app.add_option("input_filename", input_filename, "Input filename with code to compile (empty or '-' for stdin)");
    app.add_option("output_filename", output_filename, "Output filename with generated assembly code (empty or '-' for stdout)");
    app.add_flag("-s,--trace_scanning", trace_scanning, "Show debug output from scanner");
    app.add_flag("-p,--trace_parsing", trace_parsing, "Show debug output from parser");

    try
    {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError &e)
    {
        return app.exit(e);
    }

    Driver driver(trace_scanning, trace_parsing);
    int result = driver.parse(input_filename, output_filename);

    if (result != 0)
    {
        std::cerr << "Compilation failed." << std::endl;
    }

    return result;
}