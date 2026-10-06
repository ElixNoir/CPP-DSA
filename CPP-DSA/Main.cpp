#include "Stack.hpp"

#include <iostream>
#include <chrono>
#include <vector>

#include "Benchmark.hpp"

using namespace DSA;

#define ITERATIONS 100000

int main() {
    {
        const double ns = benchmark<ITERATIONS>([] {
            Stack<DynamicContainer<int>> stack(16);
            for (int i = 0; i < ITERATIONS; ++i) {
                if (!stack.can_push())
                    stack.double_capacity();
                stack.push(i);
            }
            });

        std::cout << "Custom Stack: "
            << ns << " ns/op\n";
    }

    {
        const double ns = benchmark<ITERATIONS>([] {
            std::vector<int> stack;
            stack.reserve(16);
            for (int i = 0; i < ITERATIONS; ++i)
                stack.push_back(i);
            });

        std::cout << "std::vector: "
            << ns << " ns/op\n";
    }

    return 0;
}