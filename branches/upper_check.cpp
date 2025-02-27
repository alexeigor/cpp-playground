#include <benchmark/benchmark.h>

#include <cstdint>
#include <limits>
#include <random>
#include <vector>

// -----------------------------------------------------------------------------
// GLOBAL CONFIG: random data and its preparation
// -----------------------------------------------------------------------------

// How many random numbers to generate:
static constexpr size_t kDataSize = 1 << 20; // 1 million

// We'll store random numbers here:
static std::vector<uint32_t> gRandomData;

// Generate the random numbers *before* running benchmarks, so we don't measure
// random generation overhead
static void PrepareRandomData()
{
    gRandomData.reserve(kDataSize);
    std::mt19937 rng{42};  // fixed seed for reproducible results
    std::uniform_int_distribution<uint32_t> dist(
        0, std::numeric_limits<uint32_t>::max()
    );
    for (size_t i = 0; i < kDataSize; ++i) {
        gRandomData.push_back(dist(rng));
    }
}

// -----------------------------------------------------------------------------
// CLAMPING IMPLEMENTATIONS
// -----------------------------------------------------------------------------

// 1. BRANCH-BASED (32-bit)
static constexpr uint32_t HALF_MAX_32 = std::numeric_limits<uint32_t>::max() / 2;
static constexpr uint32_t MAX_VAL_32  = std::numeric_limits<uint32_t>::max();

inline uint32_t get_upper_branch(uint32_t x)
{
    // If x > HALF_MAX_32 => 2*x would overflow => clamp to MAX_VAL_32
    return (x > HALF_MAX_32) ? MAX_VAL_32 : (x << 1);
}

// 2. BRANCHLESS (32-bit)
inline uint32_t get_upper_branchless(uint32_t x)
{
    // Doubling in 32-bit may wrap. Overflow is well-defined for unsigned => wraps modulo 2^32.
    uint32_t doubled = x + x;
    // If overflow happened, doubled < x
    uint32_t overflow = static_cast<uint32_t>(doubled < x);
    // Turn overflow {0,1} into a mask 0x00000000 or 0xFFFFFFFF
    uint32_t mask = static_cast<uint32_t>(-static_cast<int32_t>(overflow));
    // If overflowed => return MAX_VAL_32, else doubled
    return (doubled & ~mask) | (MAX_VAL_32 & mask);
}

// 3. 64-BIT CAST-BASED
inline uint32_t get_upper_uint64_t(uint32_t x)
{
    // Do the doubling in 64-bit
    uint64_t doubled64 = static_cast<uint64_t>(x) << 1; // x * 2
    // If doubling exceeds the 32-bit max, clamp, otherwise cast back
    return (doubled64 > std::numeric_limits<uint32_t>::max())
        ? std::numeric_limits<uint32_t>::max()
        : static_cast<uint32_t>(doubled64);
}

// -----------------------------------------------------------------------------
// BENCHMARKS
// -----------------------------------------------------------------------------

// Each benchmark iterates over the random data in a pseudo-round-robin fashion.
// This avoids measuring random generation. 
// "for (auto _ : state)" repeats enough times for accurate timing.

static void BM_Branch(benchmark::State& state)
{
    size_t idx = 0;
    const size_t size = gRandomData.size();
    for (auto _ : state) {
        benchmark::DoNotOptimize(get_upper_branch(gRandomData[idx]));
        idx = (idx + 1) % size;
    }
}

static void BM_Branchless(benchmark::State& state)
{
    size_t idx = 0;
    const size_t size = gRandomData.size();
    for (auto _ : state) {
        benchmark::DoNotOptimize(get_upper_branchless(gRandomData[idx]));
        idx = (idx + 1) % size;
    }
}

static void BM_CastUint64(benchmark::State& state)
{
    size_t idx = 0;
    const size_t size = gRandomData.size();
    for (auto _ : state) {
        benchmark::DoNotOptimize(get_upper_uint64_t(gRandomData[idx]));
        idx = (idx + 1) % size;
    }
}

// Register the benchmarks
BENCHMARK(BM_Branch);
BENCHMARK(BM_Branchless);
BENCHMARK(BM_CastUint64);

// -----------------------------------------------------------------------------
// MAIN
// -----------------------------------------------------------------------------

int main(int argc, char** argv)
{
    // Prepare random data *once* before running benchmarks:
    PrepareRandomData();

    // Run Google Benchmark
    ::benchmark::Initialize(&argc, argv);
    if (::benchmark::ReportUnrecognizedArguments(argc, argv)) {
        return 1;
    }
    ::benchmark::RunSpecifiedBenchmarks();
    return 0;
}