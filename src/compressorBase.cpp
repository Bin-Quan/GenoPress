
#include "compressorBase.h"

int CBSCWrapper::features;

bool CBSCWrapper::InitLibrary(int _features)
{
    features = _features;
    bsc_init(features);

    return true;
}

bool CBSCWrapper::InitCompress(int _block_size, int _lzp_hash_size, int _lzp_min_len, int _blockSorter, int _coder)
{
    block_size = _block_size;
    lzp_hash_size = _lzp_hash_size;
    lzp_min_len = _lzp_min_len;
    blockSorter = _blockSorter;
    coder = _coder;

    return true;
}

bool CBSCWrapper::InitCompress(bsc_params_t params)
{
    block_size = params.block_size;
    lzp_hash_size = params.lzp_hash_size;
    lzp_min_len = params.lzp_min_len;
    blockSorter = params.blockSorter;
    coder = params.coder;

    return true;
}

bool CBSCWrapper::InitDecompress()
{
    return true;
}

bool CBSCWrapper::Compress(const vector<uint8_t> &v_input, vector<uint8_t> &v_output)
{

    const unsigned char *vi = (const unsigned char *)v_input.data();

    v_output.resize(v_input.size() + LIBBSC_HEADER_SIZE);

    unsigned char *vo = (unsigned char *)v_output.data();

    auto c_size = bsc_compress(vi, vo, (int)v_input.size(), lzp_hash_size, lzp_min_len, blockSorter, coder, features);

    if (c_size == LIBBSC_NOT_COMPRESSIBLE)
        c_size = bsc_store(vi, vo, (int)v_input.size(), features);

    v_output.resize(c_size);

    v_output.shrink_to_fit();

    return true;
}

bool CBSCWrapper::Decompress(vector<uint8_t> &v_input, vector<uint8_t> &v_output)
{
    vector<uint8_t> vi = v_input;
    vector<uint8_t> vo;

    int p_block_size;
    int p_data_size;

    const unsigned char *ci = (const unsigned char *)vi.data();
    bsc_block_info(ci, LIBBSC_HEADER_SIZE, &p_block_size, &p_data_size, 0);

#ifdef LOG_INFO
    cout << "Block size " << p_block_size << endl;
    cout << "Data size  " << p_data_size << endl;
#endif

    unsigned char *co = (unsigned char *)malloc(p_data_size);

    bsc_decompress(ci, p_block_size, co, p_data_size, 0);

    vo.assign(co, co + p_data_size);

    free(co);

    v_output = move(vo);

    return true;
}

namespace vint_code
{

    std::vector<uint8_t> EncodeArray(const std::vector<uint32_t> &arr)
    {
        std::vector<uint8_t> buffer;

        buffer.reserve(arr.size() * 4);

        for (const auto &value : arr)
        {
            WriteVint(value, buffer);
        }

        return buffer;
    }

    std::vector<uint32_t> DecodeArray(std::vector<uint8_t> &buffer)
    {
        size_t size = buffer.size();
        std::vector<uint32_t> arr;
        arr.reserve(size / 4);
        size_t pos = 0;

        while (pos < size)
        {
            arr.push_back(ReadVint(buffer, pos));
        }

        return arr;
    }

    uint32_t ReadVint(std::vector<uint8_t> &buffer, size_t &pos)
    {
        if (buffer[pos] == '\0')
        {
            pos++;
            return 0;
        }
        uint8_t firstByte = buffer[pos++];
        uint8_t mask = 0x80;
        uint32_t value = firstByte & 0x7f;
        int shift = 7;
        while (firstByte & mask)
        {
            firstByte = buffer[pos++];
            // value = (value << 7) | (firstByte & 0x7f);
            value = ((firstByte & 0x7f) << shift) | value;
            shift += 7;
            // mask <<= 7;
        }

        return value;
    }

    size_t WriteVint(uint32_t value, std::vector<uint8_t> &buffer)
    {
        size_t size = 0;

        while (value > 0x7f)
        {
            buffer.push_back((value & 0x7f) | 0x80);
            value >>= 7;
            size++;
        }
        if (value)
            buffer.push_back(value & 0x7f);
        else
            buffer.push_back('\0');
        size++;

        return size;
    }
};

namespace zstd
{
    bool zstd_compress(const std::vector<uint8_t> &srcContent, std::vector<uint8_t> &cBuff)
    {
        size_t cSizeActual = 0;

        size_t cSize = ZSTD_compressBound(srcContent.size());

        cBuff.resize(cSize);

        cSizeActual = ZSTD_compress(cBuff.data(), cSize, srcContent.data(), srcContent.size(), 10);
        cBuff.resize(cSizeActual);

        return !ZSTD_isError(cSizeActual);
    }

    bool zstd_decompress(const std::vector<uint8_t> &cBuff, std::vector<uint8_t> &dBuff)
    {

        size_t dSizeActual = 0;

        size_t dSize = ZSTD_getDecompressedSize(cBuff.data(), cBuff.size());

        dBuff.resize(dSize);

        dSizeActual = ZSTD_decompress(dBuff.data(), dSize, cBuff.data(), cBuff.size());

        dBuff.resize(dSizeActual);

        return !ZSTD_isError(dSizeActual);
    }
};