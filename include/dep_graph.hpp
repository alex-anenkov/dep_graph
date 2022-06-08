#include <algorithm>
#include <exception>
#include <cassert>
#include <functional>
#include <limits>
#include <list>
#include <sstream>
#include <stdexcept>
#include <string>

template <class task_t>
class graph_node;

class base_task {
public:
    using node_t = graph_node<base_task>;
    using func_t = typename std::function<void (const node_t&)>;
    using result_type = typename func_t::result_type;

    base_task(func_t&& func) : func{ std::move(func) } {}
    ~base_task() = default;

    result_type operator()(const node_t& node) const {
        return func(node);
    }

private:
    const func_t func;
};

template <class task_t>
class graph_node {
public:
    using func_t = typename task_t::func_t;

    graph_node() = delete;
    graph_node(func_t&& func) : task{ std::move(func) } {}
    graph_node(const graph_node&) = delete;
    graph_node(graph_node&&) = default;
    graph_node& operator=(const graph_node&) = delete;
    graph_node& operator=(graph_node&&) = default;

    bool operator<(const graph_node& other) const noexcept {
        return weight < other.weight;
    }

    typename task_t::result_type operator()() const {
        return task(*this);
    }

    template <class... Args>
    void succeed(Args&... args) {
        static_assert(sizeof...(Args) > 0, "must be more than 0 arguments");
        size_t counter = sizeof...(Args);
        int dummy0[] = {(counter += args.get_weight(), 0)...};
        (void)dummy0;

        if (weight > std::numeric_limits<size_t>::max() - counter) {
            throw std::range_error("weight value overflow");
        }

        int dummy1[] = {(args.succeed_nodes.push_back(this), 0)...};
        (void)dummy1;

        update_weight(counter);
    }

    size_t get_weight() const noexcept {
        return weight;
    }

    graph_node& name(const std::string& name) noexcept {
        node_name = name;
        return *this;
    }

    const std::string& name() const noexcept {
        return node_name;
    }

    const task_t& get_task() const noexcept {
        return task;
    }

private:
    size_t weight = 0;
    const task_t task;
    std::string node_name = {};
    std::list<graph_node*> succeed_nodes = {};

    void update_weight(size_t count) noexcept {
        std::for_each(succeed_nodes.begin(), succeed_nodes.end(), [count](graph_node* node) {
            node->update_weight(count);
        });
        weight += count;
    }
};


template <class task_t = base_task>
class graph {
public:
    using node_t = graph_node<task_t>;
    using container_type = std::list<node_t>;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using size_type = typename container_type::size_type;
    using func_t = typename task_t::func_t;

    graph(const std::string& name = {}) : graph_name{ name } {}

    iterator begin() noexcept {
        return list.begin();
    }

    const_iterator cbegin() const noexcept {
        return list.cbegin();
    }

    iterator end() noexcept {
        return list.end();
    }

    const_iterator cend() const noexcept {
        return list.cend();
    }

    reference front() noexcept {
        return list.front();
    }

    reference back() noexcept {
        return list.back();
    }

    bool empty() const noexcept {
        return list.empty();
    }

    size_type size() const noexcept {
        return list.size();
    }

    bool is_sorted() const noexcept {
        return sorted;
    }

    bool is_closed() const noexcept {
        return closed;
    }

    reference emplace(func_t&& func) {
        if (is_closed()) {
            throw std::logic_error("a node cannot be emplaced into an already closed graph");
        }
        list.emplace_back(std::move(func));
        return list.back();
    }

    void sort() noexcept {
        if (!is_sorted()) {
            list.sort();
            if (is_closed()) {
                sorted = true;
            }
        }
    }

    void close() noexcept {
        closed = true;
        sorted = false;
    }

    void execute() const {
        std::for_each(list.cbegin(), list.cend(), [](const_reference node) {
            node();
        });
    }

    void clear() {
        list.clear();
        closed = false;
        sorted = false;
    }

    const std::string& name() const noexcept {
        return graph_name;
    }

private:
    container_type list = {};
    bool sorted = false;
    bool closed = false;
    const std::string graph_name;
};

template <class task_t>
std::string to_string(const graph<task_t>& gr) {
    std::stringstream ss;
    std::string graph_name = (!gr.name().empty()) ? gr.name() : "noname";
    ss << "graph name: " << graph_name;
    ss << ", size: " << gr.size();
    ss << ", is sorted: " << ((gr.is_sorted()) ? "true" : "false");
    ss << ", is closed: " << ((gr.is_closed()) ? "true" : "false");
    ss << std::endl;
    size_t i = 0;
    std::for_each(gr.cbegin(), gr.cend(), [&](const typename graph<task_t>::node_t& node) {
        std::string node_name = (!node.name().empty()) ? node.name() : "noname";
        ss << i++ << ": name: " << node_name << ", weight: " << node.get_weight() << std::endl;
    });
    return ss.str();
}
