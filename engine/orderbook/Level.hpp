#pragma once
#include <cstdint>

struct Level {
    uint32_t agg_qty = 0;
    int32_t head = -1;
    int32_t tail = -1;
};