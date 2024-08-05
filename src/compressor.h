
#pragma once

#include "argumentParser.h"


class Compressor
{
private:
    /* data */
public:
    Compressor();
    Compressor(GSCParams& params);
    ~Compressor();

    bool CompressProcess();
};

