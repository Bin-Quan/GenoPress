/*
1. Define important constants and enumerations required for program execution.
*/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits>
#include <string>
#include <cstddef>


const uint32_t p_bsc_features = 1u;

const float min_float = std::numeric_limits<float>::lowest();
const float max_float = std::numeric_limits<float>::max();

#define CHUNK_SIZE1 1<<16
#define CHUNK_SIZE2 1<<15
#define CHUNK_SIZE3 1<<14

const size_t no_variants_in_buf = 8192u;

const uint32_t max_buffer_size = 8 << 20;  // CompressionReader::InitVarinats
