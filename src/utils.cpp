
#include "utils.h"

void append_str(vector<uint8_t> &v_comp, const string &x){
    size_t old_size = v_comp.size();
    size_t new_size = old_size + x.size() + 1;
    v_comp.reserve(new_size);
    copy(x.begin(), x.end(), std::back_inserter(v_comp));
    v_comp.emplace_back('\0'); 
}

void read_str(const vector<uint8_t>& v_comp, size_t& pos, string& x){
	x.clear();
    auto null_pos = find(v_comp.begin() + pos, v_comp.end(),'\0');
    x.assign(v_comp.begin() + pos, null_pos);
    pos = null_pos - v_comp.begin() + 1;
}

