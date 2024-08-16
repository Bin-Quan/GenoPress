
#include "VCFCodecReader.h"
#include <fstream>
#include <sstream>

bool CompressionReader::OpenForReading(string &file_name)
{
    if (merge_flag) // 读取多个文件
    {
        bcf_hdr_t *temp_vcf_hdr = nullptr;
        vector<string> merge_file_names;
        long size = 0;

        if (file_name[0] == '@')
        {
            std::ifstream directory_file_name(file_name.substr(1));
            if (!directory_file_name.is_open())
            {
                std::cerr << "Error. Cannot open " << file_name.substr(1) << " file with samples.\n";
                exit(1);
            }
            std::string item;

            while (directory_file_name >> item)
            {
                merge_file_names.emplace_back(item);
                size++;
            }
            std::cerr << "size:" << size << endl;
            directory_file_name.clear();
        }
        else
        {
            char delim = ',';
            std::stringstream ss(file_name);
            std::string item;
            while (getline(ss, item, delim))
            {
                merge_file_names.emplace_back(item);
                size++;
            }
        }

        merge_files.resize(size);

        for (long i = 0; i < size; i++)
        {
            if (merge_files[i])
                hts_close(merge_files[i]);
            if (in_type == file_type::VCF_File)
            {
                merge_files[i] = hts_open(merge_file_names[i].c_str(), "r"); // 文本文件
            }
            else
            {
                merge_files[i] = hts_open(merge_file_names[i].c_str(), "rb"); // 二进制文件
            }
            if (!merge_files[i])
            {
                std::cerr << "could not open " << merge_file_names[i] << " file" << std::endl;
                merge_failure_flag = true;
                return false;
            }

            // 设置文件的缓存和块大小选项
            hts_set_opt(merge_files[i], HTS_OPT_CACHE_SIZE, 32 << 20);
            hts_set_opt(merge_files[i], HTS_OPT_BLOCK_SIZE, 32 << 20);

            temp_vcf_hdr = bcf_hdr_read(merge_files[i]);
            vcf_hdr = bcf_hdr_merge(vcf_hdr, temp_vcf_hdr);
        }
        vcf_record = bcf_init();
    }
    else // 读取单个文件
    {
        if (in_file)
            hts_close(in_file);

        if (in_type == file_type::VCF_File)
        {
            in_file = hts_open(file_name.c_str(), "r");
        }
        else
        {
            in_file = hts_open(file_name.c_str(), "rb");
        }
        if (!in_file)
        {
            std::cerr << "could not open " << in_file_name << " file" << std::endl;
            return false;
        }

        hts_set_opt(in_file, HTS_OPT_CACHE_SIZE, 32 << 20);
        hts_set_opt(in_file, HTS_OPT_BLOCK_SIZE, 32 << 20);

        if (vcf_hdr)
            bcf_hdr_destroy(vcf_hdr);

        vcf_hdr = bcf_hdr_read(in_file);
        int thread_count = 4;
        hts_set_threads(in_file, thread_count);
        vcf_record = bcf_init();
    }

    return true;
}

bool CompressionReader::GetHeader(string &v_header)
{
    if (!vcf_hdr)
        return false;
    kstring_t str = {0, 0, nullptr}; // 动态字符串处理的结构体
    // bcf_hdr_remove(vcf_hdr,BCF_HL_INFO, NULL);
    bcf_hdr_format(vcf_hdr, 0, &str);
    char *ptr = strstr(str.s, "#CHROM");
    v_header.assign(str.s, ptr - str.s);
    if (str.m)
        free(str.s);
    return true;
}

// Get number of samples in VCF
// no_samples：存放样本数量
// s_list：存放样本名
uint32_t CompressionReader::GetSamples(vector<string> &s_list)
{
    if (!vcf_hdr_read)
    {
        ReadFile();
    }
    s_list = samples_list;

    return no_samples;
}

bool CompressionReader::ReadFile()
{
    if (!(in_file || !merge_failure_flag) || !vcf_hdr)
        return -1;

    no_samples = bcf_hdr_nsamples(vcf_hdr); // 文件头部中样本的数量
    if (!vcf_hdr_read)
    {
        for (size_t i = 0; i < no_samples; i++)
            samples_list.emplace_back(vcf_hdr->samples[i]);

        vcf_hdr_read = true;
    }
    return true;
}

uint32_t CompressionReader::setNoVecBlock(GSCParams &params)
{
    params.var_in_block = no_samples * params.ploidy;

    int numThreads = std::thread::hardware_concurrency() / 2; // 硬件线程数，减半避免占用所有资源

    int numChunks = 1 + (params.var_in_block / 1024); // 大致需要多少块来处理变异数据，大致需要多少块来处理变异数据

    if (numChunks < numThreads) // 防止创建过多的空闲线程
    {
        numThreads = numChunks;
    }

    // params.no_gt_threads = numThreads;
    params.no_gt_threads = 1;

    if (params.var_in_block < 1024)
    {
        chunk_size = CHUNK_SIZE1;
    }
    else if (params.var_in_block < 4096)
    {
        chunk_size = CHUNK_SIZE2;
    }
    else if (params.var_in_block < 8192)
    {
        chunk_size = CHUNK_SIZE3;
    }
    else
    {
        chunk_size = params.var_in_block;
    }

    params.no_blocks = chunk_size / params.var_in_block;
    no_fixed_fields = params.no_blocks * params.var_in_block; // 画图理解

    if (params.task_mode == task_mode_t::mcompress)
    {
        no_vec_in_block = params.var_in_block * 2;                                      // bit向量表示？
        params.vec_len = params.var_in_block / 8 + ((params.var_in_block % 8) ? 1 : 0); // 位变字节
        params.n_samples = no_samples;
    }

    return 0;
}

// 初始化基因型数据压缩所需的变量和缓冲区，建立映射表
void CompressionReader::InitVarinats(File_Handle_2 *_file_handle2)
{
    file_handle2 = _file_handle2;
    GetFilterInfoFormatKeys(no_flt_keys, no_info_keys, no_fmt_keys, keys);
    no_keys = no_flt_keys + no_info_keys + no_fmt_keys;
    v_o_buf.resize(no_keys);
    for (uint32_t i = 0; i < no_keys; i++)
        v_o_buf[i].SetMaxSize(max_buffer_size, 0);
    v_buf_ids_size.resize(no_keys, -1);
    v_buf_ids_data.resize(no_keys, -1);
    // std::cerr << "no_keys:" << no_keys << endl;
    for (uint32_t i = 0; i < no_keys; i++) // 根据键的类型（flt、info、fmt），将它们映射到不同的字段ID向量中
    {
        switch (keys[i].keys_type)
        {
        case key_type_t::flt:
            if (keys[i].key_id >= FilterIdToFieldId.size())
                FilterIdToFieldId.resize(keys[i].key_id + 1, -1);
            FilterIdToFieldId[keys[i].key_id] = i;
            break;
        case key_type_t::info:
            // std::cerr << "no_samples:" <<keys[i].key_id << endl;
            if (keys[i].key_id >= InfoIdToFieldId.size())
                InfoIdToFieldId.resize(keys[i].key_id + 1, -1);
            InfoIdToFieldId[keys[i].key_id] = i;
            break;
        case key_type_t::fmt:
            if (keys[i].key_id >= FormatIdToFieldId.size())
                FormatIdToFieldId.resize(keys[i].key_id + 1, -1);
            FormatIdToFieldId[keys[i].key_id] = i;
            // std::cerr<<"InfoIdToFieldId:"<<FormatIdToFieldId[keys[i].key_id]<<endl;
            break;
        }
    }

    // file_handle2->RegisterStream("start");
    for (uint32_t i = 0; i < no_keys; i++)
    {
        v_buf_ids_size[i] = file_handle2->RegisterStream("key_" + to_string(i) + "_size");
        // std::cerr<<v_buf_ids_size[i]<<endl;
    }
    for (uint32_t i = 0; i < no_keys; i++)
    {
        v_buf_ids_data[i] = file_handle2->RegisterStream("key_" + to_string(i) + "_data");
        // std::cerr<<v_buf_ids_data[i]<<endl;
    }
}

// 从元信息行中获取Filter/Info/Format字段中子字段的数量和对应的keys表
// no_flt_keys: Filter包含子字段的数量
// no_info_keys: Info包含子字段的数量
// no_fmt_keys: Format包含子字段的数量
bool CompressionReader::GetFilterInfoFormatKeys(int &no_flt_keys, int &no_info_keys, int &no_fmt_keys, vector<key_desc> &keys)
{
    if (!vcf_hdr)
        return false;

    no_flt_keys = 0;
    no_info_keys = 0;
    no_fmt_keys = 0;

    key_desc new_key;
    // std::cerr<<vcf_hdr->nhrec<<endl;
    for (int i = 0; i < vcf_hdr->nhrec; i++) // 遍历 VCF 头部的记录
    {
        if (vcf_hdr->hrec[i]->type == BCF_HL_FLT || vcf_hdr->hrec[i]->type == BCF_HL_INFO || vcf_hdr->hrec[i]->type == BCF_HL_FMT)
        {
            // checking if it is a duplicate; if so, curr_dict_id different (and not increased)
            int id = bcf_hdr_id2int(vcf_hdr, BCF_DT_ID, vcf_hdr->hrec[i]->vals[0]);

            new_key.key_id = id;
            if (vcf_hdr->hrec[i]->type == BCF_HL_FLT)
            {
                no_flt_keys++;
                new_key.keys_type = key_type_t::flt;
                new_key.type = BCF_HT_FLAG;
            }
            else if (vcf_hdr->hrec[i]->type == BCF_HL_INFO || vcf_hdr->hrec[i]->type == BCF_HL_FMT)
            {
                if (vcf_hdr->hrec[i]->type == BCF_HL_FMT) // FORMAT
                {
                    no_fmt_keys++;
                    new_key.keys_type = key_type_t::fmt;
                    new_key.type = bcf_hdr_id2type(vcf_hdr, BCF_HL_FMT, id);
                    if (strcmp(vcf_hdr->id[BCF_DT_ID][id].key, "GT") == 0)
                    {
                        key_gt_id = (int)keys.size();
                    }
                }
                else // INFO
                {
                    no_info_keys++;
                    new_key.keys_type = key_type_t::info;
                    new_key.type = bcf_hdr_id2type(vcf_hdr, BCF_HL_INFO, id);
                }
            }
            new_key.actual_field_id = (uint32_t)keys.size();
            // std::cerr<<"new_key.actual_field_id:"<<new_key.key_id<<":"<<new_key.actual_field_id<<endl;
            keys.emplace_back(new_key);
        }
    }
    keys.shrink_to_fit(); // 调整 keys 向量的容量，以最小化内存使用

    return true;
}

void CompressionReader::GetOtherField(vector<key_desc> &_keys,uint32_t &_no_keys,int &_key_gt_id)
{
    _keys = keys;
    _no_keys = no_keys;
    _key_gt_id = key_gt_id;
}