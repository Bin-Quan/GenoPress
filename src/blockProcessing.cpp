
#include "blockProcessing.h"

void BlockProcess::SetCurBlock(uint64_t _cur_no_vec, uint8_t *cur_data)
{
    data = cur_data;
    cur_no_vec = _cur_no_vec;    
}

void BlockProcess::ProcessSquareBlock(vector<uint32_t> &perm,vector<bool> &zeros, vector<bool> &copies, vector<uint32_t> &origin_of_copy, vector<uint8_t> &samples_indexes, bool permute = true)
{
    if (permute)
    {
        //对基因型块进行预处理：包括使用最近邻贪心算法排序、列异或、标记全零行和复制行、稀疏编码、差分编码和可变字节编码
        permute_range_vec(0, cur_no_vec, perm,zeros, copies, origin_of_copy,samples_indexes);
    }
    else
    {
        perm.clear();

        perm.resize(params.vec_len * 8, 0);

        for (int i = 0; i < (int)params.vec_len * 8; ++i)
            perm[i] = i;
    }
}

// ProcessVariant 用于将perm顺序隐藏在固定字段的POS字段中
// perm 存放基因型比特矩阵重排的顺序
// v_vcf_data_io 存放固定字段的数据结构
void BlockProcess::ProcessVariant(vector<uint32_t> &perm, vector<variant_desc_t> &v_vcf_data_io)
{
    if (v_vcf_data_io.size() == perm.size())
    {
        get_perm(perm, perm.size(), v_vcf_data_io);
    }
}

//permute_range_vec对基因型块进行预处理：包括使用最近邻贪心算法排序、列异或、标记全零行和复制行、稀疏编码、差分编码和可变字节编码
//id_start  基因型比特矩阵的开始行id
//id_stop  基因型比特矩阵的结束行id
// v_perm  存放排序后的顺序（大小为 单倍型数量 = 倍性*样本数）
// zeros   存放全零行的标记，全零行在对应的下标数组中标记为1 （大小等于 比特块的行数）
// copies  存放复制行的标记，复制行在对应的下标数组中标记为1 （大小等于 比特块的行数）
// origin_of_copy  存放复制行的原始行位置
// samples_indexes 存放矩阵经过一系列编码后的字节
void BlockProcess::permute_range_vec(uint64_t id_start, uint64_t id_stop, vector<uint32_t> &v_perm,vector<bool> &zeros, vector<bool> &copies, vector<uint32_t> &origin_of_copy, vector<uint8_t> &samples_indexes)
{

}

