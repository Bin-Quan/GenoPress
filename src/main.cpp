
#include "compressor.h"
#include "decompressor.h"
#include "argumentParser.h"

#include <iostream>

#include <chrono>
#include <time.h>
using namespace std::chrono;

int params_options(int argc, const char *argv[], argumentParser &parser);
int compress_entry(argumentParser &parser);
int decompress_entry(argumentParser &parser);

int main(int argc, const char *argv[])
{

    auto start = high_resolution_clock::now();

    int result = 0;
    argumentParser parser;

    std::cout << parser.params.compression_level << std::endl;

    if (!params_options(argc, argv, parser))
        return 1;

    if (parser.params.task_mode == task_mode_t::mcompress)
    {
        result = compress_entry(parser);
        if (result)
            std::cerr << "Compression error!!!\n";
    }
    else if (parser.params.task_mode == task_mode_t::mdecompress)
    {
        result = decompress_entry(parser);
        if (result)
            std::cerr << " Decompression error!!!\n";
    }

    auto end = high_resolution_clock::now();
    double time_duration = duration<double>(end - start).count();

    std::cerr << "Total processing time: " << time_duration << " seconds.\n";

    return result;
}

int params_options(int argc, const char *argv[], argumentParser &parser)
{

    if (argc < 2)
    {
        return parser.usage();
    }
    if (!parser.parse_mode_option(argv[1]))
    {
        return parser.usage();
    }

    bool parse_success = false;
    if (parser.params.task_mode == task_mode_t::mcompress)
    {
        parse_success = parser.parse_compress_options(argc, argv);
    }
    else if (parser.params.task_mode == task_mode_t::mdecompress)
    {
        parse_success = parser.parse_decompress_options(argc, argv);
    }

    return parse_success && parser.validate_inputs() ? 1 : parser.usage();
}

int compress_entry(argumentParser &parser)
{

    Compressor compressor(parser.params); // Passing compression parameters.
    if (!compressor.CompressProcess())
        return 1;

    return 0;
}

int decompress_entry(argumentParser &parser)
{

    // bool result = true;
    if (parser.params.out_type == file_type::BCF_File && parser.params.out_file_name == "")

        return parser.usage_decompress();

    Decompressor decompressor(parser.params); // Load settings and data

    // decompressor.getChrom();              //Obtaining chromosome information.

    if (!decompressor.DecompressProcess())
        return 1;

    return 0;
}