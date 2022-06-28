#pragma once

#include <algorithm>
#include <functional>
#include <tuple>
#include <limits>
#include <list>
#include <stdexcept>
#include <string>

namespace graph {

template <class Task>
class graph;

template <class Task>
class node {
public:
    node(graph<Task>& gr, const Task& task) : gr{ gr }, my_task{ task } {}
    node(graph<Task>& gr, Task&& task) : gr{ gr }, my_task{ std::move(task) } {}

    node(const node&) = delete;
    node(node&&) = default;
    node& operator=(const node&) = delete;
    node& operator=(node&&) = default;

    bool operator<(const node& other) const noexcept {
        return my_weight < other.my_weight;
    }

    auto operator()() const {
        return my_task(*const_cast<node*>(this));
    }

    template <class... Nodes>
    constexpr node& depend(Nodes&... nodes) {
        static_assert(sizeof...(Nodes) > 0, "must be more than 0 arguments");
        static_assert((std::is_same_v<Nodes, node> && ...), "wrong type of arguments");

#ifndef MULTIPLE_DEPEND_SUPPORT
        if (depends_is_set) {
            throw std::runtime_error("multiple depend is not allowed");
        }
        depends_is_set = true;
#endif

        size_t max_weight = 0;
        auto update_nodes = [&](node& node) {
            if (&node == this) {
                throw std::runtime_error("node depends on itself");
            }
            max_weight = std::max(max_weight, node.weight());
#ifdef MULTIPLE_DEPEND_SUPPORT
            node.deps().push_back(*this);
#endif
        };

        (update_nodes(nodes),...);

        if (weight() <= max_weight) {
            size_t diff = (max_weight - weight()) + 1;

            if (weight() > std::numeric_limits<size_t>::max() - diff) {
                throw std::range_error("weight value overflow");
            }

            gr.is_sorted = false;
#ifdef MULTIPLE_DEPEND_SUPPORT
            update_weights(diff);
#else
            my_weight += diff;
#endif
        }

        return *this;
    }

    size_t weight() const noexcept {
        return my_weight;
    }

#ifdef MULTIPLE_DEPEND_SUPPORT
    std::list<std::reference_wrapper<node>>& deps() noexcept {
        return depends;
    }

    const std::list<std::reference_wrapper<node>>& deps() const noexcept {
        return depends;
    }
#endif

    Task& task() noexcept {
        return my_task;
    }

    const Task& task() const noexcept {
        return my_task;
    }

    node& name(const std::string& name) noexcept {
        node_name = name;
        return *this;
    }

    std::string name() const noexcept {
        return node_name;
    }

private:
    size_t my_weight = 0;

#ifdef MULTIPLE_DEPEND_SUPPORT
    std::list<std::reference_wrapper<node>> depends = {}; // means who depends on me

    void update_weights(size_t count) noexcept {
        std::for_each(depends.begin(), depends.end(), [count](node& node) {
            node.update_weights(count);
        });
        my_weight += count;
    }
#else
    bool depends_is_set = false;
#endif

    graph<Task>& gr;
    Task my_task;
    std::string node_name = {};
};

template <class Task>
class graph {
public:
    using node_type = node<Task>;
    using container_type = std::list<node_type>;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using size_type = typename container_type::size_type;

    graph() = default;
    graph(const graph&) = delete;
    graph(graph&&) = default;
    graph& operator=(const graph&) = delete;
    graph& operator=(graph&&) = default;

    node_type& emplace(const Task& task) {
        is_sorted = false;
        return nodes.emplace_back(*this, task);
    }

    node_type& emplace(Task&& task) {
        is_sorted = false;
        return nodes.emplace_back(*this, std::move(task));
    }

    bool sorted() const noexcept {
        return is_sorted;
    }

    void sort() noexcept {
        if (!sorted()) {
            nodes.sort();
            is_sorted = true;
        }
    }

    void clear() noexcept {
        nodes.clear();
        is_sorted = false;
        graph_name.clear();
    }

    void name(const std::string& name) noexcept {
        graph_name = name;
    }

    std::string name() const noexcept {
        return graph_name;
    }

    iterator begin() noexcept {
        return nodes.begin();
    }

    const_iterator cbegin() const noexcept {
        return nodes.cbegin();
    }

    iterator end() noexcept {
        return nodes.end();
    }

    const_iterator cend() const noexcept {
        return nodes.cend();
    }

    reference front() noexcept {
        return nodes.front();
    }

    const_reference front() const noexcept {
        return nodes.front();
    }

    reference back() noexcept {
        return nodes.back();
    }

    const_reference back() const noexcept {
        return nodes.back();
    }

    bool empty() const noexcept {
        return nodes.empty();
    }

    size_type size() const noexcept {
        return nodes.size();
    }

private:
    friend class node<Task>;
    container_type nodes = {};
    bool is_sorted = false;
    std::string graph_name = {};
};

template <class... Ts, class... Nodes>
void depend(std::tuple<Ts...> list, Nodes&... nodes) {
    std::apply([&](auto&... list_nodes) {
        (list_nodes.depend(nodes...), ...);
    }, list);
}

} // namespace graph