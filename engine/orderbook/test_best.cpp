#include "Book.hpp"
#include <iostream>
#include <cassert>

int main() {
    long long center_tick = price_to_ticks(18000.00);
    OrderBook ob(/*half_span=*/10, center_tick, /*max_orders=*/1000);

    // Add some bids/asks
    ob.add(BID, 17999.75, 4, 1001);  // bid level below center
    ob.add(BID, 18000.00, 5, 1002);  // better bid
    ob.add(ASK, 18000.25, 3, 2001);  // best ask
    ob.add(ASK, 18000.50, 7, 2002);

    double px; uint32_t qty;

    bool ok = ob.best_bid(px, qty);
    assert(ok && px == 18000.00 && qty == 5);

    ok = ob.best_ask(px, qty);
    assert(ok && px == 18000.25 && qty == 3);

    // Buyer takes 2 at best ask (should partially fill 2001 from 3 -> 1)
    uint32_t filled = ob.trade_at_best(BID, 2);
    assert(filled == 2);
    ok = ob.best_ask(px, qty);
    assert(ok && px == 18000.25 && qty == 1);

    // Buyer takes 5 more (consumes remaining 1 @18000.25, then 4 from 18000.50)
    filled = ob.trade_at_best(BID, 5);
    assert(filled == 5);
    ok = ob.best_ask(px, qty);
    assert(ok && px == 18000.50 && qty == 3); // 7-4=3 remaining

    std::cout << "OK best/ask and trade-at-best\n";

    filled = ob.trade_at_best(BID, 4);
    ok = ob.best_ask(px, qty);
    assert(ok && filled == 3  && px == 18000.50 && qty == 0);

    std::cout << "OK liquidity\n";

    return 0;
}
