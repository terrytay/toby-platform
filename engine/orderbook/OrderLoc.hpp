#pragma once

#include <cstdint>

struct OrderLoc {
    uint8_t side; // 0=bid, 1=ask
    int idx; // ladder index
    int32_t node; // node in pool
    int32_t prev; // prev node in FIFO (-1 if head)
};