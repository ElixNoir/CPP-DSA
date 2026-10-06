#pragma once

#include <cstdlib>

namespace DSA {

    template <typename T = void>
    struct StandardAllocator {

        [[nodiscard]] static T* allocate(size_t size) {
            return reinterpret_cast<T*>(std::malloc(size * sizeof(T)));
        }

        static void deallocate(T* block) noexcept {
            std::free(reinterpret_cast<void*>(block));
        }

        [[nodiscard]] static T* reallocate(T* block, size_t size) {
            return reinterpret_cast<T*>(std::realloc(reinterpret_cast<void*>(block), size * sizeof(T)));
        }

    };

}