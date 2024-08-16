#pragma once

#include <cstring>
#include <vector>
#include "defs.h"

using namespace std;

//存放VCF文件的数据结构
typedef struct variant_desc_tag {
    string chrom;
    // uint64_t pos;
    // string id;
    // string ref;
    // string alt;
    // string qual;
    // string filter;
    // string info;
    // string format;
    // // int alt_num;
    // variant_desc_tag():pos(0),ref(""){}
    // bool operator==(const struct variant_desc_tag &x)
    // {
    //     if (chrom == "" && x.chrom == "")
    //         return true;

    //     return chrom == x.chrom &&
    //            pos == x.pos;
    //     //		&&
    //     //			id == x.id;
    //     //		&&
    //     //			ref == x.ref &&
    //     //			alt == x.alt &&
    //     //			qual == x.qual &&
    //     //			filter == x.filter;
    //     //			info == x.info;
    // }

    // bool operator !=(const struct variant_desc_tag &x)
    // {
    //     return !operator==(x);
    // }

    // bool operator<(const struct variant_desc_tag &x)
    // {
    //     if (chrom != x.chrom)
    //     {
    //         if (chrom.empty())
    //             return false;
    //         if (x.chrom.empty())
    //             return true;

    //         return chrom < x.chrom;
    //     }
    //     if (pos != x.pos)
    //         return pos < x.pos;
    //     return alt < x.alt;
    // }
} variant_desc_t;


// fixed_field_block ：存放固定字段和基因型编码后的block的合并数据码流
//  no_variants：合并block，包含的变体总行数
//  chrom: CHROM字段编码后数据码流
//  id: ID字段编码后数据码流
//  alt: ALT字段编码后数据码流
//  qual: QUAL字段编码后数据码流
//  pos: POS字段编码后数据码流
//  ref: REF字段编码后数据码流
//  gt_block: GT子字段编码后数据码流
struct fixed_field_block
{
    // uint32_t no_variants;
    // vector<uint8_t> chrom;
    // vector<uint8_t> id;
    // vector<uint8_t> alt;
    // vector<uint8_t> qual;
    // vector<uint8_t> pos;
    // vector<uint8_t> ref;
    vector<uint8_t> gt_block;

    // fixed_field_block()
    // {
    //     no_variants = 0;
    //     chrom.reserve(no_variants_in_buf); // 预留空间 8MB
    //     id.reserve(no_variants_in_buf);
    //     alt.reserve(no_variants_in_buf);
    //     qual.reserve(no_variants_in_buf);
    //     pos.reserve(no_variants_in_buf);
    //     ref.reserve(no_variants_in_buf);
    //     gt_block.reserve(no_variants_in_buf);
    // }
    // fixed_field_block(uint32_t _no_variants, vector<uint8_t> _chrom, vector<uint8_t> _id, vector<uint8_t> _alt, vector<uint8_t> _qual, vector<uint8_t> _pos,
    //                   vector<uint8_t> _ref, vector<uint8_t> _gt_block)
    //     : no_variants(_no_variants), chrom(_chrom), id(_id), alt(_alt), qual(_qual), pos(_pos), ref(_ref), gt_block(_gt_block)
    // {
    // }
    // void Initalize()
    // {
    //     no_variants = 0;
    //     chrom.reserve(no_variants_in_buf);
    //     id.reserve(no_variants_in_buf);
    //     alt.reserve(no_variants_in_buf);
    //     qual.reserve(no_variants_in_buf);
    //     pos.reserve(no_variants_in_buf);
    //     ref.reserve(no_variants_in_buf);
    //     gt_block.reserve(no_variants_in_buf);
    // }
    // void Clear()
    // {
    //     no_variants = 0;
    //     chrom.clear();
    //     id.clear();
    //     alt.clear();
    //     qual.clear();
    //     pos.clear();
    //     ref.clear();
    //     gt_block.clear();
    // }
};

struct SPackage
{
    int key_id;
    int part_id;
    // uint32_t stream_id_size;
    // uint32_t stream_id_data;

    // vector<uint32_t> v_size;
    // vector<uint8_t> v_data;
    // int stream_id_src;
    // bool is_func;
    // SPackage()
    // {
    //     key_id = -1;

    //     stream_id_size = 0;
    //     stream_id_data = 0;
    //     part_id = -1;
    //     stream_id_src = -1;
    //     is_func = false;
    // }

    // SPackage(int _key_id, uint32_t _stream_id_size, uint32_t _stream_id_data, int _part_id, vector<uint32_t> &_v_size, vector<uint8_t> &_v_data)
    // {
    //     key_id = _key_id;
    //     stream_id_size = _stream_id_size;
    //     stream_id_data = _stream_id_data;
    //     stream_id_src = -1;
    //     part_id = _part_id;
    //     v_size = move(_v_size);
    //     v_data = move(_v_data);
    //     is_func = false;

    //     _v_size.clear();
    //     _v_data.clear();
    // }

    // SPackage(int _key_id, int _db_id, uint32_t _stream_id_size, uint32_t _stream_id_data, int _part_id, vector<uint32_t> &_v_size, int _stream_id_src)
    // {
    //     key_id = _key_id;
    //     stream_id_size = _stream_id_size;
    //     stream_id_data = _stream_id_data;
    //     stream_id_src = _stream_id_src;
    //     part_id = _part_id;
    //     is_func = true;
    // }
};

enum  class key_type_t {flt, info, fmt};  //其它字段类型 ：FILTER / INFO / FORMAT
typedef struct key_desc_tag
{
    uint32_t key_id;
    uint32_t actual_field_id;
    key_type_t keys_type;
    int8_t type; //one of: BCF_HT_FLAG 0 / BCF_HT_INT  1 / BCF_HT_REAL 2 / BCF_HT_STR 3
}key_desc;
