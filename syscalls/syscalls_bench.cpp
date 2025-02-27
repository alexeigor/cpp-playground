
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: © 2021 Georg Sauthoff <mail@gms.tf>

#include <benchmark/benchmark.h>

#include <unistd.h>
#include <sys/types.h>
#include <sched.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/mman.h>

#include <assert.h>


static void bench_getuid(benchmark::State& state) {
    for (auto _ : state) {
        getuid();
    }
}

BENCHMARK(bench_getuid);

static void bench_getpid(benchmark::State& state) {
    for (auto _ : state) {
        getpid();
    }
}

BENCHMARK(bench_getpid);

static void bench_close(benchmark::State& state) {
    for (auto _ : state) {
        close(999);
    }
}

BENCHMARK(bench_close);

static void bench_syscall(benchmark::State& state) {
    for (auto _ : state) {
        syscall(423);
    }
}

BENCHMARK(bench_syscall);

static void bench_sched_yield(benchmark::State& state) {
    for (auto _ : state) {
        sched_yield();
    }
}

BENCHMARK(bench_sched_yield);

static void bench_clock_gettime(benchmark::State& state) {
    struct timespec ts = {0};
    for (auto _ : state) {
        clock_gettime(CLOCK_REALTIME, &ts);
    }
}

BENCHMARK(bench_clock_gettime);

/*

static void bench_clock_gettime_tai(benchmark::State& state) {
    struct timespec ts = {0};
    for (auto _ : state) {
        clock_gettime(CLOCK_TAI, &ts);
    }
}

BENCHMARK(bench_clock_gettime_tai);
*/

static void bench_clock_gettime_monotonic(benchmark::State& state) {
    struct timespec ts = {0};
    for (auto _ : state) {
        clock_gettime(CLOCK_MONOTONIC, &ts);
    }
}

BENCHMARK(bench_clock_gettime_monotonic);

static void bench_clock_gettime_monotonic_raw(benchmark::State& state) {
    struct timespec ts = {0};
    for (auto _ : state) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    }
}

BENCHMARK(bench_clock_gettime_monotonic_raw);

static void bench_nanosleep0(benchmark::State& state) {
    struct timespec ts = {0};
    for (auto _ : state) {
        int r = nanosleep(&ts, 0);
        assert(!r);
    }
}

BENCHMARK(bench_nanosleep0);

/*

static void bench_nanosleep0_slack1(benchmark::State& state) {
    int r = prctl(PR_SET_TIMERSLACK, 1, 0, 0, 0);
    assert(!r);
    struct timespec ts = {0};
    for (auto _ : state) {
        int r = nanosleep(&ts, 0);
        assert(!r);
    }
}

BENCHMARK(bench_nanosleep0_slack1);

static void bench_nanosleep1_slack1(benchmark::State& state) {
    int r = prctl(PR_SET_TIMERSLACK, 1, 0, 0, 0);
    assert(!r);
    struct timespec ts = { .tv_nsec = 1 };
    for (auto _ : state) {
        int r = nanosleep(&ts, 0);
        assert(!r);
    }
}

BENCHMARK(bench_nanosleep1_slack1);
*/

static void bench_pthread_cond_signal(benchmark::State& state) {
    pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
    for (auto _ : state) {
        int r = pthread_cond_signal(&cv);
        assert(!r);
    }
}

BENCHMARK(bench_pthread_cond_signal);

static void bench_assign(benchmark::State& state) {
    double f = 0;
    for (auto _ : state) {
        f = 23;
        benchmark::DoNotOptimize(f);
    }
}

BENCHMARK(bench_assign);

static void bench_sqrt(benchmark::State& state) {
    double f = 23;
    double g = 0;
    for (auto _ : state) {
        benchmark::DoNotOptimize(f);
        g = sqrt(f);
        benchmark::DoNotOptimize(g);
    }
}

BENCHMARK(bench_sqrt);

static void bench_sqrtrec(benchmark::State& state) {
    double f = 23;
    for (auto _ : state) {
        f = sqrt(f);
    }
}

BENCHMARK(bench_sqrtrec);

static void bench_nothing(benchmark::State& state) {
    unsigned i = 0;
    for (auto _ : state) {
        ++i;
    }
}

BENCHMARK(bench_nothing);



// Benchmarking getpid() syscall
static void BM_GetPid(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(getpid());
    }
}
BENCHMARK(BM_GetPid);

// Benchmarking read() syscall from /dev/null
static void BM_ReadSyscall(benchmark::State& state) {
    int fd = open("/dev/null", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }

    char buffer[64];  // Small buffer for read()
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(read(fd, buffer, sizeof(buffer)));
    }

    close(fd);
}
BENCHMARK(BM_ReadSyscall);

// Benchmarking write() syscall to /dev/null
static void BM_WriteSyscall(benchmark::State& state) {
    int fd = open("/dev/null", O_WRONLY);
    if (fd == -1) {
        perror("open");
        return;
    }

    const char buffer[64] = "Benchmarking write syscall";  // Sample data

    for (auto _ : state) {
        benchmark::DoNotOptimize(write(fd, buffer, sizeof(buffer)));
    }

    close(fd);
}
BENCHMARK(BM_WriteSyscall);

// Benchmarking gettimeofday() syscall
static void BM_GetTimeOfDay(benchmark::State& state) {
    struct timeval tv;
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(gettimeofday(&tv, nullptr));
    }
}
BENCHMARK(BM_GetTimeOfDay);

// Benchmarking clock_gettime() syscall
static void BM_ClockGettime(benchmark::State& state) {
    struct timespec ts;
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(clock_gettime(CLOCK_MONOTONIC, &ts));
    }
}
BENCHMARK(BM_ClockGettime);

// Benchmarking mmap() and munmap() syscalls
static void BM_Mmap(benchmark::State& state) {
    size_t size = 4096;  // 4KB page
    for (auto _ : state) {
        void* addr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        benchmark::DoNotOptimize(addr);
        munmap(addr, size);
    }
}
BENCHMARK(BM_Mmap);

BENCHMARK_MAIN();