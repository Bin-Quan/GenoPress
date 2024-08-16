#pragma once

#include "argumentParser.h"
#include "variant.h"

#include <vector>
using namespace std;

class BlockProcess
{
private:
    GSCParams params;
    uint32_t no_copy;
    uint32_t no_samples_index;
    uint32_t perm_lut8[8];
    uint64_t perm_lut64[64];

    uint8_t *data = nullptr;
    uint64_t cur_no_vec;

    void permute_range_vec(uint64_t id_start, uint64_t id_stop, vector<uint32_t> &v_perm,vector<bool> &zeros, vector<bool> &copies, vector<uint32_t> &origin_of_copy, vector<uint8_t> &samples_indexes);
    inline void get_perm(vector<uint32_t> perm, int n,vector<variant_desc_t> &v_vcf_data_compress);  // 未实现


public:
    BlockProcess(GSCParams &_params)
    {
        params = _params;
        no_copy = 0;
        // prev_pos = 0;
        no_samples_index = 0;
        for (int i = 0; i < 8; ++i)
            perm_lut8[i] = 1 << (7 - i);
        for (int i = 0; i < 64; ++i)
            perm_lut64[i] = 1ull << i;
    }
    ~BlockProcess() {}

    void SetCurBlock(uint64_t _cur_no_vec, uint8_t *cur_data);
    void ProcessSquareBlock(vector<uint32_t> &perm,vector<bool> &zeros, vector<bool> &copies, vector<uint32_t> &origin_of_copy, vector<uint8_t> &samples_indexes, bool permute = true);
    void ProcessVariant(vector<uint32_t> &perm, vector<variant_desc_t> &v_vcf_data_io);

};