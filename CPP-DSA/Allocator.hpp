#pragma once

#include <concepts>

namespace DSA {

    namespace concepts {

        template <typename T>
        concept Allocator = requires(T allocator, typename T::T* block, typename T::INDEX size) {

            typename T::T;
            typename T::INDEX;

            { allocator.allocate(size) } -> std::same_as<typename T::T*>;
            { allocator.deallocate(block) };

        };

        template <typename T>
        concept ReallocatableAllocator = Allocator<T> && requires(T allocator, typename T::T* block, typename T::INDEX size) {

            { allocator.reallocate(block, size) } -> std::same_as<typename T::T*>;

        };

        template <typename T>
        concept NothrowResizableAllocator = Allocator<T> && requires(T allocator, typename T::T* block, typename T::INDEX size) {
            requires (
                (ReallocatableAllocator<T> && noexcept(allocator.reallocate(block, size)))
                || (!ReallocatableAllocator<T> && noexcept(allocator.allocate(size) && noexcept(block)))
            );
        };

    }

}