add_library(tick STATIC
    TickReplayer.cpp
)

target_include_directories(tick PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})