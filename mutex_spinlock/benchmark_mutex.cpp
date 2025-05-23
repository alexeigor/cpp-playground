#include <benchmark/benchmark.h>
#include <atomic>
#include <mutex>
#include <vector>
#include <thread>
#include <chrono>

// Custom Spinlock Mutex
class SpinlockMutex {
private:
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {
            // Busy wait
        }
    }
    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

// Shared resource
int shared_value = 0;

// Mutex objects
std::mutex std_mutex;
SpinlockMutex spinlock_mutex;

// Function to measure real work time
template <typename MutexType>
void benchmark_function(benchmark::State& state, MutexType& mutex) {
    int iterations = state.range(0);

    for (auto _ : state) {
        shared_value = 0; // Reset shared value
        std::vector<std::thread> threads;

        for (int i = 0; i < state.threads(); ++i) {
            threads.emplace_back([iterations, &mutex]() {
                for (int j = 0; j < iterations; ++j) {
                    mutex.lock();
                    shared_value = shared_value ^ j;  // Simulated computation
                    mutex.unlock();
                }
            });
        }

        for (auto& t : threads) {
            t.join();
        }
    }
}

// Benchmark for std::mutex
static void BM_StdMutex(benchmark::State& state) {
    benchmark_function(state, std_mutex);
}

// Benchmark for SpinlockMutex
static void BM_SpinlockMutex(benchmark::State& state) {
    benchmark_function(state, spinlock_mutex);
}

// Register benchmarks with different thread counts
BENCHMARK(BM_StdMutex)
    ->Arg(100000) // 100K iterations per thread
    ->Threads(1)->Threads(2)->Threads(4)->Threads(8);

BENCHMARK(BM_SpinlockMutex)
    ->Arg(100000) // 100K iterations per thread
    ->Threads(1)->Threads(2)->Threads(4)->Threads(8);

static void BM_Mutex_LockUnlock(benchmark::State& state) {
    std::mutex mtx;
    for (auto _ : state) {
        mtx.lock();
        mtx.unlock();
    }
}

static void BM_Mutex_LockUnlock_2(benchmark::State& state) {
    std::mutex mtx;
    std::mutex mtx_1;
    for (auto _ : state) {
        mtx.lock();
        mtx.unlock();
        mtx_1.lock();
        mtx_1.unlock();
    }
}

// Register the function as a benchmark
BENCHMARK(BM_Mutex_LockUnlock);
BENCHMARK(BM_Mutex_LockUnlock_2);

static void BM_Mutex_Contended(benchmark::State& state) {
    std::mutex mtx;
    #pragma omp parallel num_threads(state.range(0))
    {
        for (auto _ : state) {
            mtx.lock();
            mtx.unlock();
        }
    }
}

BENCHMARK(BM_Mutex_Contended)->Range(1, 8);

// Main function for Google Benchmark
BENCHMARK_MAIN();