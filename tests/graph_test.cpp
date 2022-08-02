#include "graph.hpp"
#include "tasks.hpp"
#include <gtest/gtest.h>

TEST(graph, empty) {
    graph::graph<graph::empty_task> gr;
    EXPECT_TRUE(gr.empty());
    EXPECT_EQ(gr.begin(), gr.end());
    EXPECT_EQ(gr.cbegin(), gr.cend());
}

TEST(graph, single_node) {
    graph::graph<graph::empty_task> gr;
    auto& A = graph::add_node(graph::empty_task{}, gr).name("A");
    EXPECT_TRUE(A.name() == "A");
    EXPECT_FALSE(gr.empty());
}

TEST(graph, serial) {
    graph::graph<graph::empty_task> gr;
    auto& A = graph::add_node(graph::empty_task{}, gr).name("A");
    auto& B = graph::add_node(graph::empty_task{}, A).name("B");
    auto& C = graph::add_node(graph::empty_task{}, B).name("C");

    auto it = gr.cbegin();
    const auto& node1 = *it++;
    const auto& node2 = *it++;
    const auto& node3 = *it++;
    
    EXPECT_TRUE(node1.name() == "A");
    EXPECT_TRUE(node2.name() == "B");
    EXPECT_TRUE(node3.name() == "C");
}

// A --|
//     | -- C -- D
// B --|

TEST(graph, parallel_graph) {
    graph::graph<graph::empty_task> gr;
    auto& B = graph::add_node(graph::empty_task{}, gr).name("B");
    auto& A = graph::add_node(graph::empty_task{}, gr).name("A");
    auto& C = graph::add_node(graph::empty_task{}, A, B).name("C");
    auto& D = graph::add_node(graph::empty_task{}, C).name("D");

    auto it = gr.cbegin();
    const auto& node1 = *it++;
    const auto& node2 = *it++;
    const auto& node3 = *it++;
    const auto& node4 = *it++;
    
    EXPECT_EQ(node1.name(), "A");
    EXPECT_EQ(node2.name(), "B");
    EXPECT_EQ(node3.name(), "C");
    EXPECT_EQ(node4.name(), "D");

    EXPECT_TRUE(A.weight() == B.weight());
    EXPECT_TRUE(C.weight() < D.weight());
}

// A --| -- C -- |
//     |         | -- E
// B --| -- D -- |

TEST(graph, parallel_graph2) {
    graph::graph<graph::empty_task> gr;
    auto& B = graph::add_node(graph::empty_task{}, gr).name("B");
    auto& A = graph::add_node(graph::empty_task{}, gr).name("A");
    auto& C = graph::add_node(graph::empty_task{}, A, B).name("C");
    auto& D = graph::add_node(graph::empty_task{}, A, B).name("D");
    auto& E = graph::add_node(graph::empty_task{}, C, D).name("E");

    auto it = gr.cbegin();
    const auto& node1 = *it++;
    const auto& node2 = *it++;
    const auto& node3 = *it++;
    const auto& node4 = *it++;
    const auto& node5 = *it++;

    EXPECT_TRUE(node1.name() == "A" || node1.name() == "B");
    EXPECT_TRUE(node2.name() == "A" || node2.name() == "B");
    EXPECT_TRUE(node3.name() == "C" || node3.name() == "D");
    EXPECT_TRUE(node4.name() == "C" || node4.name() == "D");
    EXPECT_EQ(node5.name(), "E");

    EXPECT_TRUE(A.weight() == B.weight());
    EXPECT_TRUE(C.weight() == D.weight());
}

// A --| -- C -- | -- E -- |
//     |                   | -- F
// B --| -- D ------------ |
// G

TEST(graph, parallel_graph3) {
    graph::graph<graph::empty_task> gr;
    auto& B = graph::add_node(graph::empty_task{}, gr).name("B");
    auto& A = graph::add_node(graph::empty_task{}, gr).name("A");
    auto& G = graph::add_node(graph::empty_task{}, gr).name("G");
    auto& C = graph::add_node(graph::empty_task{}, A, B).name("C");
    auto& D = graph::add_node(graph::empty_task{}, A, B).name("D");
    auto& E = graph::add_node(graph::empty_task{}, C).name("E");
    auto& F = graph::add_node(graph::empty_task{}, E, D).name("F");

    auto it = gr.cbegin();
    const auto& node1 = *it++;
    const auto& node2 = *it++;
    const auto& node3 = *it++;
    const auto& node4 = *it++;
    const auto& node5 = *it++;
    const auto& node6 = *it++;
    const auto& node7 = *it++;
    
    EXPECT_TRUE(node1.name() == "A" || node1.name() == "B" || node1.name() == "G");
    EXPECT_TRUE(node2.name() == "A" || node2.name() == "B" || node2.name() == "G");
    EXPECT_TRUE(node3.name() == "A" || node3.name() == "B" || node3.name() == "G");
    EXPECT_TRUE(node1.name() != node2.name() && node1.name() != node3.name());

    EXPECT_TRUE(node4.name() == "C" || node4.name() == "D");
    EXPECT_TRUE(node5.name() == "C" || node5.name() == "D");
    EXPECT_TRUE(node4.name() != node5.name());

    EXPECT_EQ(node6.name(), "E");
    EXPECT_EQ(node7.name(), "F");

    EXPECT_TRUE(A.weight() == B.weight());
    EXPECT_TRUE(A.weight() == G.weight());
    EXPECT_TRUE(C.weight() == D.weight());
    EXPECT_TRUE(C.weight() < E.weight());
    EXPECT_TRUE(E.weight() < F.weight());
}

// A --| -- C
//     |
// B --| -- D

TEST(graph, parallel_graph4) {
    graph::graph<graph::empty_task> gr;
    auto& B = graph::add_node(graph::empty_task{}, gr).name("B");
    auto& A = graph::add_node(graph::empty_task{}, gr).name("A");
    auto& C = graph::add_node(graph::empty_task{}, A, B).name("C");
    auto& D = graph::add_node(graph::empty_task{}, A, B).name("D");

    auto it = gr.cbegin();
    const auto& node1 = *it++;
    const auto& node2 = *it++;
    const auto& node3 = *it++;
    const auto& node4 = *it++;
    
    EXPECT_TRUE(node1.name() == "A" || node1.name() == "B");
    EXPECT_TRUE(node2.name() == "A" || node2.name() == "B");
    EXPECT_TRUE(node1.name() != node2.name());

    EXPECT_TRUE(node3.name() == "C" || node3.name() == "D");
    EXPECT_TRUE(node4.name() == "C" || node4.name() == "D");
    EXPECT_TRUE(node3.name() != node4.name());

    EXPECT_TRUE(A.weight() == B.weight());
    EXPECT_TRUE(C.weight() == D.weight());
}

// A -- B,C,D,E -- F

TEST(graph, parallel_graph5) {
    graph::graph<graph::empty_task> gr;
    auto& A = graph::add_node(graph::empty_task{}, gr).name("A");
    auto& B = graph::add_node(graph::empty_task{}, A).name("B");
    auto& E = graph::add_node(graph::empty_task{}, A).name("E");
    auto& C = graph::add_node(graph::empty_task{}, A).name("C");
    auto& D = graph::add_node(graph::empty_task{}, A).name("D");
    auto& F = graph::add_node(graph::empty_task{}, B, C, D, E).name("F");

    auto it = gr.cbegin();
    const auto& node1 = *it++;
    const auto& node2 = *it++;
    const auto& node3 = *it++;
    const auto& node4 = *it++;
    const auto& node5 = *it++;
    const auto& node6 = *it++;
    
    EXPECT_EQ(node1.name(), "A");
    EXPECT_TRUE(node2.name() == "B" || node2.name() == "C" || node2.name() == "D" || node2.name() == "E");
    EXPECT_TRUE(node2.name() != node3.name() && node3.name() != node4.name());

    EXPECT_EQ(node6.name(), "F");

    EXPECT_TRUE(A.weight() < B.weight());
    EXPECT_TRUE(B.weight() == C.weight() && C.weight() == D.weight() && D.weight() == E.weight());
    EXPECT_TRUE(E.weight() < F.weight());
}

TEST(graph_test, clear_graph) {
    graph::graph<graph::empty_task> gr;
    gr.name("graph example");
    auto& D = graph::add_node(graph::empty_task{}, gr).name("D");
    gr.clear();

    EXPECT_TRUE(gr.name().empty());
    EXPECT_TRUE(gr.empty());
}
