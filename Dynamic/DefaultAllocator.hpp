#pragma once

#include <cstdlib>

struct DefaultAllocator {

    static void* allocate(size_t bytes) {
        return std::malloc(bytes);
    }

    static void deallocate(void* block) {
        std::free(block);
    }

    static void* reallocate(void* block, size_t bytes) {
        return std::realloc(block, bytes);
    }

};
