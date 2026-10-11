#include "BinarySearchTree.hpp"

#include <iostream>

using namespace DSA;

int main() {
    BinarySearchTree<StaticContainer<int, 4>> q;
    auto index = q.insert(69);
    std::cout << q.get(index).key_value_pair.key << '\n' << static_cast<int>(q.find(69));

    return 0;
}