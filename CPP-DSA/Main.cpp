#include "Stack.hpp"

#include <iostream>

int main() {
	DSA::Stack<DSA::DynamicContainer<int>> q(4);
	q.push(69);
	q.double_capacity();
	std::cout << q.pop();

	return 0;
}