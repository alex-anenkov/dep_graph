#include "graph.hpp"
#include <gtest/gtest.h>

TEST(graph_test, empty) {
    graph::graph gr;
    EXPECT_EQ(gr.size(), 0);
    EXPECT_TRUE(gr.empty());
    EXPECT_EQ(gr.begin(), gr.end());
    EXPECT_EQ(gr.cbegin(), gr.cend());
}

TEST(graph_test, single_node) {
    graph::graph gr;
    auto& A = gr.emplace(graph::empty_task{}).name("A");
    EXPECT_TRUE(A.name() == "A");
    EXPECT_EQ(gr.size(), 1);
    EXPECT_FALSE(gr.empty());
}

TEST(graph_test, sort) {
    graph::graph gr;
    auto& B = gr.emplace(graph::empty_task{}).name("B");
    auto& A = gr.emplace(graph::empty_task{}).name("A");
    gr.emplace(graph::empty_task{}).name("C");
    B.depend(A);
    const auto& first_node_before_sort = gr.front();
    EXPECT_TRUE(first_node_before_sort.name() == "B");
    gr.sort();
    const auto& first_node_after_sort = gr.front();
    EXPECT_TRUE(first_node_after_sort.name() != "B"); // A or C
}

TEST(graph_test, direct_graph) {
    graph::graph gr;
    auto& B = gr.emplace(graph::empty_task{}).name("B");
    auto& A = gr.emplace(graph::empty_task{}).name("A");
    auto& C = gr.emplace(graph::empty_task{}).name("C");
    B.depend(A);
    C.depend(B);

    gr.sort();

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

TEST(graph_test, parallel_graph) {
    graph::graph gr;
    auto& B = gr.emplace(graph::empty_task{}).name("B");
    auto& A = gr.emplace(graph::empty_task{}).name("A");
    auto& C = gr.emplace(graph::empty_task{}).name("C");
    auto& D = gr.emplace(graph::empty_task{}).name("D");
    C.depend(A, B);
    D.depend(C);

    gr.sort();

    auto it = gr.cbegin();
    const auto& node1 = *it++;
    const auto& node2 = *it++;
    const auto& node3 = *it++;
    const auto& node4 = *it++;

    EXPECT_FALSE(node1.name() == "C");
    EXPECT_FALSE(node2.name() == "C");
    EXPECT_TRUE(node1.name() != node2.name());
    EXPECT_TRUE(node3.name() == "C");
    EXPECT_TRUE(node4.name() == "D");

    EXPECT_TRUE(A.weight() == B.weight());
    EXPECT_TRUE(C.weight() < D.weight());
}

// A --| -- C -- |
//     |         | -- E
// B --| -- D -- |

TEST(graph_test, parallel_graph2) {
    graph::graph gr;
    auto& B = gr.emplace(graph::empty_task{}).name("B");
    auto& A = gr.emplace(graph::empty_task{}).name("A");
    auto& C = gr.emplace(graph::empty_task{}).name("C");
    auto& D = gr.emplace(graph::empty_task{}).name("D");
    auto& E = gr.emplace(graph::empty_task{}).name("E");
    C.depend(A, B);
    D.depend(A, B);
    E.depend(C, D);

    gr.sort();

    auto it = gr.cbegin();
    const auto& node1 = *it++;
    const auto& node2 = *it++;
    const auto& node3 = *it++;
    const auto& node4 = *it++;
    const auto& node5 = *it++;

    EXPECT_FALSE(node1.name() == "C" || node1.name() == "D");
    EXPECT_FALSE(node2.name() == "C" || node2.name() == "D");
    EXPECT_TRUE(node1.name() != node2.name());
    EXPECT_FALSE(node3.name() == "A" || node3.name() == "B");
    EXPECT_FALSE(node4.name() == "A" || node4.name() == "B");
    EXPECT_TRUE(node3.name() != node4.name());
    EXPECT_TRUE(node5.name() == "E");

    EXPECT_TRUE(A.weight() == B.weight());
    EXPECT_TRUE(C.weight() == D.weight());
}

// A --| -- C -- | -- E -- |
//     |                   | -- F
// B --| -- D ------------ |
// G

TEST(graph_test, parallel_graph3) {
    graph::graph gr;
    auto& B = gr.emplace(graph::empty_task{}).name("B");
    auto& A = gr.emplace(graph::empty_task{}).name("A");
    auto& C = gr.emplace(graph::empty_task{}).name("C");
    auto& D = gr.emplace(graph::empty_task{}).name("D");
    auto& F = gr.emplace(graph::empty_task{}).name("F");
    auto& E = gr.emplace(graph::empty_task{}).name("E");
    auto& G = gr.emplace(graph::empty_task{}).name("G");
    F.depend(D);
    C.depend(A);
    D.depend(A);
    D.depend(B);
    E.depend(C);
    F.depend(E);
    C.depend(B);

    gr.sort();

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
    EXPECT_TRUE(node6.name() == "E");
    EXPECT_TRUE(node7.name() == "F");

    EXPECT_TRUE(A.weight() == B.weight());
    EXPECT_TRUE(A.weight() == G.weight());
    EXPECT_TRUE(C.weight() == D.weight());
    EXPECT_TRUE(C.weight() < E.weight());
    EXPECT_TRUE(E.weight() < F.weight());
}

// A --| -- C
//     |
// B --| -- D

TEST(graph_test, parallel_graph0) {
    graph::graph gr;
    auto& B = gr.emplace(graph::empty_task{}).name("B");
    auto& A = gr.emplace(graph::empty_task{}).name("A");
    auto& C = gr.emplace(graph::empty_task{}).name("C");
    auto& D = gr.emplace(graph::empty_task{}).name("D");
    C.depend(A, B);
    D.depend(A, B);

    gr.sort();

    auto it = gr.cbegin();
    const auto& node1 = *it++;
    const auto& node2 = *it++;
    const auto& node3 = *it++;
    const auto& node4 = *it++;

    EXPECT_FALSE(node1.name() == "C");
    EXPECT_FALSE(node2.name() == "C");
    EXPECT_TRUE(node1.name() != node2.name());
    EXPECT_TRUE(node3.name() == "C" || node3.name() == "D");
    EXPECT_TRUE(node4.name() == "C" || node4.name() == "D");
    EXPECT_TRUE(node3.name() != node4.name());

    EXPECT_TRUE(A.weight() == B.weight());
    EXPECT_TRUE(C.weight() == D.weight());
}

TEST(graph_test, clear_graph) {
    graph::graph gr;
    auto& D = gr.emplace(graph::empty_task{}).name("D");
    gr.sort();
    gr.clear();

    auto& B = gr.emplace(graph::empty_task{}).name("B");
    auto& A = gr.emplace(graph::empty_task{}).name("A");
    auto& C = gr.emplace(graph::empty_task{}).name("C");
    B.depend(A);
    C.depend(B);

    gr.sort();

    auto it = gr.cbegin();
    const auto& node1 = *it++;
    const auto& node2 = *it++;
    const auto& node3 = *it++;

    EXPECT_TRUE(node1.name() == "A");
    EXPECT_TRUE(node2.name() == "B");
    EXPECT_TRUE(node3.name() == "C");
    EXPECT_TRUE(gr.size() == 3);
}
