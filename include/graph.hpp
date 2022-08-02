#pragma once

#include <algorithm>
#include <forward_list>
#include <functional>
#include <tuple>
#include <initializer_list>
#include <limits>
#include <list>
#include <stdexcept>
#include <string>

namespace graph {

template <class T>
class graph;

template <class T>
class node;

template <class T>
node<T>& add_node(const T& task, graph<T>& gr) {
    node<T>& new_node = gr.nodes.emplace_front(gr, task);
    new_node.node_pos = gr.nodes.begin();
    return new_node;
}

template <class T>
node<T>& add_node(const T& task, node<T>& node) {
    auto new_it = node.gr.nodes.emplace_after(node.gr.get_it(node.node_pos), node.gr, task);
    new_it->node_pos = new_it;
    new_it->node_weight = node.weight() + 1;
    return *new_it;
}

namespace detail {

template <class T>
T& vmax(T& val) {
    return val;
}

template <class T, class... Ts>
T& vmax(T& val1, T& val2, Ts&... vs) {
    bool comp = val1 < val2;
    return (!comp) ?
      vmax(val1, vs...) :
      vmax(val2, vs...);
}

} // namespace detail

template <class T, class... Nodes, class = typename std::enable_if<sizeof...(Nodes) >= 2>::type>
node<T>& add_node(const T& task, Nodes&... list) {
    node<T>& n = detail::vmax(list...);
    return add_node(task, n);
}

template <class T>
class node {
public:
    node(graph<T>& gr, const T& task) : gr{ gr }, task{ task } {}
    node(graph<T>& gr, T&& task) : gr{ gr }, task{ std::move(task) } {}

    node(const node&) = delete;
    node(node&&) = default;
    node& operator=(const node&) = delete;
    node& operator=(node&&) = default;

    bool operator<(const node& other) const {
        return node_weight < other.node_weight;
    }

    auto operator()() const {
        return task(*this);
    }

    size_t weight() const {
        return node_weight;
    }

    node& name(std::string name) {
        node_name = name;
        return *this;
    }

    std::string name() const {
        return node_name;
    }

    typename graph<T>::iterator pos() {
        return node_pos;
    }

private:
    friend node<T>& add_node<>(const T&, graph<T>&);
    friend node<T>& add_node<>(const T&, node<T>&);

    graph<T>& gr;
    T task;
    typename graph<T>::iterator node_pos = {};
    size_t node_weight = 0;
    std::string node_name = {};
};

template <class T>
class graph {
public:
    using node_type = node<T>;
    using container = typename std::forward_list<node<T>>;
    using iterator = typename container::iterator;
    using const_iterator = typename container::const_iterator;

    graph() = default;
    graph(const graph&) = delete;
    graph(graph&&) = default;
    graph& operator=(const graph&) = delete;
    graph& operator=(graph&&) = default;

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

    bool empty() const noexcept {
        return nodes.empty();
    }

private:
    friend node<T>& add_node<>(const T&, graph<T>&);
    friend node<T>& add_node<>(const T&, node<T>&);

    container nodes = {};
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

} // namespace graph
