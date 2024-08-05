
#include "argumentParser.h"
#include <iostream>

argumentParser::argumentParser(){
    params.task_mode = task_mode_t::none;
    params.in_type = file_type::VCF_File;
    params.out_type = file_type::VCF_File;
    params.compress_mode = compress_mode_t::lossless_mode;
    params.max_replication_depth = 100;
    params.ploidy = 2;
    params.no_threads = 5;
    params.no_gt_threads = 1;
    params.var_in_block = 0;
    params.vec_len = 0;
    params.n_samples = 0;
    params.no_blocks = 0;
    params.in_file_name = "-";
    params.out_file_name = "-";
    params.merge_file_flag = false;
    params.out_samples_file_name = "";
    params.range = "";
    params.samples = "";
    params.MB_memory = true;  // Remember some of the decoded vectors
    params.max_MB_memory = 0; // 0 means no limit (if MB_memory == true)
    params.compression_level = '1';
    params.out_samples_name = false;
    params.out_AC_AN = false;
    params.out_genotypes = true;
    // out_file_flag = false;
    // out_ohter_fields = false;
    params.split_flag = false;
    params.out_header_flag = true;
    params.records_to_process = UINT32_MAX;
    params.mode = '\0';
    params.out_id = "";
    params.min_qual = min_float;
    params.max_qual = max_float;
    params.minAC = 0;
    params.maxAC = INT32_MAX;
    params.minAF = 0;
    params.maxAF = 1;
}

int argumentParser::usage() const{
    
    std::cerr << "Usage: gsc [option] [arguments] " << std::endl;
    std::cerr << "Available options: " << std::endl;
    std::cerr << "\tcompress - compress VCF/BCF file" << std::endl;
    std::cerr << "\tdecompress     - query and decompress to VCF/BCF file " << std::endl;

    exit(0);
}

int argumentParser::usage_compress() const{
    
    std::cerr << "Usage of gsc compress:\n\n"
                << "\tgsc compress [options] [--in [in_file]] [--out [out_file]]\n\n"
                << "Where:\n\n"
                << "\t[options]              Optional flags and parameters for compression.\n"
                << "\t-i,  --in [in_file]    Specify the input file (default: VCF or VCF.GZ). If omitted, input is taken from standard input (stdin).\n"
                << "\t-o,  --out [out_file]  Specify the output file. If omitted, output is sent to standard output (stdout).\n\n"
                
                << "Options:\n\n"
                << "\t-M,  --mode_lossy     Choose lossy compression mode (lossless by default).\n"
                << "\t-b,  --bcf             Input is a BCF file (default: VCF or VCF.GZ).\n"
                << "\t-p,  --ploidy [X]      Set ploidy of samples in input VCF to [X] (default: 2).\n"
                << "\t-t,  --threads [X]     Set number of threads to [X] (default: 1).\n"
                << "\t-d,  --depth [X]       Set maximum replication depth to [X] (default: 100, 0 means no matches).\n"
                << "\t-m,  --merge [X]       Specify files to merge, separated by commas (e.g., -m chr1.vcf,chr2.vcf), or '@' followed by a file containing a list of VCF files (e.g., -m @file_with_IDs.txt). By default, all VCF files are compressed.\n"
                << std::endl;

    exit(0);
}

int argumentParser::usage_decompress() const{
    std::cerr << "Usage of gsc decompress and query:\n\n"
                << "\tgsc decompress [options] --in [in_file] --out [out_file]\n\n"

                << "Where:\n"
                << "\t[options]              Optional flags and parameters for compression.\n"
                << "\t-i,  --in [in_file]    Specify the input file . If omitted, input is taken from standard input (stdin).\n"
                << "\t-o,  --out [out_file]  Specify the output file (default: VCF). If omitted, output is sent to standard output (stdout).\n\n"

                << "Options:\n\n"
                << "    General Options:\n\n"
                << "\t-M,  --mode_lossy\tChoose lossy compression mode (default: lossless).\n"
                << "\t-b,  --bcf\t\tOutput a BCF file (default: VCF).\n\n"

                << "    Filter options (applicable in lossy compression mode only): \n\n"
                << "\t-r,  --range [X]\tSpecify range in format [start],[end] (e.g., -r 4999756,4999852).\n"
                << "\t-s,  --samples [X]\tSamples separated by comms (e.g., -s HG03861,NA18639) OR '@' sign followed by the name of a file with sample name(s) separated by whitespaces (for exaple: -s @file_with_IDs.txt). By default all samples/individuals are decompressed. \n"
                << "\t--header-only\t\tOutput only the header of the VCF/BCF.\n"
                << "\t--no-header\t\tOutput without the VCF/BCF header (only genotypes).\n"
                << "\t-G,  --no-genotype\tDon't output sample genotypes (only #CHROM, POS, ID, REF, ALT, QUAL, FILTER, and INFO columns).\n"
                << "\t-C,  --out-ac-an\tWrite AC/AN to the INFO field.\n"
                << "\t-S,  --split\t\tSplit output into multiple files (one per chromosome).\n"
                << "\t-I, [ID=^]\t\tInclude only sites with specified ID (e.g., -I \"ID=rs6040355\").\n"
                << "\t--minAC [X]\t\tInclude only sites with AC <= X.\n"
                << "\t--maxAC [X]\t\tInclude only sites with AC >= X.\n"
                << "\t--minAF [X]\t\tInclude only sites with AF >= X (X: 0 to 1).\n"
                << "\t--maxAF [X]\t\tInclude only sites with AF <= X (X: 0 to 1).\n"
                << "\t--min-qual [X]\t\tInclude only sites with QUAL >= X.\n"
                << "\t--max-qual [X]\t\tInclude only sites with QUAL <= X.\n"
                << std::endl;
    
    exit(0);
}

bool argumentParser::parse_common_options(int& i, int argc, const char* argv[]) {
    if (std::strcmp(argv[i], "--in") == 0 || std::strcmp(argv[i], "-i") == 0) {
        if (!isatty(STDIN_FILENO)) {
            std::cerr << "Error: Conflicting inputs - both filename and stdin data detected." << std::endl;
            return false;
        }
        i++;
        if (i >= argc) return false;
        params.in_file_name = argv[i];
    } else if (std::strcmp(argv[i], "--out") == 0 || std::strcmp(argv[i], "-o") == 0) {
        if (!isatty(STDOUT_FILENO)) return false;
        i++;
        if (i >= argc) return false;
        params.out_file_name = argv[i];
    } else {
        return false;
    }
    return true;
}

bool argumentParser::parse_compress_options(int argc, const char* argv[]) {
    for (int i = 2; i < argc; ++i) {
        if (argv[i][0] != '-') {
            return false;
        }
        if (std::strcmp(argv[i], "--mode_lossy") == 0 || std::strcmp(argv[i], "-M") == 0) {
            params.compress_mode = compress_mode_t::lossy_mode;
        } else if (!parse_common_options(i, argc, argv)) {
            return false;
        } else if (std::strcmp(argv[i], "--ploidy") == 0 || std::strcmp(argv[i], "-p") == 0) {
            i++;
            if (i >= argc) return false;
            params.ploidy = std::atoi(argv[i]);
        } else if (std::strcmp(argv[i], "--depth") == 0 || std::strcmp(argv[i], "-d") == 0) {
            i++;
            if (i >= argc) return false;
            params.max_replication_depth = std::atoi(argv[i]);
        } else if (std::strcmp(argv[i], "--threads") == 0 || std::strcmp(argv[i], "-t") == 0) {
            i++;
            if (i >= argc) return false;
            params.no_threads = std::atoi(argv[i]);
        }
    }
    return true;
}

bool argumentParser::parse_decompress_options(int argc, const char* argv[]) {
    for (int i = 2; i < argc; ++i) {
        if (argv[i][0] != '-') {
            return false;
        }
        if (std::strcmp(argv[i], "--mode_lossy") == 0 || std::strcmp(argv[i], "-M") == 0) {
            params.compress_mode = compress_mode_t::lossy_mode;
        } else if (!parse_common_options(i, argc, argv)) {
            return false;
        } else if (std::strcmp(argv[i], "--level") == 0 || std::strcmp(argv[i], "-l") == 0) {
            i++;
            if (i >= argc) return false;
            int temp = std::atoi(argv[i]);
            if (temp < 0 || temp > 9) return false;
            params.compression_level = temp ? argv[i][0] : 'u';
        }
    }
    return true;
}

bool argumentParser::parse_mode_option(const std::string& mode) {
    if (mode == "compress") {
        params.task_mode = task_mode_t::mcompress;
    } else if (mode == "decompress") {
        params.task_mode = task_mode_t::mdecompress;
    } else {
        return false;
    }
    return true;
}

bool argumentParser::validate_inputs() {
    if (isatty(STDIN_FILENO) && params.in_file_name == "-") {
        std::cerr << "Error: No input file specified and no data provided via stdin!" << std::endl;
        return false;
    }
    if (isatty(STDOUT_FILENO) && params.out_file_name == "-") {
        std::cerr << "Warning: No output file specified and no data provided via stdout!" << std::endl;
    }
    return true;
}


