#pragma once

/*
1 bsc
2 zstd
3 vint codec (Integer variable byte encoding)
*/

#include <stddef.h>
#include "libbsc.h"
#include "defs.h"

#include <vector>
#include <cstdint>

struct bsc_params_t {
    int block_size;
    int lzp_hash_size;
    int lzp_min_len;
    int blockSorter;
    int coder;
};

//  BSC compression algorithm interface class
class CBSCWrapper
{
private:
    bsc_params_t m_params;
	static int features;

public:
	CBSCWrapper();
	~CBSCWrapper();

	static bool InitLibrary(int _features);

	bool InitDecompress();
	bool InitCompress(int _block_size, int _lzp_hash_size, int _lzp_min_len, int _blockSorter, int _coder);
	bool InitCompress(bsc_params_t params);

	bool Compress(const std::vector<uint8_t>& v_input, std::vector<uint8_t>& v_output);
	static bool Decompress(std::vector<uint8_t>& v_input, std::vector<uint8_t>& v_output);
};

namespace vint_code
{
    uint32_t ReadVint(std::vector<uint8_t>& buffer, size_t& pos);
    
    size_t WriteVint(uint32_t value, std::vector<uint8_t>& buffer);
    
    //编码
    // 输入整数数组：arr  
    //可变字节编码后返回的字节数组 std::vector<uint8_t>
    std::vector<uint8_t> EncodeArray(const std::vector<uint32_t>& arr);

    //解码
    // 输入字节数组：buffer  
    //可变字节解码后的返回的整数组 std::vector<uint32_t>
    std::vector<uint32_t> DecodeArray(std::vector<uint8_t>& buffer);	
}
