#include "dep_graph.hpp"
#include <iostream>

class custom_task {
public:
    using node_t = graph_node<custom_task>;
    using func_t = typename std::function<void (const custom_task&)>;
    using result_type = typename func_t::result_type;

    custom_task(func_t&& func) : func{ std::move(func) } {}
    ~custom_task() = default;

    result_type operator()(const node_t&) const {
        func(*this);
    }

private:
    const func_t func;
};

int main() {
    graph<custom_task> gr("graph example");
    auto& A = gr.emplace([](const custom_task&) {
        std::cout << "A" << std::endl;
    }).name("A");
    auto& C = gr.emplace([](const custom_task&) {
        std::cout << "C" << std::endl;
    });
    auto& B = gr.emplace([](const custom_task&) {
        std::cout << "B" << std::endl;
    });
    C.succeed(A, B);

    gr.sort();
    std::cout << to_string(gr) << std::endl;
    std::for_each(gr.cbegin(), gr.cend(), [](const custom_task::node_t& node) {
        node();
    });
}
