#include <algorithm>
#include <benchmark/benchmark.h>
#include <forward_list>
#include <initializer_list>
#include <iostream>
#include <string>

class Graph;

class Node {
public:
    Node(Graph& gr, float a) : gr{ gr }, a{ a } {}
    Node(const Node&) = delete;

    bool operator<(const Node& other) const {
        return node_weight < other.node_weight;
    }

    size_t weight() const {
        return node_weight;
    }

    Node& name(std::string name) {
        node_name = name;
        return *this;
    }

    std::string name() const {
        return node_name;
    }

private:
    friend Node& add_node(Graph&, float);
    friend Node& add_node(Node&, float);

    Graph& gr;
    float a;
    std::forward_list<Node>::iterator node_pos = {};
    size_t node_weight = 0;
    std::string node_name = {};
};

class Graph {
public:
    using container_type = typename std::forward_list<Node>;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;

    Graph() = default;
    Graph(const Graph&) = delete;
    Graph(Graph&&) = default;
    Graph& operator=(const Graph&) = delete;
    Graph& operator=(Graph&&) = default;

    void clear() noexcept {
        nodes.clear();
        graph_name.clear();
    }

    void name(const std::string& name) noexcept {
        graph_name = name;
    }

    std::string name() const noexcept {
        return graph_name;
    }

    iterator begin() {
        return nodes.begin();
    }

    const_iterator cbegin() const {
        return nodes.cbegin();
    }

    iterator end() {
        return nodes.end();
    }

    const_iterator cend() const {
        return nodes.cend();
    }

    reference front() noexcept {
        return nodes.front();
    }

    const_reference front() const noexcept {
        return nodes.front();
    }

private:
    friend Node& add_node(Graph&, float);
    friend Node& add_node(Node&, float);

    container_type nodes = {};
    std::string graph_name = {};

    iterator get_it(iterator node_pos) const {
        auto fwd_pos = node_pos;
        auto cur_pos = node_pos;
        fwd_pos++;
        for (; fwd_pos != nodes.end(); fwd_pos++, cur_pos++) {
            if (fwd_pos->weight() > node_pos->weight()) {
                return cur_pos;
            }
        }
        return cur_pos;
    }
};

Node& add_node(Graph& gr, float a) {
    Node& new_node = gr.nodes.emplace_front(gr, a);
    new_node.node_pos = gr.nodes.begin();
    return new_node;
}

Node& add_node(Node& node, float a) {
    auto new_it = node.gr.nodes.emplace_after(node.gr.get_it(node.node_pos), node.gr, a);
    new_it->node_pos = new_it;
    new_it->node_weight = node.weight() + 1;
    return *new_it;
}

Node& add_node(std::initializer_list<std::reference_wrapper<Node>> list, float a) {
    Node& n = std::max(list, [](const auto& a, const auto& b) {
        return a.get() < b.get();
    });
    return add_node(n, a);
}

static void EmplaceNodeToContainer(benchmark::State& state) {
  Graph gr;
  std::forward_list<Node> nodes;
  for (auto _ : state) {
    nodes.emplace_front(gr, 123);
  }
}
BENCHMARK(EmplaceNodeToContainer);

static void AddNodeToGraph(benchmark::State& state) {
  Graph gr;
  add_node(gr, 1);
  for (auto _ : state) {
    add_node(gr, 123);
  }
}
BENCHMARK(AddNodeToGraph);

static void AddNodeToSameNode(benchmark::State& state) {
  Graph gr;
  Node& node = add_node(gr, 1);
  for (auto _ : state) {
    add_node(node, 123);
  }
}
BENCHMARK(AddNodeToSameNode);

static void AddNodeToNodeSerial(benchmark::State& state) {
  Graph gr;
  std::reference_wrapper<Node> node = add_node(gr, 1);
  for (auto _ : state) {
    node = add_node(node.get(), 123);
  }
}
BENCHMARK(AddNodeToNodeSerial);

BENCHMARK_MAIN();
