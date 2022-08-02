include_directories(
  ${DEP_GRAPH_SOURCE_DIR}/include
)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -pedantic -O3 -Weffc++)

add_executable(
  lambda_task
  lambda_task.cpp
)

# apt install libbenchmark-dev
find_package(benchmark REQUIRED)

add_executable(
  benchmark
  benchmark.cpp
)

target_link_libraries(benchmark PRIVATE benchmark::benchmark)
