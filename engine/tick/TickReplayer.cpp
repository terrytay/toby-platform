#include "TickReplayer.hpp"
#include <cstdio>
#include <functional>
#include <cassert>

TickReplayer::TickReplayer(const char* filepath) {
    file = std::fopen(filepath, "rb");
    assert(file && "Failed to open tick data file");
}

TickReplayer::~TickReplayer() {
    if (file) std::fclose(file);
}

void TickReplayer::replay(const std::function<void(const TickEvent&)>& onTick) {
    TickEvent tick;
    while (std::fread(&tick, sizeof(TickEvent), 1, file) == 1) {
        onTick(tick);
    }
}