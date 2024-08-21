
#pragma once

#include "argumentParser.h"
#include "utils.h"
#include "fileHandler.h"
#include "variant.h"
#include "blockProcessing.h"
#include "sdsl/bit_vectors.hpp"
#include "bitOperations.h"

#include <map>
#include <thread>
#include <mutex>

using namespace std;

class Compressor
{
private:
    GSCParams params;
    uint64_t copy_no = 0;
    uint64_t unique_no = 0;
    uint32_t no_blocks = 0;
    bool input_pos;

    // OpenForWriting function
    string fname;
    FILE *comp = nullptr;
    bool is_stdout = false;
    bool mode_type; // ？
    uint64_t other_fields_offset;
    uint64_t sdsl_offset;
    std::map<int, chunk_stream> chunks_streams;

    // OpenTempFile function
    char *temp_file1_fname = nullptr; // 处理低级别的细节。例如字符串的长度、内存分配和释放等。容易出错，例如缓冲区溢出和内存泄漏。
    string temp_file2_fname;          // 提供了许多易用和安全的字符串操作方法，例如连接、比较、查找、替换等，极大简化了字符串处理。
    FILE *temp_file = nullptr;
    File_Handle_2 *file_handle2 = nullptr; // 暂时只写构造函数和析构函数

    // CompressProcess function
    bool compress_meta(vector<string> v_samples, const string &v_header);
    vector<uint8_t> all_v_header, comp_v_header; // compress_meta
    vector<uint8_t> all_v_samples, comp_v_samples;

    // GetOtherField function
    vector<key_desc> keys;
    uint32_t no_keys;
    int key_gt_id;

    void InitCompressParams();
    vector<uint32_t> v_coder_part_ids;
    vector<CBSCWrapper *> v_bsc_size;
    vector<CBSCWrapper *> v_bsc_data;

    vector<thread> part_compress_thread;

    bool check_coder_compressor(SPackage &pck);
    mutex mtx_v_coder;

    void compress_other_fileds(SPackage &pck, vector<uint8_t> &v_compressed, vector<uint8_t> &v_tmp);

    size_t block_size;
    uint32_t fixed_field_block_id = 0;
    bool compressFixedFields(fixed_field_block &field_block_io);

    vector<thread> block_process_thread;

    mutex mtx_gt_block;
    condition_variable cv_gt_block;
    int cur_block_id = 0;

    inline void lock_gt_block_process(int &_block_id)
    {
        unique_lock<mutex> lck(mtx_gt_block);
        cv_gt_block.wait(lck, [&, this]
                         { return cur_block_id == _block_id; });
    }
    inline void unlock_gt_block_process()
    {
        lock_guard<mutex> lck(mtx_gt_block);
        ++cur_block_id;
        cv_gt_block.notify_all();
    }

    uint32_t no_curr_chrom_block = 0;
    size_t toal_all_size = 0;
    int64_t prev_pos = 0;
    fixed_field_block fixed_field_block_io, fixed_field_block_compress;
    uint32_t cur_chunk_actual_pos = 0;
    map<uint32_t, vector<uint8_t>> vint_last_perm;

    vector<bool> all_zeros;
    vector<bool> all_copies;
    vector<uint32_t> comp_pos_copy;

    uint64_t no_vec;
    vector<pair<std::string, uint32_t>> where_chrom;
    vector<int64_t> chunks_min_pos;

    void compressReplicatedRow();
    bool writeCompressFlie();

	condition_variable cv_v_coder;
    void lock_coder_compressor(SPackage& pck);
    void unlock_coder_compressor(SPackage& pck);

    void Encoder(vector<uint8_t>& v_data, vector<uint8_t>& v_tmp);

    bool writeTempFlie(fixed_field_block &fixed_field_block_io);

    sdsl::bit_vector zeros_only_bit_vector[2];
    sdsl::bit_vector copy_bit_vector[2];
    sdsl::bit_vector unique;
    sdsl::rank_support_v5<> rank_unique;
    uint32_t used_bits_cp;
    CBitMemory bm_comp_copy_orgl_id;
    char bits_used(unsigned int n);

    uint64_t Meta_comp_size = 0;

public:


    Compressor(GSCParams &_params)
    {
        params = _params;
        // curr_vec_id = 0;
        copy_no = 0;
        unique_no = 0;
        no_blocks = 0;
        input_pos = true;
        all_zeros.reserve(no_variants_in_buf);
        all_copies.reserve(no_variants_in_buf);
        comp_pos_copy.reserve(no_variants_in_buf);

        chunks_min_pos.reserve(no_variants_in_buf);
    }

    ~Compressor()
    {
        for (auto p : v_bsc_size)
            if (p)
                delete p;

        for (auto p : v_bsc_data)
            if (p)
                delete p;

        if (file_handle2)
            delete file_handle2;
        // if(fname)
        //     free(fname);
        if (temp_file1_fname)
            free(temp_file1_fname);
        // if(comp)
        //     fclose(comp);
    }

    bool CompressProcess();
    bool OpenForWriting(const string &out_file_name);
    bool OpenTempFile(const string &out_file_name);
};
