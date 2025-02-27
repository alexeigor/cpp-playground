#include <benchmark/benchmark.h>
#include <atomic>
#include <thread>
#include <vector>
#include <iostream>

// Number of increments per thread
constexpr int NUM_INCREMENTS = 10000;
constexpr int NUM_THREADS = 4;

// False Sharing Case: Contiguous atomic variables
struct FalseSharing {
    std::atomic<int> a, b, c, d;
} sharedFalse;

// Avoid false sharing using padding
struct alignas(64) PaddedAtomic {
    std::atomic<int> value;
    char padding[64 - sizeof(std::atomic<int>)]; // Ensure separate cache lines
};

struct TrueSharing {
    PaddedAtomic a, b, c, d;
} sharedTrue;

// Work function
void work(std::atomic<int>& var) {
    for (int i = 0; i < NUM_INCREMENTS; ++i) {
        var.fetch_add(1, std::memory_order_relaxed);
    }
}

// Benchmark false sharing scenario
static void BM_FalseSharing(benchmark::State& state) {
    for (auto _ : state) {
        sharedFalse.a = 0;
        sharedFalse.b = 0;
        sharedFalse.c = 0;
        sharedFalse.d = 0;

        std::thread t1(work, std::ref(sharedFalse.a));
        std::thread t2(work, std::ref(sharedFalse.b));
        std::thread t3(work, std::ref(sharedFalse.c));
        std::thread t4(work, std::ref(sharedFalse.d));

        t1.join();
        t2.join();
        t3.join();
        t4.join();
    }
}
BENCHMARK(BM_FalseSharing);

// Benchmark true sharing scenario (avoiding false sharing)
static void BM_TrueSharing(benchmark::State& state) {
    for (auto _ : state) {
        sharedTrue.a.value = 0;
        sharedTrue.b.value = 0;
        sharedTrue.c.value = 0;
        sharedTrue.d.value = 0;

        std::thread t1(work, std::ref(sharedTrue.a.value));
        std::thread t2(work, std::ref(sharedTrue.b.value));
        std::thread t3(work, std::ref(sharedTrue.c.value));
        std::thread t4(work, std::ref(sharedTrue.d.value));

        t1.join();
        t2.join();
        t3.join();
        t4.join();
    }
}
BENCHMARK(BM_TrueSharing);

BENCHMARK_MAIN();