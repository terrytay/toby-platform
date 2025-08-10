#pragma once
#include <cstdint>
#include "../tick/TickEvent.hpp"
#include "../orderbook/Book.hpp"

struct MarketDataRouter {
    OrderBook& ob;
    uint64_t next_oid = 1; // synthetic oid

    explicit MarketDataRouter(OrderBook& book) : ob(book) {}

    inline void on_tick(const TickEvent& t) {
        // std::cout << "Price: " << t.price << "\t" << " Size: " << t.size << "\t" << " Type: " << static_cast<int>(t.type) << std::endl;
        switch (static_cast<int>(t.type)) {
            case 1:
                ob.add(BID, t.price, t.size, next_oid++);
                break;
            case 2:
                ob.add(ASK, t.price, t.size, next_oid++);
                break;
            case 0:
                ob.trade_at_best(BID, t.size);
                break;
            case 3:
                ob.trade_at_best(ASK, t.size);
                break;
            default:
                break;
        }
    }
};