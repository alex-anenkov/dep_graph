#pragma once

#include <functional>

namespace graph {

template <class Task>
class node;

class empty_task {
public:
    empty_task() = default;

    void operator()(node<empty_task>&) const {}
};

class lambda_task {
public:
    using func_type = typename std::function<void (node<lambda_task>&)>;

    lambda_task() = default;
    lambda_task(const lambda_task&) = default;
    lambda_task(lambda_task&&) = default;

    template <class Arg>
    lambda_task(Arg&& func) : func{ std::move(func) } {}

    auto operator()(node<lambda_task>& node) const {
        return func(node);
    }

private:
    const func_type func = {};
};

} // namespace graph
