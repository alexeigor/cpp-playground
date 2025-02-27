#include <benchmark/benchmark.h>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <ctime>

template <size_t n>
void mul1(std::vector<std::vector<int>>& a, std::vector<std::vector<int>>& b, std::vector<std::vector<int>>& c) {
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j) {
            int sum = 0;
            for (size_t k = 0; k < n; ++k)
                sum += a[i][k] * b[k][j];
            c[i][j] = sum;
        }
}

template <size_t n>
void mul2(std::vector<std::vector<int>>& a, std::vector<std::vector<int>>& b, std::vector<std::vector<int>>& c) {
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < i; ++j)
            std::swap(b[i][j], b[j][i]);

    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j) {
            int sum = 0;
            for (size_t k = 0; k < n; ++k)
                sum += a[i][k] * b[j][k];
            c[i][j] = sum;
        }
}

// Generate random matrix
template <size_t n>
void fill_random(std::vector<std::vector<int>>& matrix) {
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            matrix[i][j] = rand() % 100;  // Random values between 0 and 99
}

// Benchmark for mul1
template <size_t n>
static void BM_Mul1(benchmark::State& state) {
    std::vector<std::vector<int>> a(n, std::vector<int>(n));
    std::vector<std::vector<int>> b(n, std::vector<int>(n));
    std::vector<std::vector<int>> c(n, std::vector<int>(n));

    fill_random<n>(a);
    fill_random<n>(b);

    for (auto _ : state) {
        mul1<n>(a, b, c);
        benchmark::DoNotOptimize(c);
    }
}

// Benchmark for mul2
template <size_t n>
static void BM_Mul2(benchmark::State& state) {
    std::vector<std::vector<int>> a(n, std::vector<int>(n));
    std::vector<std::vector<int>> b(n, std::vector<int>(n));
    std::vector<std::vector<int>> c(n, std::vector<int>(n));

    fill_random<n>(a);
    fill_random<n>(b);

    for (auto _ : state) {
        mul2<n>(a, b, c);
        benchmark::DoNotOptimize(c);
    }
}

// Define benchmarks with different matrix sizes
BENCHMARK_TEMPLATE(BM_Mul1, 128);
BENCHMARK_TEMPLATE(BM_Mul1, 256);
BENCHMARK_TEMPLATE(BM_Mul1, 512);

BENCHMARK_TEMPLATE(BM_Mul2, 128);
BENCHMARK_TEMPLATE(BM_Mul2, 256);
BENCHMARK_TEMPLATE(BM_Mul2, 512);

BENCHMARK_TEMPLATE(BM_Mul1, 2048);
BENCHMARK_TEMPLATE(BM_Mul1, 2049);

BENCHMARK_TEMPLATE(BM_Mul2, 2048);
BENCHMARK_TEMPLATE(BM_Mul2, 2049);

BENCHMARK_MAIN();