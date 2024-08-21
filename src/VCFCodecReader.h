#pragma once

#include "argumentParser.h"
#include "htslib/vcf.h"
#include "fileHandler.h"
#include "variant.h"
#include "queues.h"
#include "bitOperations.h"

#include <thread>

using namespace std;

class CompressionReader
{
private:
    // OpenForReading function
    bool merge_flag;
    vector<htsFile *> merge_files;
    file_type in_type;
    bool merge_failure_flag;
    bcf_hdr_t *vcf_hdr = nullptr; // 存每个文件的 meta data
    bcf1_t *vcf_record;
    htsFile *in_file = nullptr; // 读取VCF的文件指针
    std::string in_file_name;

    // GetSamples function
    vector<string> samples_list; // 存储所有样本名
    uint32_t no_samples;
    bool ReadFile();
    bool vcf_hdr_read; // ReadFile()

    // setNoVecBlock function
    size_t chunk_size;
    uint32_t no_vec_in_block, vec_read_in_block, block_id;
    uint32_t no_fixed_fields, vec_read_fixed_fields, fixed_fields_id;

    GtBlockQueue *Gt_queue = nullptr;
    PartQueue<SPackage> * part_queue = nullptr;

    // InitVarinats function
    File_Handle_2 * file_handle2 = nullptr;
    bool GetFilterInfoFormatKeys(int &no_flt_keys, int &no_info_keys, int &no_fmt_keys, vector<key_desc> &keys); 
    int key_gt_id;
    int no_flt_keys, no_info_keys, no_fmt_keys;
    vector<key_desc> keys;
    uint32_t no_keys;
    vector<CBuffer> v_o_buf;   // BitOperation，只声明类CBuffer，未定义
    vector<int> v_buf_ids_size;
	vector<int> v_buf_ids_data;
    vector<int> FilterIdToFieldId;
    vector<int> InfoIdToFieldId;
    vector<int> FormatIdToFieldId;

    uint64_t no_vec;
    vector<pair<std::string,uint32_t>> where_chrom;
    vector<int64_t> chunks_min_pos;
    vector<uint32_t> actual_variants;

    vector<int> order;

public:
    CompressionReader()
    {
        // in_open = false;
        // vcf_hdr_read = false;
        // no_samples = 0;
        // ploidy = 0;
        // no_chrom_num = 0;
        // no_vec = 0;
        // start_flag = true;
        // field_order_flag = false;
    }

    CompressionReader(const GSCParams &params)
    {
        // in_open = false;
        // vcf_hdr_read = false;
        // no_samples = 0;
        // ploidy = params.ploidy;
        // no_chrom_num = 0;
        // in_file_name = params.in_file_name;
        // in_type = params.in_type;
        // compress_mode = params.compress_mode;
        // merge_flag = params.merge_file_flag;
        // merge_failure_flag = false;
        // v_vcf_data_compress.reserve(no_variants_in_buf);
        // actual_variants.reserve(no_variants_in_buf);
        // no_vec = 0;
        // start_flag = true;
        // field_order_flag = false;
    }

    ~CompressionReader()
    {
    }

    bool OpenForReading(string &file_name);
    bool GetHeader(string &v_header);
    uint32_t GetSamples(vector<string> &s_list);
    uint32_t setNoVecBlock(GSCParams &params);
    void setQueue(GtBlockQueue *_queue) { Gt_queue = _queue; }
    void setPartQueue(PartQueue<SPackage> *_part_queue) { part_queue = _part_queue; }
    void InitVarinats(File_Handle_2 *_file_handle2);
    
    void GetOtherField(vector<key_desc> &_keys,uint32_t &_no_keys,int &_key_gt_id);

	bool ProcessInVCF();

    uint64_t getNoVec()
    {
        return no_vec;
    }

    void GetWhereChrom(vector<pair<std::string, uint32_t>> &_where_chrom,vector<int64_t> &_chunks_min_pos);
    vector<uint32_t> GetActualVariants();
    void UpdateKeys(vector<key_desc> &_keys);

};