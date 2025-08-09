#pragma once
#include <cmath>

static inline long long price_to_ticks(double px) {
    return (long long) llround(px * 4.0); // NQ tick size = 0.25 => 4 ticks per handle
}