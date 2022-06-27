#include <functional>
#include <initializer_list>
#include <iostream>
#include <list>
#include <tuple>

struct Node {
    using func_type = typename std::function<void (const Node&)>;
    
    Node(const Node&) = delete;
    Node(const func_type& func = {}) : func{ func } {}
    Node(func_type&& func) : func{ std::move(func) } {}

    bool operator<(const Node& node) const {
        return weight < node.weight;
    }

    auto operator()() const {
        return func(*this);
    };

    mutable size_t weight = 0;
    mutable bool weight_is_set = false;
    const func_type func;
};

template <class... Ts>
void depend(const Node& node, Ts&... nodes) {
    if constexpr (sizeof...(Ts) == 0) {
        return;
    }

    if (node.weight_is_set) {
        throw;
    }

    size_t max_weight = 0;
    auto f = [&](Node& n) {
        max_weight = std::max(max_weight, n.weight);
    };
    (f(nodes), ...);
    node.weight += max_weight + 1;
    node.weight_is_set = true;
}

template <class... Ts>
void depend(const std::initializer_list<std::reference_wrapper<Node>>& list, Ts&... nodes) {
    if constexpr (sizeof...(Ts) == 0) {
        return;
    }

    for (const Node& node : list) {
        depend(node, nodes...);
    }
}

template <size_t num, class... Nodes>
struct Edge {
    constexpr static size_t weight = num;
    constexpr static size_t size = sizeof...(Nodes);

    constexpr Edge(const Edge&) = delete;
    constexpr Edge(Edge&&) = default;
    constexpr Edge(const std::tuple<const Nodes&...>& nodes) : nodes{ nodes } {}
    constexpr Edge(const Nodes&... nodes) : nodes{ std::tie(nodes...) } {}

    std::tuple<const Nodes&...> nodes;
};

template <class First, class... Ts>
constexpr size_t get_max_value() {
    if constexpr (sizeof...(Ts) == 0 && !std::is_same_v<First, Node>) {
        return First::weight;
    }
    else if constexpr (!std::is_same_v<First, Node>) {
        return std::max(First::weight, get_max_value<Ts...>());
    }
    return 0;
}

template <size_t D, class Arg>
constexpr void set_weight(const Arg& node) {
    if constexpr (std::is_same_v<Arg, Node>) {
        node.weight = D;
    }
}

template <class First, class... Ts>
constexpr auto get_tuple(const First& f, const Ts&... nodes) {
    if constexpr (sizeof...(Ts) != 0 && std::is_same_v<First, Node>) {
        return std::tuple_cat(std::tie(f), get_tuple(nodes...));
    }
    else if constexpr (sizeof...(Ts) != 0 && !std::is_same_v<First, Node>) {
        return std::tuple_cat(f.nodes, get_tuple(nodes...));
    }
    else if constexpr (sizeof...(Ts) == 0 && std::is_same_v<First, Node>) {
        return std::tie(f);
    }
    else if constexpr (sizeof...(Ts) == 0 && !std::is_same_v<First, Node>) {
        return f.nodes;
    }
    else {
        return std::make_tuple();
    }
}

template <size_t D, class... Ts>
constexpr auto construct(const std::tuple<Ts...>& t) {
    return Edge<D, Ts...>(t);
}

template <class... Ts>
constexpr auto make_edge(const Ts&... args) {
    constexpr bool no_edges = (std::is_same_v<Node, Ts> && ...);
    constexpr bool no_nodes = (!std::is_same_v<Node, Ts> && ...);
    constexpr int inc = (no_nodes) ? 0 : 1;

    auto t = get_tuple(args...);

    constexpr size_t max = (no_edges) ? 0 : get_max_value<typename std::remove_reference<Ts>::type...>();
    constexpr size_t weight = max + inc;
    (set_weight<weight>(args), ...);
    return construct<weight>(t);
}

template <size_t D, class F, class Arg>
constexpr void handler(const F& func, const Arg& arg) {
    if constexpr (std::is_same_v<Node, Arg>) {
        func(arg);
    }
    else {
        traverse(arg);
    }
}

template <size_t D, class F, class... Ts>
constexpr void traverse(const F& func, const std::tuple<Ts...>& t) {
    auto f = [&](const auto&... nodes) {
        (handler<D>(func, nodes), ...);
    };
    std::apply(f, t);
}

template <size_t D, class F, class... Ts>
constexpr void traverse(const F& f, const Edge<D, Ts...>& e) {
    traverse<D>(f, e.nodes);
}

template <size_t D, class... Ts>
constexpr auto get_array_from_edge(const Edge<D, Ts...>& e)
{
    constexpr auto get_array = [](auto&& ... n){
        return std::array{ std::forward<decltype(n)>(n)... };
    };
    return std::apply(get_array, e.nodes);
}

std::list<Node> nodes;

int main() {
    auto& n1 = nodes.emplace_back([](const Node& node){
        std::cout << 10 << " weight " << node.weight << std::endl;
    });
    auto& n2 = nodes.emplace_back([](const Node& node){
        std::cout << 20 << " weight " << node.weight << std::endl;
    });
    auto& n3 = nodes.emplace_back([](const Node& node){
        std::cout << 30 << " weight " << node.weight << std::endl;
    });
    auto& n4 = nodes.emplace_back([](const Node& node){
        std::cout << 40 << " weight " << node.weight << std::endl;
    });
    auto& n5 = nodes.emplace_back([](const Node& node){
        std::cout << 50 << " weight " << node.weight << std::endl;
    });
    auto& n6 = nodes.emplace_back([](const Node& node){
        std::cout << 60 << " weight " << node.weight << std::endl;
    });
    auto& n7 = nodes.emplace_back([](const Node& node){
        std::cout << 70 << " weight " << node.weight << std::endl;
    });

    depend(n2, n1);
    depend(n3, n2);

    depend(n5, n4);
    depend({ n6, n7 }, n5);
    // depend(n6, n5);
    // depend(n7, n5);

    // depend(n2, n7);

    // auto e1 = make_edge(n1, n2);
    // auto e2 = make_edge(e1, n3);
    // auto e3 = make_edge(n4);
    // auto e4 = make_edge(e2, e3);
    // auto e5 = make_edge(e4, n5);

    // std::cout << "nodes size " << e5.size << " edge weight " << e5.weight << std::endl;
    
    // traverse(print, e5);

    nodes.sort();
    
    for (const Node& node : nodes) {
        node();
    }

    // auto arr = get_array_from_edge(e5);
    // std::sort(arr.begin(), arr.end());
    // for (const Node& node : arr) {
    //     node();
    // }
}
