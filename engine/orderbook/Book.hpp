#pragma once
#include <vector>
#include <cstdint>
#include "../common/PriceTicks.hpp"
#include "Level.hpp"
#include "OrderPool.hpp"
#include "OrderIndex.hpp"

enum Side : uint8_t {BID = 0, ASK = 1};

struct Ladder {
    long long center_tick;
    int half_span;
    std::vector<Level> levels;

    inline int idx_from_tick(long long t) const {
        return int(t - center_tick) + half_span;
    }

    inline bool in_range(int idx) const {
        return idx >= 0 && idx < (int)levels.size();
    }
};

struct OrderBook {
    Ladder side[2]; // [BID], [ASK]
    OrderPool pool;
    OrderIndex<> loc; // order_id -> {side, idx, node, prev}

    OrderBook(int half_span, long long center_tick, size_t max_orders) : pool(max_orders) { // initialise pool before body of constructor runs
        for (int s = 0; s < 2; ++s) {
            side[s].center_tick = center_tick;
            side[s].half_span = half_span;
            side[s].levels.resize(2*half_span + 1);
        }
        loc.reserve(max_orders); // reduce rehash jitters
    }

    // TODO: real recenter; for now assuming in range
    inline void ensure_index_in_range(uint8_t s, int& idx) {
        (void)s; (void)idx;
    }

    void add(uint8_t s, double price, uint32_t qty, uint64_t oid) {
        long long t = price_to_ticks(price);
        int idx = side[s].idx_from_tick(t);
        ensure_index_in_range(s, idx);
        Level& L = side[s].levels[idx];

        int32_t n = pool.alloc();
        OrderNode& node = pool.nodes[n];
        node.order_id = oid;
        node.qty = qty;
        node.next = -1;

        int32_t prev = L.tail;
        if (prev == -1) L.head = L.tail = n;
        else { pool.nodes[prev].next = n; L.tail = n;}

        L.agg_qty += qty;

        loc[oid] = OrderLoc{s, idx, n, prev};
    }

    bool cancel(uint64_t oid) {
        auto it = loc.find(oid);
        if (it == loc.end()) return false;

        OrderLoc ol = it->second;
        Level& L = side[ol.side].levels[ol.idx];
        OrderNode& cur = pool.nodes[ol.node];
        int32_t next = cur.next;
        
        if (ol.prev == - 1) L.head = next;
        else pool.nodes[ol.prev].next = next;
        if (L.tail == ol.node) L.tail = ol.prev;
        
        if (cur.qty <= L.agg_qty) L.agg_qty -=  cur.qty;
        else L.agg_qty = 0;

        pool.release(ol.node);
        loc.erase(it);
        return true;
    }
};