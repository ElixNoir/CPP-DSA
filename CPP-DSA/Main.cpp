#pragma region Dependencies

#include "Scheduler.hpp"

#include <iostream>

#pragma endregion

void dothing(DSA::EMPTY_STRUCT q) {
    std::cout << "hi" << '\n';
}

int main() {
    DSA::StaticScheduler q;
    q.schedule(dothing, 0);
    q.execute_current_tasks();
    return 0;
}