#include "../engine/tick/TickEvent.hpp"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <filesystem>
#include <vector>

TickEvent generateTick(size_t i) {
    TickEvent tick;
    tick.timestamp_ns = 1650000000000000000ULL + i * 1000;
    tick.price = 4200.0 + (std::rand() % 100);
    tick.size = (std::rand() % 5) + 1;
    tick.type = (std::rand() % 4);
    return tick;
}

int main() {
    std::string path = std::filesystem::absolute("../../data/mock_ticks.bin").string();
    std::FILE* out = std::fopen(path.c_str(), "wb");
    if (!out) {
        std::perror("fopen failed");
        return 1;
    }

    const size_t total = 10'000'000;
    std::vector<TickEvent> buffer;
    buffer.reserve(10000);

    for (size_t i = 0; i < total; ++i) {
        buffer.push_back(generateTick(i));

        if (buffer.size() == 10000) {
            std::fwrite(buffer.data(), sizeof(TickEvent), buffer.size(), out);
            buffer.clear();
        }

        if (i % 1000000 == 0) {
            std::printf("Progress: %lu / %lu\n", i, total);
        }
    }

    if (!buffer.empty()) {
        std::fwrite(buffer.data(), sizeof(TickEvent), buffer.size(), out);
    } 

    std::fclose(out);
    std::puts("Done writing 10 million ticks!");
    return 0;
}