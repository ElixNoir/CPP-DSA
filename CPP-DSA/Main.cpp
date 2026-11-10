#pragma region Dependencies

//#include "SIMD.hpp"

#include "benchmark.hpp"
#include "Stack.hpp"

#include <vector>

#pragma endregion

using namespace DSA;

volatile void a1(std::vector<int>& q) {
    q.push_back(69420);
}

volatile void b1(Stack<ContainerPolicy<HeapStoragePolicy<>>, int>& q) {
    if (!q.can_push())
        q.expand();
    q.push(69420);
}

volatile int main() {
    std::vector<int> A;
    A.resize(16);
    Stack<ContainerPolicy<HeapStoragePolicy<>>, int> B(1);

    Algorithms::benchmark<a1>(std::ref(A));
    Algorithms::benchmark<b1>(std::ref(B));

    std::cout << A.capacity() << '\n' << A.size() << '\n' << B.get_capacity() << '\n' << B.get_size();
    
    return 0;
}
