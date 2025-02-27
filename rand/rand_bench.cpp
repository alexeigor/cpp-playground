#include <benchmark/benchmark.h>
#include <random>
#include <vector>
#include <thread>
#include <cstdlib>

const int kMaxIter = 1e9;  // Reduce iterations for benchmarking

void RunC(int) {    
    int64_t s = 0;    
    for (int i = 0; i < kMaxIter; ++i) {        
        if (rand() & 1) {            
            s += i;        
        }    
    } 
} 

void RunCpp(int ind) {    
    std::mt19937 gen(ind);    
    int64_t s = 0;    
    std::uniform_int_distribution<int> dist(0, 1);    
    for (int i = 0; i < kMaxIter; ++i) {        
        if (dist(gen)) {            
            s += i;        
        }    
    } 
} 

void BenchmarkRunC(benchmark::State& state) {
    int threads_cnt = state.range(0);
    for (auto _ : state) {
        std::vector<std::thread> threads;
        for (int i = 0; i < threads_cnt; ++i) {  
            threads.emplace_back([i]() { RunC(i); }); 
        }
        for (auto& t : threads) {  
            t.join(); 
        }
    }
}

void BenchmarkRunCpp(benchmark::State& state) {
    int threads_cnt = state.range(0);
    for (auto _ : state) {
        std::vector<std::thread> threads;
        for (int i = 0; i < threads_cnt; ++i) {  
            threads.emplace_back([i]() { RunCpp(i); }); 
        }
        for (auto& t : threads) {  
            t.join(); 
        }
    }
}

// Register benchmarks with different thread counts
BENCHMARK(BenchmarkRunC)->Arg(1)->Arg(2)->Arg(4)->Arg(8);
BENCHMARK(BenchmarkRunCpp)->Arg(1)->Arg(2)->Arg(4)->Arg(8);

BENCHMARK_MAIN();