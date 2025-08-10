cmake_minimum_required(VERSION 3.15)
project(TobyPlatform LANGUAGES CXX)

# Compiler & flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# High perf flags
add_compile_options(-O3 -march=native)

# -------- Libraries --------

# 1) tick: static library for TickReplayer
# add_library(tick STATIC
#     engine/tick/TickReplayer.cpp
# )
add_library(tickv2 STATIC
    engine/tick/TickReplayerV2.cpp
)

# target_include_directories(tick PUBLIC
#     engine            # so consumers can #include "tick/TickReplayer.hpp" & "tick/TickEvent.hpp"
# )
target_include_directories(tickv2 PUBLIC
    engine            # so consumers can #include "tick/TickReplayer.hpp" & "tick/TickEvent.hpp"
)

# 2) orderbook: header-only interface (all order book headers)
add_library(orderbook INTERFACE)
target_include_directories(orderbook INTERFACE
    engine            # so consumers can #include "orderbook/Book.hpp" etc.
)

# -------- Executables --------

# Main app: TickReplayer -> MarketDataRouter -> OrderBook
# add_executable(hft_main
#     engine/main.cpp
# )
# target_link_libraries(hft_main PRIVATE tick orderbook)
# target_include_directories(hft_main PRIVATE engine)

add_executable(hft_main_v2
    engine/main_v2.cpp
)
target_link_libraries(hft_main_v2 PRIVATE tickv2 orderbook)
target_include_directories(hft_main_v2 PRIVATE engine)



# Unit tests / demos
# add_executable(test_min engine/orderbook/test_min.cpp)
# target_link_libraries(test_min PRIVATE orderbook)
# target_include_directories(test_min PRIVATE engine)

# add_executable(test_best engine/orderbook/test_best.cpp)
# target_link_libraries(test_best PRIVATE orderbook)
# target_include_directories(test_best PRIVATE engine)
