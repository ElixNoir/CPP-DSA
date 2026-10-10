#include "Hashmap.hpp"

#include <iostream>

using namespace DSA;

int main() {
    Hashmap<StaticContainer<int, 4>> q;
    q.insert(69);
    std::cout << static_cast<int>(q.find(69));

    return 0;
}