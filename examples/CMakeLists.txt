include_directories(
  ${DEP_GRAPH_SOURCE_DIR}/include
)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -pedantic -O3 -Weffc++)

add_executable(
  custom_task
  custom_task.cpp
)

add_executable(
  benchmark
  benchmark.cpp
)
