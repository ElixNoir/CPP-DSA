#pragma region Dependencies

#include "Statistics.hpp"

#include <iostream>

#pragma endregion

using namespace Algorithms::Statistics;
//using namespace DSA;

int main() {
    std::cout << uniform_expected_value(0, 3);
    return 0;
}
