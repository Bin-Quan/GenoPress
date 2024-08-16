
#include "compressor.h"
#include "VCFCodecReader.h"
#include "queues.h"

#include <numeric>

bool Compressor::OpenForWriting(const string &out_file_name)
{
    if (out_file_name != "-") // -?
    {
        fname = out_file_name;
        // fname = (char *)malloc(strlen(out_file_name.c_str()) + 5);
        // snprintf(fname, strlen(out_file_name.c_str()) + 5, "%s.gsc", out_file_name.c_str());
        comp = fopen(fname.c_str(), "wb");
        if (!comp)
        {
            std::cerr << "ERROR: storing archive not successful for: `" << fname << "`" << std::endl;
            exit(1);
        }
    }
    else
    {
        comp = stdout; // 标准输出
        is_stdout = true;
    }
    if (setvbuf(comp, nullptr, _IOFBF, 64 << 20) != 0) // 缓冲区大小为64 MB
    {
        std::cerr << "ERROR: Buffer setup failed for: `" << fname << "`" << std::endl;
        // if (fname != nullptr) {
        //     free(fname);
        // }
        exit(1);
    }

    mode_type = false;

    fwrite(&mode_type, sizeof(mode_type), 1, comp);

    other_fields_offset = ftell(comp) + sizeof(uint64_t);
    fwrite(&other_fields_offset, sizeof(other_fields_offset), 1, comp);

    sdsl_offset = ftell(comp) + sizeof(uint64_t); // 存全零行和复制行的标记向量

    if (fwrite(&sdsl_offset, sizeof(sdsl_offset), 1, comp) != 1)
    {
        std::cerr << "ERROR: Write operation failed for: `" << fname << "`" << std::endl;
    }

    // setvbuf(comp, nullptr, _IOFBF, 64 << 20);
    // sdsl_offset = ftell(comp) + sizeof(uint64_t);
    // fwrite(&sdsl_offset, sizeof(sdsl_offset), 1, comp);

    // 初始化一个流对象
    int id = (int)chunks_streams.size();
    chunks_streams[id] = chunk_stream(0, 0);

    return true;
}

bool Compressor::OpenTempFile(const string &out_file_name)
{
    temp_file1_fname = (char *)malloc(strlen(out_file_name.c_str()) + 5);

    snprintf(temp_file1_fname, strlen(out_file_name.c_str()) + 5, "%s.temp", out_file_name.c_str()); // 第一个临时文件

    temp_file = fopen(temp_file1_fname, "wb");
    if (!temp_file)
    {
        std::cerr << "ERROR: storing archive not successful for: `" << temp_file1_fname << "`" << std::endl;
        exit(1);
    }

    setvbuf(temp_file, nullptr, _IOFBF, 64 << 20); // 临时文件缓冲区

    if (params.compress_mode == compress_mode_t::lossless_mode)
    {
        if (file_handle2)
            delete file_handle2;
        file_handle2 = new File_Handle_2(false);

        temp_file2_fname = out_file_name + ".com_tmp_gsc"; // 第二个临时文件

        if (!file_handle2->Open(temp_file2_fname))
        {
            cerr << "Cannot open " << temp_file2_fname << "\n";
            return false;
        }
    }
    return true;
}

// compressor meta data
//  v_samples : 存放样本名
//  v_header : 存放元信息行
bool Compressor::compress_meta(vector<string> v_samples, const string &v_header)
{
    append_str(all_v_header, v_header);

    for (auto &x : v_samples)
        append_str(all_v_samples, x);

    // 打包成一个元组，并附加一个字符串标签
    for (auto data : {
             make_tuple(ref(all_v_header), ref(comp_v_header), "header"),
             make_tuple(ref(all_v_samples), ref(comp_v_samples), "samples"),
         })
    {
        CBSCWrapper bsc;

        bsc.InitCompress(p_bsc_meta);
        bsc.Compress(get<0>(data), get<1>(data));

        // zstd::zstd_compress(get<0>(data), get<1>(data));

        // LZMACompress::Compress(get<0>(data), get<1>(data), 9);

        // lz4:: lz4_compress(get<0>(data), get<1>(data), 12);
        // BrotliUtils::compressData(get<0>(data), get<1>(data));
        // std::cerr<<get<0>(data).size()<<":"<<get<1>(data).size()<<endl;
        // lzma2::lzma2_compress(get<0>(data), get<1>(data), get<2>(data), 10);
        // std::cerr << get<2>(data) << " size: " << get<1>(data).size() << endl;

        // fh.WriteUInt(get<1>(data).size(), 4, f);
        // fh.Write(get<1>(data).data(), get<1>(data).size(), f);
    }

    return true;
}

bool Compressor::CompressProcess()
{
    CBSCWrapper::InitLibrary(p_bsc_features);
    MyBarrier my_barrier(3); // 3 threads 作用分别是：？

    unique_ptr<CompressionReader> compression_reader(new CompressionReader(params));

    if (!compression_reader->OpenForReading(params.in_file_name))
    {
        cerr << "Cannot open: " << params.in_file_name << endl;
        return false;
    }
    if (!OpenForWriting(params.out_file_name))
        return false;

    if (!OpenTempFile(params.out_file_name))
        return false;

    string header;
    vector<string> v_samples;
    compression_reader->GetHeader(header);
    uint32_t no_samples = compression_reader->GetSamples(v_samples);
    std::cerr << "no_samples:" << no_samples << endl;

    if (!no_samples)
    {
        std::cerr << "The number of genotype samples is zero and cannot be compressed!\n";
        return false;
    }

    compress_meta(v_samples, header);

    compression_reader->setNoVecBlock(params);
    std::cerr << "no_gt_threads:" << params.no_gt_threads << endl;

    GtBlockQueue inGtBlockQueue(max((int)(params.no_blocks * params.no_gt_threads), 8));    // 管理基因型块的队列
    VarBlockQueue<fixed_field_block> sortVarBlockQueue(max((int)params.no_threads * 2, 8)); // 变异块队列
    compression_reader->setQueue(&inGtBlockQueue);

    PartQueue<SPackage> part_queue(max((int)params.no_threads * 2, 8));

    if (params.compress_mode == compress_mode_t::lossless_mode)
    {
        compression_reader->setPartQueue(&part_queue);
        compression_reader->InitVarinats(file_handle2); // 处理较复杂
        compression_reader->GetOtherField(keys, no_keys, key_gt_id);

        InitCompressParams();

        part_compress_thread.reserve(params.no_threads);

        for (uint32_t i = 0; i < params.no_threads; ++i)
        {
            part_compress_thread.emplace_back(thread([&]()
                                                     {
                                                         SPackage pck;
                                                         vector<uint8_t> v_compressed;
                                                         vector<uint8_t> v_tmp;

                                                         auto fo = [this](SPackage &pck) -> bool
                                                         { return check_coder_compressor(pck); };

                                                         while (true)
                                                         {
                                                             if (!part_queue.Pop<SPackage>(pck, fo))
                                                                 break;
                                                             compress_other_fileds(pck, v_compressed, v_tmp);
                                                         } }));
        }
    }

    block_size = no_samples * params.ploidy * 2;

    unique_ptr<thread> compress_thread(new thread([&]
                                                  {
                                                      fixed_field_block fixed_field_block_process;
                                                      while (true)
                                                      {
                                                          if (!sortVarBlockQueue.Pop(fixed_field_block_id, fixed_field_block_process))
                                                          {
                                                              break;
                                                          }
                                                          compressFixedFields(fixed_field_block_process);  //tmp
                                                      } }));

    // create multiple threads to handle individual blocks
    block_process_thread.reserve(params.no_gt_threads);
    string prev_chrom = "";
    int chunk_id = 0;

    for (uint32_t i = 0; i < params.no_gt_threads; ++i)
    {
        block_process_thread.emplace_back(thread([&]()
                                                 {
            int block_id = 0;
            unsigned long num_rows;
            unsigned char *data = nullptr;
            vector<variant_desc_t> v_vcf_data_io;             
            vector<uint32_t> origin_of_copy;
            origin_of_copy.reserve(no_variants_in_buf);
            vector<uint8_t> samples_indexes;   //Index of the location where the block storing 1 is stored.
            vector<uint32_t> perm;  // 存储样本排列顺序的向量
            perm.clear();
            perm.resize(no_samples * params.ploidy, 0);
            for (size_t i_p = 0; i_p < perm.size(); i_p++)
                perm[i_p] = i_p;
            BlockProcess block_process(params);

            while (true)
            {
                if (!inGtBlockQueue.Pop(block_id, data, num_rows, v_vcf_data_io)){
                    break;
                }

                vector<bool> zeros_only(num_rows, false);
                vector<bool> copies(num_rows, false);

                block_process.SetCurBlock(num_rows, data);
                //Gets the sparse encoding for each block        
                          
                if (num_rows){    
                    // if(num_rows == block_size)  
                        block_process.ProcessSquareBlock(perm, zeros_only, copies, origin_of_copy,samples_indexes,true);
                    // else
                    //     block_process.ProcessLastBlock(zeros_only, copies, origin_of_copy,samples_indexes);

                    if(num_rows == block_size)
                        block_process.ProcessVariant(perm , v_vcf_data_io);

                }
                
                if(data != nullptr)
                    delete[] data;

                //Gets the sparse encoding for each block  如此多的if else是否可以交给AI自动识别？
                lock_gt_block_process(block_id);
                {
                    // if(!cur_block_id)
                    //     prev_chrom = v_vcf_data_io[0].chrom;
                    // if(num_rows != block_size)                    
                    //     block_process.ProcessLastPerm(perm,vint_last_perm);
                    
                    if(prev_chrom != v_vcf_data_io[0].chrom)
                    {
                        prev_chrom = v_vcf_data_io[0].chrom;
                        if(no_curr_chrom_block)
                        {
                            sortVarBlockQueue.Push(chunk_id,fixed_field_block_io);
                            // compressFixedFields(fixed_field_block_io);
                            toal_all_size += fixed_field_block_io.gt_block.size();
                            int id = (int) chunks_streams.size();                
	                        chunks_streams[id] = chunk_stream(cur_chunk_actual_pos,0);
                            
                            no_curr_chrom_block = 0;
                            prev_pos = 0;
                            chunk_id++;
                            fixed_field_block_io.Clear();
                        }
                        if(num_rows)
                        {
                            cur_chunk_actual_pos += (uint32_t)v_vcf_data_io.size();
                            block_process.addSortFieldBlock(fixed_field_block_io,all_zeros,all_copies,comp_pos_copy,zeros_only, copies, origin_of_copy,samples_indexes,v_vcf_data_io,prev_pos);
                            no_curr_chrom_block++;
                            if(num_rows % block_size){
                                vint_last_perm.emplace(chunk_id,vint_code::EncodeArray(perm));
                            }

                            if(no_curr_chrom_block == params.no_blocks){
                                sortVarBlockQueue.Push(chunk_id,fixed_field_block_io);
                                // compressFixedFields(fixed_field_block_io);
                                toal_all_size += fixed_field_block_io.gt_block.size();
                                int id = (int) chunks_streams.size();               
                                chunks_streams[id] = chunk_stream(cur_chunk_actual_pos,0);
                                no_curr_chrom_block = 0;
                                prev_pos = 0;
                                chunk_id++;
                                fixed_field_block_io.Clear();
                            }
                        }
                        else{
                            sortVarBlockQueue.Complete();
                        }                            
                    }
                    else{

                        cur_chunk_actual_pos += (uint32_t)v_vcf_data_io.size();
                        block_process.addSortFieldBlock(fixed_field_block_io, all_zeros, all_copies, comp_pos_copy, zeros_only, copies, origin_of_copy,samples_indexes,v_vcf_data_io,prev_pos);
                        no_curr_chrom_block++;

                        if(num_rows % block_size){
                            vint_last_perm.emplace(chunk_id,vint_code::EncodeArray(perm));
                        }

                        if(no_curr_chrom_block == params.no_blocks){
                            sortVarBlockQueue.Push(chunk_id,fixed_field_block_io);
                            // compressFixedFields(fixed_field_block_io);
                            toal_all_size += fixed_field_block_io.gt_block.size();
                            int id = (int) chunks_streams.size();               
	                        chunks_streams[id] = chunk_stream(cur_chunk_actual_pos,0);
                            no_curr_chrom_block = 0;
                            prev_pos = 0;
                            chunk_id++;
                            fixed_field_block_io.Clear();
                        }
                    }           
                }
                unlock_gt_block_process();                   
                
            } }));
    }

    return true;
}

// 初始化bsc压缩参数
void Compressor::InitCompressParams()
{
    v_coder_part_ids.resize(no_keys, 0);
    v_bsc_data.resize(no_keys);
    v_bsc_size.resize(no_keys);

    for (uint32_t i = 0; i < no_keys; ++i)
    {
        v_bsc_data[i] = new CBSCWrapper();
        v_bsc_size[i] = new CBSCWrapper();
        v_bsc_size[i]->InitCompress(p_bsc_size);

        switch (keys[i].type)
        {
        case BCF_HT_FLAG:
            v_bsc_data[i]->InitCompress(p_bsc_flag);
            break;
        case BCF_HT_INT:
            v_bsc_data[i]->InitCompress(p_bsc_int);
            break;
        case BCF_HT_REAL:
            v_bsc_data[i]->InitCompress(p_bsc_real);
            break;
        case BCF_HT_STR:
            v_bsc_data[i]->InitCompress(p_bsc_text);
            break;
        }
    }
}

bool Compressor::check_coder_compressor(SPackage &pck)
{
    unique_lock<mutex> lck(mtx_v_coder);
    int sid = pck.key_id;

    return (int)v_coder_part_ids[sid] == pck.part_id;
}

void Compressor::compress_other_fileds(SPackage &pck, vector<uint8_t> &v_compressed, vector<uint8_t> &v_tmp)
{
}

bool Compressor::compressFixedFields(fixed_field_block &fixed_field_block_io)
{
}
