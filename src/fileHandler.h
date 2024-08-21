#pragma once

#include <iostream>
#include <cstdint>
#include <mutex>
#include <map>
#include <vector>

using namespace std;

class File_Handle_2
{
private:
    FILE *f;
    bool input_mode;

    // Open function
    struct part_t
    {
        size_t offset;
        size_t size;

        part_t() : offset(0), size(0) {};
        part_t(size_t _offset, size_t _size) : offset(_offset), size(_size) {};
    };
    typedef struct
    {
        string stream_name;
        size_t cur_id;
        vector<part_t> parts;
    } stream_t;
    mutex mtx;
    map<int, stream_t> m_streams;
	size_t f_offset;


public:
    File_Handle_2(bool _input_mode)
    {
        f = nullptr;
        input_mode = _input_mode;
    }

    ~File_Handle_2()
    {
        if (f)
        {
            Close();
        }
    }

    // OpenTempFile function
    bool Open(const string &temp_file2_fname);
    bool Close();

    // Open function
    bool deserialize();   // 只声明未实现

    // Close function
    bool serialize();  // 只声明未实现

    int RegisterStream(string stream_name);

    void AddParamsPart(int stream_id,vector<uint8_t> & v_data);
	bool AddPartComplete(int stream_id, int part_id, vector<uint8_t>& v_data);

};