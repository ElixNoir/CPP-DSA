#pragma once

#include <cstdlib>

namespace DSA {

    template <typename _T = void, typename _INDEX = size_t>
    struct StandardAllocator {

        using T = _T;
        using INDEX = _INDEX;

        [[nodiscard]] static T* allocate(INDEX size) {
            return reinterpret_cast<T*>(std::malloc(size * sizeof(T)));
        }

        static void deallocate(T* block) noexcept {
            std::free(reinterpret_cast<void*>(block));
        }

        [[nodiscard]] static T* reallocate(T* block, INDEX size) {
            return reinterpret_cast<T*>(std::realloc(reinterpret_cast<void*>(block), size * sizeof(T)));
        }

    };

}