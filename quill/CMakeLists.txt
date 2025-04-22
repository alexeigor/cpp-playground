# Set the minimum required CMake version
# Quill requires at least CMake 3.10
cmake_minimum_required(VERSION 3.15)

# Define your project
project(MyQuillApp CXX)

# Set the C++ standard to at least C++14
# Quill requires C++14 or higher
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include the FetchContent module to download external projects
include(FetchContent)

# Define the details for fetching Quill
# We recommend using a specific tag or commit hash for reproducible builds
FetchContent_Declare(
  quill
  GIT_REPOSITORY https://github.com/odygrd/quill.git
  GIT_TAG v9.0.0 # Use a specific tag for stability, e.g., v9.0.0
)

# Populate the content for Quill
# This downloads and makes the Quill project available in your build
FetchContent_MakeAvailable(quill)

# Add your executable
add_executable(${PROJECT_NAME} quill_example.cpp)

# Link your executable against the quill library target
# The target name is quill::quill as defined by Quill's CMakeLists.txt
target_link_libraries(${PROJECT_NAME} PRIVATE quill::quill)

# Optional: If you need to specify include directories explicitly, although FetchContent_MakeAvailable
# usually handles this for targets.
# target_include_directories(${PROJECT_NAME} PRIVATE ${quill_SOURCE_DIR}/quill)