#pragma once

#include "defs.h"
#include <cstdint>

// 处理各种类型的数据缓冲区
class CBuffer // 首次使用在 CompressionReader::InitVarinats
{
private:
	uint32_t max_size;
#ifndef IGNORE_OFFSET_IN_FIRST_BLOCK
	uint32_t offset;
#endif

public:
    CBuffer() {}
    ~CBuffer() {}

    void SetMaxSize(uint32_t _max_size, uint32_t _offset)
    {
        max_size = _max_size;
        #ifndef IGNORE_OFFSET_IN_FIRST_BLOCK
        offset = _offset;
        #endif
    }
};