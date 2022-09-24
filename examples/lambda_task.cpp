#include "graph.hpp"
#include "tasks.hpp"
#include "utils.hpp"
#include <iostream>

int main() {
    graph::graph<graph::lambda_task> gr;

    gr.name("graph example");
    auto& A = graph::add_node(graph::lambda_task{[](auto& node) {
        std::cout << "execute " << node.name() << std::endl;
    }}, gr).name("A");
    auto& B = graph::add_node(graph::lambda_task{[](auto& node) {
        std::cout << "execute " << node.name() << std::endl;
    }}, gr).name("B");
    auto& C = graph::add_node(graph::lambda_task{[](auto& node) {
        std::cout << "execute " << node.name() << std::endl;
    }}, A, B).name("C");
    auto& D = graph::add_node(graph::lambda_task{[](auto& node) {
        std::cout << "execute " << node.name() << std::endl;
    }}, gr).name("D");

    std::cout << graph::to_string(gr) << std::endl;
    std::for_each(gr.begin(), gr.end(), [](auto& node) {
        node();
    });
}
