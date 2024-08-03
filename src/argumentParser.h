/*
1. Parameter declaration and initialization.
2. Parameter resolution during gsc program execution.
*/

#pragma once

#include "compressorBase.h"

#include <string>

namespace gsc{

enum class TaskMode { None, Compress, Decompress };
enum class CompressMode { Lossy, Lossless };
enum class FileType { VCF, BCF, BED };

class GSCParams{
public:
    GSCParams();
    // Constant parameters for defining compression and decompression tasks.
    static constexpr bsc_params_t p_bsc_fixed_fields = {25, 16, 64, LIBBSC_BLOCKSORTER_BWT, LIBBSC_CODER_QLFC_ADAPTIVE};
    static constexpr bsc_params_t p_bsc_meta = {25, 16, 64, LIBBSC_BLOCKSORTER_BWT, LIBBSC_CODER_QLFC_ADAPTIVE};
    static constexpr bsc_params_t p_bsc_size = {25, 19, 128, LIBBSC_BLOCKSORTER_BWT, LIBBSC_CODER_QLFC_ADAPTIVE};
    static constexpr bsc_params_t p_bsc_flag = {25, 19, 64, LIBBSC_BLOCKSORTER_BWT, LIBBSC_CODER_QLFC_ADAPTIVE};
    static constexpr bsc_params_t p_bsc_text = {25, 19, 128, LIBBSC_BLOCKSORTER_BWT, LIBBSC_CODER_QLFC_ADAPTIVE};
    static constexpr bsc_params_t p_bsc_int = {25, 19, 128, LIBBSC_BLOCKSORTER_BWT, LIBBSC_CODER_QLFC_ADAPTIVE};
    static constexpr bsc_params_t p_bsc_real = {25, 19, 64, LIBBSC_BLOCKSORTER_BWT, LIBBSC_CODER_QLFC_ADAPTIVE};

private:
    // Basic parameters for defining the compression and decompression tasks.
    TaskMode taskMode;
    CompressMode compressMode;
    FileType inputFileType;
    FileType outputFileType;
    uint32_t maxReplicationDepth;
    uint32_t maxMBMemory;
    std::string inputFileName;
    std::string outputFileName;
    std::string outputSamplesFileName;
    std::string samples;
    std::string range;
    uint32_t numThreads;
    uint32_t numGTThreads;
    uint32_t variantsInBlock;
    uint32_t ploidy;
    uint64_t vectorLength;
    uint32_t recordsToProcess;
    uint32_t numSamples;
    char compressionLevel;
    bool useMBMemory;
    size_t numBlocks;
    bool compressAll;
    bool outputACAN;
    bool outputGenotypes;
    bool outputSamplesName;
    bool splitFlag;
    bool mergeFileFlag;
    bool outputHeaderFlag;
    uint32_t minAC;
    uint32_t maxAC;
    float minAF;
    float maxAF;
    float minQual;
    float maxQual;
    std::string outputID;

    // Default settings and other logical methods define extensions.
    void setDefaultValues();

};

class ArgumentParser{

};

} // namespace gsc