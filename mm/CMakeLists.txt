cmake_minimum_required(VERSION 3.10)
project(MMBench)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find Google Benchmark
find_package(benchmark REQUIRED)

# Create executable
add_executable(mm mm.cpp)

# Link Google Benchmark and pthread (required for multithreading)
target_link_libraries(mm PRIVATE benchmark::benchmark pthread)