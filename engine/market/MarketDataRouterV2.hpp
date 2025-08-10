#pragma once
#include <cstdint>
#include "../tick/TickEventV2.hpp"
#include "../orderbook/Book.hpp"

struct MarketDataRouterV2 {
    OrderBook& ob;

    explicit MarketDataRouterV2(OrderBook& book) : ob(book) {}

    inline void on_tick(const TickEventV2& t) {
        switch (t.type) {
            case 1:
                ob.add(BID, t.price, t.size, t.order_id);
                break;
            case 2:
                ob.add(ASK, t.price, t.size, t.order_id);
                break;
            case 0:
                ob.trade_at_best(BID, t.size);
                break;
            case 3:
                ob.trade_at_best(ASK, t.size);
                break;
            case 4:
                ob.cancel(t.order_id);
                break;
            default:
                break;
        }
    }
};