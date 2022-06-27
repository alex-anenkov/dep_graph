#include "graph.hpp"
#include "tasks.hpp"
#include <iostream>

int main() {
    graph::graph<graph::lambda_task> gr;
    using node_type = graph::graph<graph::lambda_task>::node_type;

    gr.name("graph example");
    auto& A = gr.emplace([](auto&) {
        std::cout << "execute A" << std::endl;
    }).name("A");
    auto& C = gr.emplace([](node_type&) {
        std::cout << "execute C" << std::endl;
    }).name("B");;
    auto& B = gr.emplace([](node_type&) {
        std::cout << "execute B" << std::endl;
    }).name("C");;
    C.depend(A, B);

    gr.sort();
    std::cout << to_string(gr) << std::endl;
    std::for_each(gr.begin(), gr.end(), [](const node_type& node) {
        node();
    });
}
