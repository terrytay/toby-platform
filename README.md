# High-Performance Trading System (C++)

## Author’s Note

I am a software engineer and trader building a modular, low-latency C++ trading engine from the ground up. This project is designed to meet HFT-grade requirements while staying maintainable and extensible. Every component — from data ingestion to strategy execution — is planned with nanosecond-level performance, cache-efficiency, and deterministic behavior in mind. This README documents the architecture, build milestones, and roadmap.

---

## 1. System Overview

The system is a **market data processing and execution pipeline** optimized for ultra-low latency:

- **Order Book Core** with O(1) best bid/ask lookup
- **Order Pool & Free List** to avoid runtime allocations
- **Lock-Free SPSC Ring Buffer** for inter-thread communication
- **Tick Replayer** with memory-mapped files for historical playback
- Modular design for plugging in **execution strategies** without affecting data path

Target markets: futures & equities at tick resolution, with the ability to scale to live feed ingestion.

---

## 2. Architecture

```
[ Market Data Source / Replay File ]
                 │
        Tick Replayer (Producer)
                 │  (SPSC Ring)
                 ▼
        Order Book Engine (Consumer)
                 │
          Strategy Module
                 │
         Execution Gateway
```

---

## 3. Build Plan (Milestones)

| Stage | Feature                                           | Status         |
| ----- | ------------------------------------------------- | -------------- |
| **1** | Core order book structure with FIFO price levels  | ✅ Done        |
| **2** | Best bid/ask pointers for O(1) top-of-book access | ✅ Done        |
| **3** | Order Pool + Free List allocator                  | ✅ Done        |
| **4** | Lock-Free SPSC ring buffer for producer→consumer  | ✅ Done        |
| **5** | Tick Replayer (mmap + fast & real-time modes)     | ⏳ In progress |
| **6** | Strategy module interface                         | ⏳ Upcoming    |
| **7** | Execution gateway stub                            | ⏳ Upcoming    |
| **8** | Live market data feed handler                     | ⏳ Upcoming    |
| **9** | Performance tuning & profiling                    | ⏳ Ongoing     |

---

## 4. Design Decisions

**Order Book**

- Price levels stored in `unordered_map` initially for simplicity
- Will migrate to `absl::flat_hash_map` or vector-indexed storage for cache-locality after core build is stable
- FIFO order queues at each price level for correct matching semantics

**Best Bid/Ask Pointers**

- Maintained on each order add/remove to avoid scanning price levels
- Allows O(1) retrieval for top-of-book strategies

**Memory Management**

- Pool allocator with free list to reuse order objects
- Eliminates runtime `malloc`/`free` jitter

**Inter-Thread Communication**

- Lock-free SPSC ring with two atomic pointers (head/tail)
- Zero-copy reserve/commit API for direct in-slot writes

**Tick Replay**

- Binary file format (header + fixed-width events)
- Memory-mapped into process space for minimal I/O overhead
- Supports burst replay and time-paced replay

---

## 5. Performance Targets

- **Latency**: <10µs producer→consumer hop at p99 in real-time mode
- **Throughput**: ≥20M events/sec in fast mode on modern x86 core
- **Allocation profile**: zero allocations in steady-state
- **Cache usage**: all hot paths L1-friendly, minimal false sharing

---

## 6. Roadmap

**Short-Term**

- Complete Tick Replayer integration
- Add Strategy Module with simple market-making logic
- Add Execution Gateway stub

**Mid-Term**

- Swap `unordered_map` with cache-optimized container
- Introduce parallel feed handling with multiple SPSC rings
- Add risk management hooks

**Long-Term**

- Integrate with live market data APIs
- Multi-venue, multi-asset routing
- Full test harness with latency histograms & replay verification

---

## 7. How to Run (current dev state)

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
./hft_engine --file ../data/sample.l3bin --mode fast
```

Options:

- `--mode fast` — replay as fast as possible
- `--mode realtime` — pace by original tick timestamps

---

## 8. Notes for Recruiters

This project is an ongoing build intended to demonstrate:

- Strong systems programming skills in C++
- Deep understanding of low-latency architecture
- Ability to design, profile, and evolve complex systems over time
