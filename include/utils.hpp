#pragma once

#include <iostream>
#include <sstream>

namespace graph {

template <class Task>
class graph;

template <class Task>
std::string to_string(const graph<Task>& gr) {
    std::stringstream ss;
    std::string graph_name = (!gr.name().empty()) ? gr.name() : "noname";
    ss << "graph: name: " << graph_name << std::endl;
    size_t i = 0;
    std::for_each(gr.cbegin(), gr.cend(), [&](const auto& node) {
        std::string node_name = (!node.name().empty()) ? node.name() : "noname";
        ss << "node " << i++ << ": name: " << node_name << ", weight: " << node.weight() << std::endl;
    });
    return ss.str();
}

} // namespace graph
