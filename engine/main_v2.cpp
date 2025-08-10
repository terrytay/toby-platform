#include <iostream>
#include <filesystem>

#include "orderbook/Book.hpp"
#include "tick/TickReplayerV2.hpp"
#include "market/MarketDataRouterV2.hpp"

int main() {
    long long center_tick = price_to_ticks(18000.00);
    OrderBook ob(10'000, center_tick, 10'000'000);
    const size_t SAMPLE_SIZE = 200'000;
    MarketDataRouterV2 router(ob);

    std::string path = std::filesystem::absolute("../data/mock_ticks_v2.bin").string();
    TickReplayerV2 replayer(path.c_str());

    size_t count = 0;
    std::cout << std::fixed << std::setprecision(2);
    std::printf("Sampling every %ld\n", SAMPLE_SIZE);

    replayer.replay([&](const TickEventV2& tick) {
        router.on_tick(tick);

        if (++count % SAMPLE_SIZE == 0) {
            double bp, ap; uint32_t bq, aq;
            if (ob.best_bid(bp, bq)) {
                std::cout << "[TOP] bid " << bp << " x " << bq;
            } else {
                std::cout << "[TOP] bid (none)";
            }

            if (ob.best_ask(ap, aq)) {
                std::cout << " | ask " << ap << " x " << aq << "\n";
            } else {
                std::cout << " | ask (none)\n";
            }
        }
    });
    
    std::cout << "Replayed ticks: " << int(count) << "\n";
    return 0;
}