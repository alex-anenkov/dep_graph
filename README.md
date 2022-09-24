# dep_graph
Just my experiment on implementing a task dependency graph in C++17.

The key idea is to build a task dependency graph to execute tasks in the correct order later. A task is a wrapper over a function, the function parameters are unknown until the task is actually executed. Also possible for a task to access other tasks that are dependent on it.

The implementation is not based on a priority queue for performance reasons. To reduce the overhead, we do not need sorting every time a task is added to the graph. Also, unlike the priority queue from stl, the ability to iterate over the graph is supported.

In this case, a graph is a tree. The computational complexity of inserting a node into a tree leaf is O(1). But if a node is inserted not into a tree leaf, then this will may require to find a place to insert a new node. Sorting is unstable.

## example
```cpp
using namespace graph;

graph<lambda_task> gr;
gr.name("graph example");
auto& A = add_node(lambda_task{[](auto&) {
    std::cout << "execute A" << std::endl;
}}, gr).name("A");
auto& B = add_node(lambda_task{[](auto&) {
    std::cout << "execute B" << std::endl;
}}, gr).name("B");
auto& C = add_node(lambda_task{[](auto&) {
    std::cout << "execute C" << std::endl;
}}, A, B).name("C");

// A --|
//     | -- C
// B --|

std::cout << to_string(gr) << std::endl;
std::for_each(gr.begin(), gr.end(), [](auto& node) {
    node();
});
```

Output:
```
graph: name: graph example
node 0: name: B, weight: 0
node 1: name: A, weight: 0
node 2: name: C, weight: 1

execute B
execute A
execute C
```

## how to build
To build examples and tests:
```shell
./build.sh
```
