# dep_graph
Just my experiment on implementing a task dependency graph in C++17.

The key idea is to build a task dependency graph to execute tasks in the correct order later.

The implementation is not based on a priority queue for performance reasons. To reduce the overhead, we do not need sorting every time a task is added to the graph. Also, unlike the priority queue from stl, the ability to iterate over the graph is supported.

In this case, a graph is a tree. The computational complexity of inserting a node into a tree leaf is O(1). But if a node is inserted not into a tree leaf, then this will may require to find a place to insert a new node. Sorting is unstable.

## example
```cpp
#include "graph.hpp"
#include "tasks.hpp"
#include "utils.hpp"

using namespace graph;

graph<lambda_task> gr;
gr.name("graph example");
auto& A = add_node(lambda_task{[](auto& node) {
    std::cout << "execute " << node.name() << std::endl;
}}, gr).name("A");
auto& B = add_node(lambda_task{[](auto& node) {
    std::cout << "execute " << node.name() << std::endl;
}}, gr).name("B");
auto& C = add_node(lambda_task{[](auto& node) {
    std::cout << "execute " << node.name() << std::endl;
}}, A, B).name("C");
auto& D = add_node(lambda_task{[](auto& node) {
    std::cout << "execute " << node.name() << std::endl;
}}, gr).name("D");

// A --|
//     | -- C
// B --|
// D

std::cout << to_string(gr) << std::endl;
std::for_each(gr.begin(), gr.end(), [](auto& node) {
    node();
});
```

Output:
```
graph: name: graph example
node 0: name: D, weight: 0
node 1: name: B, weight: 0
node 2: name: A, weight: 0
node 3: name: C, weight: 1

execute D
execute B
execute A
execute C
```

## how to build
To build examples and tests:
```shell
./build.sh
```
