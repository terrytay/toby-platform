#pragma once
#include <cstdint>

#pragma pack(push,1)
struct TickEventV2 {
    uint64_t timestamp_ns;
    double price;
    uint32_t size;
    uint8_t type; // 0=MARKETBUY, 1=BID 2=ASK, 3=MARKETSELL, 4=CANCEL
    uint64_t order_id; // used for add, cancel
};
#pragma pack(pop)
static_assert(sizeof(TickEventV2) == 29, "TickEventV2 must be 29 bytes");
// total (64 + 64 + 32 + 8 + 64) / 8 = 29 bytes