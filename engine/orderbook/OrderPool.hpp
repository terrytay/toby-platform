#pragma once
#include <vector>
#include <cstdint>
#include <iostream>

struct OrderNode {
    uint64_t order_id;
    uint32_t qty;
    int32_t next = -1;
};

struct OrderPool {
    std::vector<OrderNode> nodes;
    int32_t free_head = -1;

    explicit OrderPool(size_t capacity) { init(capacity); }

    void init(size_t capacity) {
        nodes.resize(capacity);
        free_head = -1;
        for (int32_t i = (int32_t)capacity - 1; i>=0; --i) {
            nodes[i].next = free_head;
            free_head = i;
        }
    }

    int32_t alloc() {
        int32_t id = free_head; // assume capacity is sufficient for now
        free_head = nodes[id].next;
        nodes[id].next = -1;
        return id;
    }

    void release(int32_t id) {
        nodes[id].next = free_head;
        free_head = id;
    }

    // void printNodes() {
    //     for (auto &n : nodes)
    //         std::cout << n.order_id << '\t';
    //     std::cout << "\n";
    // }
};