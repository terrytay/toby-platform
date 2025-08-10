#include "TickReplayerV2.hpp"
#include <cstdio>
#include <functional>
#include <stdexcept>

TickReplayerV2::TickReplayerV2(const char* filepath) {
    file = std::fopen(filepath, "rb");
    if (!file) throw std::runtime_error("Failed to open tick file V2");
}

TickReplayerV2::~TickReplayerV2() {
    if (file) std::fclose(file);
}

void TickReplayerV2::replay(const std::function<void(const TickEventV2&)>& onTick) {
    TickEventV2 tick;
    while (std::fread(&tick, sizeof(TickEventV2), 1, file) == 1) {
        onTick(tick);
    }
}