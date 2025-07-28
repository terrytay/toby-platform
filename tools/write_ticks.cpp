#include "../engine/tick/TickEvent.hpp"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <filesystem>

int main() {
    std::string path = std::filesystem::absolute("../../data/mock_ticks.bin").string();
    std::FILE* out = std::fopen(path.c_str(), "wb");
    if (!out) {
        std::perror("fopen failed");
        return 1;
    }

    std::srand(std::time(nullptr));

    for (int i = 0; i < 100000; ++i) {
        TickEvent tick;
        tick.timestamp_ns = 1650000000000000000ULL + i * 1000; // fake ns ts
        tick.price = 4200.0 + (std::rand() % 100) / 10.0;
        tick.size = (std::rand() % 5) + 1;
        tick.type = (std::rand() % 3);

        std::fwrite(&tick, sizeof(TickEvent), 1, out);
    }

    std::fclose(out);
    printf("Wrote 100,000 fake ticks to data/mock_ticks.bin\n");
    return 0;
}