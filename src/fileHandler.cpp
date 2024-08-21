
#include "fileHandler.h"

bool File_Handle_2::Open(const string &temp_file2_fname)
{
    lock_guard<mutex> lck(mtx);

    if (f)
        fclose(f);

    m_streams.clear();
    // file_name = _file_name+".temp_gsc";

    f = fopen(temp_file2_fname.c_str(), input_mode ? "rb" : "wb");

    if (!f)
    {
        std::cerr << "Can't Open: " << temp_file2_fname << " failed" << endl;
        return false;
    }
    setvbuf(f, nullptr, _IOFBF, 64 << 20);
    if (input_mode)
        deserialize();

    f_offset = 0;

    return true;
}

bool File_Handle_2::Close()
{
    lock_guard<mutex> lck(mtx);

    if (!f)
        return false;

    if (input_mode)
    {
        fclose(f);
        f = nullptr;
    }
    else
    {
        serialize();
        fclose(f);
        f = nullptr;
    }

    return true;
}

bool File_Handle_2::deserialize()
{
}

bool File_Handle_2::serialize()
{
}

int File_Handle_2::RegisterStream(string stream_name)
{
    lock_guard<mutex> lck(mtx);

    int id = (int)m_streams.size();

    m_streams[id] = stream_t();
    m_streams[id].cur_id = 0;
    m_streams[id].stream_name = stream_name;

    return id;
}

void File_Handle_2::AddParamsPart(int stream_id, vector<uint8_t> &v_data)
{
    lock_guard<mutex> lck(mtx);

    m_streams[stream_id].parts.emplace_back(f_offset, v_data.size());
    ;
    if (v_data.size())
        fwrite(v_data.data(), 1, v_data.size(), f);

    f_offset += v_data.size();
}

bool File_Handle_2::AddPartComplete(int stream_id, int part_id, vector<uint8_t> &v_data)
{
    lock_guard<mutex> lck(mtx);
    m_streams[stream_id].parts[part_id] = part_t(f_offset, v_data.size());

    if (v_data.size())
        fwrite(v_data.data(), 1, v_data.size(), f);

    f_offset += v_data.size();

    return true;
}