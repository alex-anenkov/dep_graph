#include "dep_graph.hpp"
#include <iostream>

class custom_task {
public:
    using node_type = graph_node<custom_task>;
    using func_type = typename std::function<void (const custom_task&)>;
    using result_type = typename func_type::result_type;

    custom_task(func_type&& func) : func{ std::move(func) } {}
    ~custom_task() = default;

    result_type operator()(const node_type&) const {
        func(*this);
    }

private:
    const func_type func;
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
    std::for_each(gr.cbegin(), gr.cend(), [](const custom_task::node_type& node) {
        node();
    });
}
