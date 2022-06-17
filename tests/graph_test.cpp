#include "dep_graph.hpp"
#include <gtest/gtest.h>

TEST(graph_test, empty) {
    graph gr;
    EXPECT_EQ(gr.size(), 0);
    EXPECT_TRUE(gr.empty());
    EXPECT_EQ(gr.begin(), gr.end());
    EXPECT_EQ(gr.cbegin(), gr.cend());
}

TEST(graph_test, single_node) {
    graph gr;
    auto& A = gr.emplace([](const base_task::node_type&) {}).name("A");
    EXPECT_TRUE(A.name() == "A");
    EXPECT_EQ(gr.size(), 1);
    EXPECT_FALSE(gr.empty());
}

TEST(graph_test, sort) {
    graph gr;
    auto& B = gr.emplace([](const base_task::node_type&) {}).name("B");
    auto& A = gr.emplace([](const base_task::node_type&) {}).name("A");
    gr.emplace([](const base_task::node_type&) {}).name("C");
    B.succeed(A);
    const auto& first_node_before_sort = gr.front();
    EXPECT_TRUE(first_node_before_sort.name() == "B");
    gr.sort();
    const auto& first_node_after_sort = gr.front();
    EXPECT_TRUE(first_node_after_sort.name() != "B"); // A or C
}

TEST(graph_test, direct_graph) {
    graph gr;
    auto& B = gr.emplace([](const base_task::node_type&) {}).name("B");
    auto& A = gr.emplace([](const base_task::node_type&) {}).name("A");
    auto& C = gr.emplace([](const base_task::node_type&) {}).name("C");
    B.succeed(A);
    C.succeed(B);

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
    graph gr;
    auto& B = gr.emplace([](const base_task::node_type&) {}).name("B");
    auto& A = gr.emplace([](const base_task::node_type&) {}).name("A");
    auto& C = gr.emplace([](const base_task::node_type&) {}).name("C");
    auto& D = gr.emplace([](const base_task::node_type&) {}).name("D");
    C.succeed(A, B);
    D.succeed(C);

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
}

// A --| -- C -- |
//     |         | -- E
// B --| -- D -- |

TEST(graph_test, parallel_graph2) {
    graph gr;
    auto& B = gr.emplace([](const base_task::node_type&) {}).name("B");
    auto& A = gr.emplace([](const base_task::node_type&) {}).name("A");
    auto& C = gr.emplace([](const base_task::node_type&) {}).name("C");
    auto& D = gr.emplace([](const base_task::node_type&) {}).name("D");
    auto& E = gr.emplace([](const base_task::node_type&) {}).name("E");
    C.succeed(A, B);
    D.succeed(A, B);
    E.succeed(C, D);

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
}

// A --| -- C -- | -- E -- |
//     |                   | -- F
// B --| -- D ------------ |
// G

TEST(graph_test, parallel_graph3) {
    graph gr;
    auto& B = gr.emplace([](const base_task::node_type&) {}).name("B");
    auto& A = gr.emplace([](const base_task::node_type&) {}).name("A");
    auto& C = gr.emplace([](const base_task::node_type&) {}).name("C");
    auto& D = gr.emplace([](const base_task::node_type&) {}).name("D");
    auto& F = gr.emplace([](const base_task::node_type&) {}).name("F");
    auto& E = gr.emplace([](const base_task::node_type&) {}).name("E");
    auto& G = gr.emplace([](const base_task::node_type&) {}).name("G");
    F.succeed(D, E);
    C.succeed(A, B);
    D.succeed(A);
    D.succeed(B);
    E.succeed(C);

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
}

// A --| -- C
//     |
// B --| -- D

TEST(graph_test, parallel_graph0) {
    graph gr;
    auto& B = gr.emplace([](const base_task::node_type&) {}).name("B");
    auto& A = gr.emplace([](const base_task::node_type&) {}).name("A");
    auto& C = gr.emplace([](const base_task::node_type&) {}).name("C");
    auto& D = gr.emplace([](const base_task::node_type&) {}).name("D");
    C.succeed(A, B);
    D.succeed(A, B);

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
}

TEST(graph_test, clear_graph) {
    graph gr;
    auto& D = gr.emplace([](const base_task::node_type&) {}).name("D");
    gr.sort();
    gr.clear();

    auto& B = gr.emplace([](const base_task::node_type&) {}).name("B");
    auto& A = gr.emplace([](const base_task::node_type&) {}).name("A");
    auto& C = gr.emplace([](const base_task::node_type&) {}).name("C");
    B.succeed(A);
    C.succeed(B);

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
