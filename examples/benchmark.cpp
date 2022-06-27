#include "graph.hpp"
#include "tasks.hpp"
#include <cassert>
#include <chrono>
#include <iostream>

void serial_insert() {
    graph::graph<graph::empty_task> gr;

    for (size_t i = 0; i < 1000; i++) {
        gr.emplace({});
        if (i > 0) {
            auto it = gr.end()----;
            it->depend(gr.back());
        }
    }
    gr.sort();
    for (const auto& node : gr) {
        node();
    }
    gr.clear();

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 1000000; i++) {
        gr.emplace({});
        if (i > 0) {
            auto it = gr.end()----;
            it->depend(gr.back());
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    gr.sort();
    for (const auto& node : gr) {
        node();
    }
    auto t3 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_insert = t2 - t1;
    std::chrono::duration<double, std::milli> ms_total = t3 - t1;
    std::cout << "insertion time: " << ms_insert.count() << "ms, total time: " << ms_total.count() << " ms" << std::endl;
}

void serial_insert_top() {
    graph::graph<graph::empty_task> gr;

    for (size_t i = 0; i < 500; i++) {
        gr.emplace({});
        if (i > 0) {
            auto it = gr.end()----;
            it->depend(gr.back());
        }
    }
    auto it = gr.begin();
    for (size_t i = 0; i < 500; i++) {
        auto& node = gr.emplace({});
        node.depend(*it++);
    }
    gr.sort();
    for (const auto& node : gr) {
        node();
    }
    gr.clear();

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 500000; i++) {
        gr.emplace({});
        if (i > 0) {
            auto it = gr.end()----;
            it->depend(gr.back());
        }
    }
    it = gr.begin();
    for (size_t i = 0; i < 500000; i++) {
        auto& node = gr.emplace({});
        node.depend(*it++);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    gr.sort();
    for (const auto& node : gr) {
        node();
    }
    auto t3 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_insert = t2 - t1;
    std::chrono::duration<double, std::milli> ms_total = t3 - t1;
    std::cout << "insertion time: " << ms_insert.count() << " ms, total time: " << ms_total.count() << " ms" << std::endl;
}

int main() {
    serial_insert();
    serial_insert_top();
}
