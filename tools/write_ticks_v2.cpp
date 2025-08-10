#include "../engine/tick/TickEventV2.hpp"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <deque>
#include <string>
#include <filesystem>

static inline double snap_to_tick(double px) {
    return std::round(px * 4.0) / 4.0;
}


int main() {
    std::string path = std::filesystem::absolute("../../data/mock_ticks_v2.bin").string();
    std::FILE* out = std::fopen(path.c_str(), "wb");
    if (!out) {
        std::perror("fopen failed");
        return 1;
    }

    const size_t TOTAL = 1'000'000;
    const double MID = 18000.00;
    const int SPAN = 80; // =/- 80 ticks
    uint64_t ts = 1'700'000'000'000'000'000ULL; 

    std::vector<TickEventV2> buffer;
    buffer.reserve(10000);

    std::deque<uint64_t> recent_ids;
    const size_t RECENT_MAX = 20000;
    uint64_t next_oid = 1;

    for (size_t i = 0; i < TOTAL; ++i) {
        TickEventV2 t{};
        t.timestamp_ns = ts; ts += 1000; // +1us
        t.size = 1 + (std::rand() % 5); // qty 1 to 5

        int dice = std::rand() % 100;
        
        if (dice < 55) { // ~55% limit orders
            int off = (std::rand() % (2*SPAN+1)) - SPAN;
            t.price = snap_to_tick(MID + off*0.25);
            t.type = (std::rand() & 1) ? 1 : 2; // 1=BID, 2=ASK
            t.order_id = next_oid++;
            recent_ids.push_back(t.order_id);
            if (recent_ids.size() > RECENT_MAX) recent_ids.pop_front();
        } else if (dice < 75) { // ~25% market orders
            bool sell = std::rand() & 1;
            t.type = sell ? 3 : 0; // 3=MARKETSELL, 0=MARKETBUY
            t.price = MID; // ignored by router for demo
            t.order_id = 0; // TODO: need to still keep track of order id?? for logging??
        } else {
            if (!recent_ids.empty()) {
                t.type = 4; // cancel
                t.price = MID; // not used
                size_t j = std::rand() % recent_ids.size();
                t.order_id = recent_ids[j];

                // swap and pop
                recent_ids[j] = recent_ids.back();
                recent_ids.pop_back();
            } else {
                // fallback to add if nth to cancel
                int off = (std::rand() % (2*SPAN+1)) - SPAN;
                t.price = snap_to_tick(MID + off*0.25);
                t.type = (std::rand() & 1) ? 1 : 2; // 1=BID, 2=ASK
                t.order_id = next_oid++;
                recent_ids.push_back(t.order_id);
                // if (recent_ids.size() > RECENT_MAX) recent_ids.pop_front(); // not needed as recent_ids is already empty
            }
        }

        buffer.push_back(t);
        if (buffer.size() == 10000) {
            std::fwrite(buffer.data(), sizeof(TickEventV2), buffer.size(), out);
            buffer.clear();
        }
    }
    
    // remainings
    if (!buffer.empty()) {
        std::fwrite(buffer.data(), sizeof(TickEventV2), buffer.size(), out);
    }

    std::fclose(out);
    std::puts("Done writing ticks!");
    return 0;
}