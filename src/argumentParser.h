/*
1. Parameter declaration and initialization.
2. Parameter resolution during gsc program execution.
*/

#pragma once

#include "compressorBase.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <unistd.h>
#include <cstring>
#include <iostream>

enum class task_mode_t { none, mcompress, mdecompress };
enum class compress_mode_t { lossy_mode, lossless_mode };
enum class file_type { VCF_File, BCF_File, BED_File };

static constexpr bsc_params_t p_bsc_fixed_fields = {25, 16, 64, LIBBSC_BLOCKSORTER_BWT, LIBBSC_CODER_QLFC_ADAPTIVE};
static constexpr bsc_params_t p_bsc_meta = {25, 16, 64, LIBBSC_BLOCKSORTER_BWT, LIBBSC_CODER_QLFC_ADAPTIVE};
static constexpr bsc_params_t p_bsc_size = {25, 19, 128, LIBBSC_BLOCKSORTER_BWT, LIBBSC_CODER_QLFC_ADAPTIVE};
static constexpr bsc_params_t p_bsc_flag = {25, 19, 64, LIBBSC_BLOCKSORTER_BWT, LIBBSC_CODER_QLFC_ADAPTIVE};
static constexpr bsc_params_t p_bsc_text = {25, 19, 128, LIBBSC_BLOCKSORTER_BWT, LIBBSC_CODER_QLFC_ADAPTIVE};
static constexpr bsc_params_t p_bsc_int = {25, 19, 128, LIBBSC_BLOCKSORTER_BWT, LIBBSC_CODER_QLFC_ADAPTIVE};
static constexpr bsc_params_t p_bsc_real = {25, 19, 64, LIBBSC_BLOCKSORTER_BWT, LIBBSC_CODER_QLFC_ADAPTIVE};

struct GSCParams {
    
    task_mode_t task_mode;                  // 任务模式
    compress_mode_t compress_mode;          // 压缩模式
    file_type in_type, out_type;             // 输入文件类型和输出文件类型
    uint32_t max_replication_depth;          // 最大复制深度
    uint32_t max_MB_memory;                  // 最大内存限制（以MB为单位）
    std::string in_file_name;                // 输入文件名
    std::string out_file_name;               // 输出文件名
    std::string out_samples_file_name;       // 输出样本文件名
    std::string samples;                     // 样本数据
    std::string range;                       // 变体范围

    // std::string merge_files;

    uint32_t no_threads;                // 线程数量
    uint32_t no_gt_threads;             // 基因型线程数量
    uint32_t var_in_block;              // 块中的变体数量
    uint32_t ploidy;                    // 倍型
    uint64_t vec_len;                   // 向量长度
    uint32_t records_to_process;        // 需要处理的记录数量
    uint32_t n_samples;                 // 样本数量

    char compression_level, mode;       // 压缩级别和模式
    bool MB_memory;                     // 是否使用MB为单位的内存？
    size_t no_blocks;                   // 块数量

    bool compress_all;                  // 是否压缩所有内容
    bool out_AC_AN;                     // 是否输出等位基因计数和等位基因数
    bool out_genotypes;                 // 是否输出基因型
    bool out_samples_name;              // 是否输出样本名称

    bool split_flag;                    // 是否分割标志
    bool merge_file_flag;               // 是否合并文件标志
    bool out_header_flag;               // 是否输出头标志
    uint32_t minAC, maxAC;              // 最小和最大等位基因计数

    float minAF, maxAF;                 // 最小和最大等位基因频率
    float min_qual, max_qual;           // 最小和最大质量分数
    std::string out_id;                 // 输出ID

};

class argumentParser{
public:
    argumentParser();
    GSCParams params;

    int usage() const;
    int usage_compress() const;
    int usage_decompress() const;

    bool parse_common_options(int& i, int argc, const char* argv[]);
    bool parse_compress_options(int argc, const char* argv[]);
    bool parse_decompress_options(int argc, const char* argv[]);
    bool parse_mode_option(const std::string& mode);
    bool validate_inputs();

// private:

};