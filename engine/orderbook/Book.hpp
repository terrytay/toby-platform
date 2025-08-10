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

    int best_idx[2] = {-1, -1}; // [BID], [ASK]

     OrderBook(int half_span, long long center_tick, size_t max_orders) : pool(max_orders) { // initialise pool before body of constructor runs
        for (int s = 0; s < 2; ++s) {
            side[s].center_tick = center_tick;
            side[s].half_span = half_span;
            side[s].levels.resize(2*half_span + 1);
        }
        loc.reserve(max_orders); // reduce rehash jitters
    }

    // TODO: Revisit this, as dropping prices too far away is not acceptable. Need to consider during high volatility sessions??
    inline void recenter(uint8_t s, int new_center_idx) {
    // Idx:  0   1   2   3   4   5   6   7   8
    // Px:  96  97  98  99 [100] 101 102 103 104
    // Best price drifts to 103 → that’s offset +3 ticks from center → shift = +3.
    // want to make index 4 = 103 now.

    Ladder& lad = side[s];
    const int shift = new_center_idx - lad.half_span;     // how far actual best moved from the middle
    if (shift == 0) return;

    const int W = (int)lad.levels.size();        // total window size

    // Build new vector and map old indices to new ones
    std::vector<Level> newLevels(W);
    for (int old = 0; old < W; ++old) {
        int newIdx = old - shift;
        if (newIdx >= 0 && newIdx < W) {
            newLevels[newIdx] = lad.levels[old];
        } else {
            // dropped out-of-window: nothing to copy (rare)
        }
    }
    lad.levels.swap(newLevels);

    // Update center tick to reflect the shift
    lad.center_tick += shift;

    // Fix best pointer if needed
    if (best_idx[s] != -1) {
        int newBest = best_idx[s] - shift;
        best_idx[s] = (newBest >= 0 && newBest < W) ? newBest : -1;
    }
}
    inline void ensure_index_in_range(uint8_t s, int& idx) {
        Ladder& lad = side[s];
        if (lad.in_range(idx)) return;

        // If price far outside, recenter around requested idx
        // Snap so requested idx ends up at half-span (middle)
        recenter(s, idx);   // this will adjust indices so old->new mapping is consistent
        // After recenter, recompute idx relative to new center
        if (!lad.in_range(idx)) {
            // If still out of range (extreme move), clamp
            idx = std::min(std::max(idx, 0), (int)lad.levels.size()-1);
        }
    }

    inline void on_level_become_nonempty(uint8_t s, int idx) {
        if (best_idx[s] == -1) {best_idx[s] = idx; return;}

        if (s == BID) {
            if (idx > best_idx[s]) best_idx[s] = idx;
        } else { // s == ASK
            if (idx < best_idx[s]) best_idx[s] = idx;
        }
    }

    inline void on_level_maybe_empty(uint8_t s, int idx) {
        Level& L = side[s].levels[idx];
        if (L.head != -1) return; // still has order(s) on this 
        if (best_idx[s] != idx) return; // not best anymore or wasn't best

        // advance to next best
        if (s == BID) {
            // move downwards until non-empty
            for (int i = idx - 1; i>=0; --i) {
                if (side[s].levels[i].head != -1) {
                    best_idx[s] = i; // found a non-empty level, set and return
                    return;
                }
            }
            best_idx[s] = -1; // none
        } else {
            for (int i = idx + 1; i < (int)side[s].levels.size(); ++i) {
                if (side[s].levels[i].head != -1) {
                    best_idx[s] = i;
                    return;
                }
            }
        }
    }
    
    inline void maybe_promote_best(uint8_t s, int idx) {
        // called after add() to see if this price improve best
        if (best_idx[s] == -1) {
            best_idx[s] = idx;
            return;
        }

        if (s == BID) {
            if (idx > best_idx[s]) best_idx[s] = idx;
        } else {
            if (idx < best_idx[s]) best_idx[s] = idx;
        }
    }


    /* Public API */
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
        if (prev == -1) {
            L.head = L.tail = n;
            on_level_become_nonempty(s, idx);
            maybe_promote_best(s, idx);
        }
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

        if (L.head == -1) on_level_maybe_empty(ol.side, ol.idx);
        return true;
    }
    
    // Best-of-book queries (O(1) if best_idx is set)
    bool best_bid(double& price_out, uint32_t& qty_out) const {
        int i = best_idx[BID];
        if (i < 0) return false;
        const Level& L = side[BID].levels[i];
        price_out = (side[BID].center_tick + i - side[BID].half_span) / 4.0; // ticks -> price
        qty_out = L.agg_qty;
        
        return true;
    }

    bool best_ask(double& price_out, uint32_t& qty_out) const {
        int i = best_idx[ASK];
        if (i < 0) return false;
        const Level& L = side[ASK].levels[i];
        price_out = (side[ASK].center_tick + i - side[ASK].half_span) / 4.0;
        qty_out = L.agg_qty;
        return true;
    }

    // Consume from best (market order impact). Side=BUY lifts ASK; Side=SELL hits BID.
    // Returns filled quantity.

    uint32_t trade_at_best(uint32_t taker_side, uint32_t want_qty) {
        uint8_t book_side = (taker_side == BID ? ASK : BID);
        uint32_t filled = 0;

        while (want_qty > 0) {
            int i = best_idx[book_side];
            if (i < 0) break; // no liquidity
            Level& L = side[book_side].levels[i];

            // pop or partial-fill from head
            int32_t n = L.head;
            if (n == -1) {
                on_level_maybe_empty(book_side, i);
                break;
            }

            OrderNode& node = pool.nodes[n];
            uint32_t take = (node.qty <=  want_qty) ? node.qty : want_qty;
            node.qty -= take;
            L.agg_qty -= take;
            want_qty -= take;
            filled += take;

            if (node.qty == 0) {
                L.head = node.next;
                if (L.head == -1) L.tail = -1;
                // remove from loc map (we need its id)
                auto it = loc.find(node.order_id);
                if (it != loc.end()) loc.erase(it);
                pool.release(n);    
                if (L.head == -1) on_level_maybe_empty(book_side, i);
            } else {
                // partial filled
                break;
            }
        }
        return filled;
    }
};