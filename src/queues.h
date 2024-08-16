#pragma once

#include "variant.h"

#include <functional>
#include <condition_variable>
#include <mutex>
#include <queue>

class GtBlockQueue
{
public:
    explicit GtBlockQueue(size_t _capacity)
        : flag(false), capacity(_capacity) {}

    bool Pop(int &id_block, unsigned char *&data, size_t &num_rows, std::vector<variant_desc_t> &v_vcf_data_compress)
    {
        std::unique_ptr<genotype_block_t> block;
        {
            std::unique_lock<std::mutex> lck(m_mutex);
            cv_pop.wait(lck, [this]
                        { return !g_blocks.empty() || flag; });

            if (flag && g_blocks.empty())
                return false;

            block = std::move(g_blocks.front());
            g_blocks.pop_front();
        }

        cv_push.notify_one();

        id_block = block->block_id;
        data = block->data;
        num_rows = block->num_rows;
        v_vcf_data_compress = std::move(block->v_vcf_data_compress);

        return true;
    }

private:
    struct genotype_block_t
    {
        int block_id;
        unsigned char *data;
        size_t num_rows;
        std::vector<variant_desc_t> v_vcf_data_compress;

        genotype_block_t(int _block_id, unsigned char *_data, size_t _num_rows, std::vector<variant_desc_t> &&_v_vcf_data_compress)
            : block_id(_block_id), data(_data), num_rows(_num_rows), v_vcf_data_compress(std::move(_v_vcf_data_compress)) {}
    };

    bool flag;
    size_t capacity;

    std::condition_variable cv_pop, cv_push;
    std::mutex m_mutex;
    std::deque<std::unique_ptr<genotype_block_t>> g_blocks; // 双端队列（double-ended queue）
};

template <typename DataType>
class VarBlockQueue
{
public:
    VarBlockQueue() : flag(false), capacity(0) {}
    VarBlockQueue(size_t _capacity) : flag(false), capacity(_capacity) {}
    ~VarBlockQueue() {}

    bool Pop(uint32_t &id_block, DataType &data)
    {
        unique_lock<std::mutex> lck(m_mutex);
        cv_pop.wait(lck, [this]
                    { return !var_blocks.empty() || flag; });
        if (flag && var_blocks.empty())
            return false;
        auto block = std::move(var_blocks.front());
        var_blocks.pop();
        id_block = block.block_id;

        data = std::move(block.data);

        cv_push.notify_all();

        return true;
    }

private:
    bool flag;
    size_t capacity;
};

template <typename PartType>
class PartQueue
{
public:
    PartQueue() : flag(false), capacity(0) {}
    PartQueue(size_t _capacity) : flag(false), capacity(_capacity) {}
    ~PartQueue() {}

    // CompressorProcess function
    template <typename S>
    bool Pop(PartType &data, const std::function<bool(S &item)> fo)
    {
        // unique_lock<std::mutex> lck(m_mutex);
        // cv_pop.wait(lck, [this]
        //             { return !part_queue.empty() || flag; });
        // if (flag && part_queue.empty())
        //     return false;
        // auto cur_part = part_queue.begin();
        // for (; cur_part != part_queue.end(); ++cur_part)
        //     if (fo(*cur_part))
        //         break;
        // if (cur_part == part_queue.end())
        //     cur_part = part_queue.begin();
        // data = move(*cur_part);
        // part_queue.erase(cur_part);
        // // auto cur_part = std::move(part_queue.front());
        // // part_queue.pop();

        // // data = std::move(cur_part.data);

        // cv_push.notify_all();

        return true;
    }

private:
    bool flag;
    size_t capacity;
    // mutex m_mutex;
    // condition_variable cv_pop, cv_push;
    // list<PartType> part_queue;
};