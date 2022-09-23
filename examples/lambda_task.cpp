#include "graph.hpp"
#include "tasks.hpp"
#include "utils.hpp"
#include <iostream>

int main() {
    graph::graph<graph::lambda_task> gr;

    gr.name("graph example");
    auto& A = graph::add_node(graph::lambda_task{[](auto&) {
        std::cout << "execute A" << std::endl;
    }}, gr).name("A");
    auto& B = graph::add_node(graph::lambda_task{[](auto&) {
        std::cout << "execute B" << std::endl;
    }}, gr).name("B");
    auto& C = graph::add_node(graph::lambda_task{[](auto&) {
        std::cout << "execute C" << std::endl;
    }}, A, B).name("C");

    std::cout << graph::to_string(gr) << std::endl;
    std::for_each(gr.begin(), gr.end(), [](auto& node) {
        node();
    });
}
