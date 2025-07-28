cmake_minimum_required(VERSION 3.15)
project(TobyPlatform LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")

add_subdirectory(engine/tick)

add_executable(hft_main engine/main.cpp)
target_link_libraries(hft_main PRIVATE tick)
