#pragma region Dependencies

#include "rank_sort.hpp"
#include "Primitive.hpp"

#include <iostream>

#pragma endregion

int main() {
    Primitive<unsigned int> q[] = {
        5, 1, 2, 15, 3, 4, 15
    };

    Algorithms::Sorting::rank_sortdup(q, q + sizeof(q) / sizeof(unsigned int));

    for (auto Q : q)
        std::cout << Q << '\n';

    return 0;
}