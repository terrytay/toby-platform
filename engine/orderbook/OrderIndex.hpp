#pragma once

#include <unordered_map>
#include "OrderLoc.hpp"

template<typename K=int64_t, typename V=OrderLoc> using OrderIndex = std::unordered_map<K, V>; // absl::flat_hash_map impl maybe