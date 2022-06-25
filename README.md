# dep_graph
Just my experiment on implementing a task dependency graph in C++17.

The key idea is to build a task dependency graph to execute tasks in the correct order later. A task is a wrapper over a function, the function parameters are unknown until the task is actually executed. Also possible for a task to access other tasks that are dependent on it.

The implementation is not based on a priority queue for performance reasons. To reduce the overhead, we do not need sorting every time a task is added to the graph. Also, unlike the priority queue from stl, the ability to iterate over the graph is supported.

In this case, a graph is a tree. The computational complexity of inserting a node into a tree leaf is O(1). But if a node is inserted not into a tree leaf, then this will may require recalculation of the weights of all underlying nodes. The computational complexity of graph sorting is O(NlogN).
