#pragma once

#include "TickEventV2.hpp"
#include <functional>
#include <cstdio>

class TickReplayerV2 {
public:
    explicit TickReplayerV2(const char* filepath);
    ~TickReplayerV2();
    void replay(const std::function<void(const TickEventV2&)>& onTick);

private:
    std::FILE* file;
};