
#include "compress.h"
#include "decompress.h"
#include "argumentParser.h"

#include <iostream>

int usage(){

    std::cerr << "Usage: gsc [option] [arguments] " << std::endl;
    std::cerr << "Available options: " << std::endl;
    std::cerr << "\tcompress - compress VCF/BCF file" << std::endl;
    std::cerr << "\tdecompress - query and decompress to VCF/BCF file " << std::endl;
    
    exit(0);
}

int usage_compress(){

    std::cerr << "Usage of gsc compress:\n\n"
                << "\tgsc compress [options] [--in [in_file]] [--out [out_file]]\n\n"
                << "Where:\n\n"
                << "\t[options]              Optional flags and parameters for compression.\n"
                << "\t-i,  --in [in_file]    Specify the input file (default: VCF or VCF.GZ). If omitted, input is taken from standard input (stdin).\n"
                << "\t-o,  --out [out_file]  Specify the output file. If omitted, output is sent to standard output (stdout).\n\n"
                
                << "Options:\n\n"
                << "\t-M,  --mode_lossy      Choose lossy compression mode (lossless by default).\n"
                << "\t-b,  --bcf             Input is a BCF file (default: VCF or VCF.GZ).\n"
                << "\t-p,  --ploidy [X]      Set ploidy of samples in input VCF to [X] (default: 2).\n"
                << "\t-t,  --threads [X]     Set number of threads to [X] (default: 1).\n"
                << "\t-d,  --depth [X]       Set maximum replication depth to [X] (default: 100, 0 means no matches).\n"
                << "\t-m,  --merge [X]       Specify files to merge, separated by commas (e.g., -m chr1.vcf,chr2.vcf), or '@' followed by a file containing a list of VCF files (e.g., -m @file_with_IDs.txt). By default, all VCF files are compressed.\n"
                << std::endl;

    exit(0);
    
}

int usage_decompress(){

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

int main(){
    
    return 0;
}