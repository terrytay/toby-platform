#include "tick/TickReplayer.hpp"
#include <iostream>
#include <filesystem>

int main() {
    std::string path = std::filesystem::absolute("../data/mock_ticks.bin").string();
    TickReplayer replayer(path.c_str());

    size_t count = 0;
    replayer.replay([&](const TickEvent& tick) {
        if (++count <= 5) {
            std::cout << "Tick: " << count
                      << " | ts: " << tick.timestamp_ns
                      << " | price: " << tick.price
                      << " | size: " << tick.size
                      << " | type: " << static_cast<int>(tick.type)
                      << "\n";
        }
    });

    std::cout << "\nTotal ticks replayed: " << count << "\n";
    return 0;
}