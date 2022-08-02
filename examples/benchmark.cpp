#include <benchmark/benchmark.h>
#include "graph.hpp"
#include "tasks.hpp"
#include <iostream>
#include <string>

static void add_node_to_container(benchmark::State& state) {
    graph::graph<graph::empty_task> gr;
    std::forward_list<graph::node<graph::empty_task>> nodes;
    for (auto _ : state) {
        nodes.emplace_front(gr, graph::empty_task{});
    }
}
BENCHMARK(add_node_to_container)->MinWarmUpTime(0.5);

static void add_node_to_graph(benchmark::State& state) {
    graph::graph<graph::empty_task> gr;
    for (auto _ : state) {
        graph::add_node(graph::empty_task{}, gr);
    }
}
BENCHMARK(add_node_to_graph)->MinWarmUpTime(0.5);

static void add_node_to_same_node(benchmark::State& state) {
    graph::graph<graph::empty_task> gr;
    auto& node = graph::add_node(graph::empty_task{}, gr);
    for (auto _ : state) {
        graph::add_node(graph::empty_task{}, node);
    }
}
BENCHMARK(add_node_to_same_node)->MinWarmUpTime(0.5);

static void add_node_to_node_serial(benchmark::State& state) {
    graph::graph<graph::empty_task> gr;
    std::reference_wrapper<graph::node<graph::empty_task>> node = graph::add_node(graph::empty_task{}, gr);
    for (auto _ : state) {
        node = graph::add_node(graph::empty_task{}, node.get());
    }
}
BENCHMARK(add_node_to_node_serial)->MinWarmUpTime(0.5);

BENCHMARK_MAIN();
