#pragma once

#include <concepts>

namespace DSA {

    namespace concepts {

        template <typename T, typename U = void>
        concept Allocator = requires(T allocator, size_t newSize, U* block) {

            { allocator.allocate(newSize) } -> std::same_as<U*>;
            { allocator.deallocate(block) };

        };

        template <typename T, typename U = void>
        concept ReallocatableAllocator = Allocator<T, U> && requires(T allocator, U* block, size_t newSize) {

            { allocator.reallocate(block, newSize) } -> std::same_as<U*>;

        };

    }

}