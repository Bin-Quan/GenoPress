#pragma once

/*
1 bsc
2 zstd
3 vint codec (Integer variable byte encoding)
*/

#include <stddef.h>  // why?
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
