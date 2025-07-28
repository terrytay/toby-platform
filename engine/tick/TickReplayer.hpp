#pragma once

#include "TickEvent.hpp"
#include <functional>
#include <cstdio>

class TickReplayer {
public:
    explicit TickReplayer(const char* filepath);
    ~TickReplayer();
    void replay(const std::function<void(const TickEvent&)>& onTick);

private:
    std::FILE* file;
};