#pragma once
#include <cstdint>

#pragma pack(push,1)
struct TickEvent {
    uint64_t timestamp_ns;
    double price;
    uint32_t size;
    uint8_t type; // 0 = trade, 1 = bid, 2 = ask
};
#pragma pack(pop)