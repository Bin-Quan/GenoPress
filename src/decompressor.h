
#pragma once

#include "argumentParser.h"

class Decompressor
{
public:
    Decompressor();
    Decompressor(GSCParams &params);
    ~Decompressor();

    bool DecompressProcess();
};
