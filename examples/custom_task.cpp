#include "graph.hpp"
#include <iostream>

class custom_task {
public:
    using func_type = typename std::function<void (graph::node<custom_task>&)>;

    template <class Arg>
    custom_task(Arg&& func) : func{ std::move(func) } {}

    auto operator()(graph::node<custom_task>& node) const {
        return func(node);
    }

private:
    const func_type func;
};

int main() {
    graph::graph<custom_task> gr;
    gr.name("graph example");
    auto& A = gr.emplace([](graph::node<custom_task>&) {
        std::cout << "A" << std::endl;
    }).name("A");
    auto& C = gr.emplace([](graph::node<custom_task>&) {
        std::cout << "C" << std::endl;
    });
    auto& B = gr.emplace([](graph::node<custom_task>&) {
        std::cout << "B" << std::endl;
    });
    C.depend(A);
    C.depend(B);

    gr.sort();
    std::cout << to_string(gr) << std::endl;
    std::for_each(gr.begin(), gr.end(), [](const graph::node<custom_task>& node) {
        node();
    });
}
