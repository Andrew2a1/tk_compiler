#include <cli_utils.h>
#include <driver.h>

#include <CLI/CLI.hpp>
#include <clocale>
#include <filesystem>
#include <fstream>

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

    std::ifstream input_file(input_filename);
    std::ofstream output_file(output_filename);

    std::istream &in = use_console(input_filename) ? std::cin : input_file;
    std::ostream &out = use_console(output_filename) ? std::cout : output_file;

    if (in.fail())
    {
        std::cerr << "Cannot open file " << input_filename << " for reading" << std::endl;
        return -1;
    }

    if (out.fail())
    {
        std::cerr << "Cannot open file " << input_filename << " for writing" << std::endl;
        return -1;
    }

    Driver driver(out, in);
    driver.set_location_filename(input_filename);
    driver.set_debug(trace_scanning, trace_parsing);
    const int result = driver.parse();

    if (result != 0)
    {
        std::cerr << "Compilation failed." << std::endl;
    }
    else
    {
        print_symbol_table(driver.symbol_table);
    }

    std::locale::global(std::locale("C"));
    return result;
}
