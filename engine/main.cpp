#include <iostream>
#include <filesystem>

#include "orderbook/Book.hpp"
#include "tick/TickReplayer.hpp"
#include "market/MarketDataRouter.hpp"

int main() {
    long long center_tick = price_to_ticks(4200.00);
    OrderBook ob(2000, center_tick, 10'000'000);
    MarketDataRouter router(ob);

    std::string path = std::filesystem::absolute("../data/mock_ticks.bin").string();
    TickReplayer replayer(path.c_str());

    size_t count = 0;
    replayer.replay([&](const TickEvent& tick) {
        router.on_tick(tick);

        if (++count % 1'000'00 == 0) { // every 100k ticks, print top-of-book
            double px; uint32_t qty;
            if (ob.best_bid(px, qty)) {
                std::cout << "[TOP] bid " << px << " x " << qty;
            } else {
                std::cout << "[TOP] bid (none)";
            }
            if (ob.best_ask(px, qty)) {
                std::cout << " | ask " << px << " x " << qty << "\n";
            } else {
                std::cout << " | ask (none)\n";
            }
        }
    });
     std::cout << "Replayed ticks: " << count << "\n";
    double px; uint32_t qty;
    if (ob.best_bid(px, qty)) std::cout << "Final best bid: " << px << " x " << qty << "\n";
    if (ob.best_ask(px, qty)) std::cout << "Final best ask: " << px << " x " << qty << "\n";
    return 0;
}