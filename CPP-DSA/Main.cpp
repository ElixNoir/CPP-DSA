#pragma region Dependencies

#include "swap.hpp"

#include <iostream>

#pragma endregion

int main() {
    int a[] = { 5, 2, 8, 1, 9 };
	int b[] = { 3, 7, 4, 6, 0 };
    Algorithms::swap_many(a, b, 5);

    return 0;
}