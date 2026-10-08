#pragma once

#include <concepts>

namespace DSA {

    namespace concepts {

        template <typename T>
        concept Allocator = requires(T allocator) {

            typename T::T;
            typename T::INDEX;

            { allocator.allocate(std::declval<typename T::INDEX>()) } -> std::same_as<typename T::T*>;
            { allocator.deallocate(std::declval<typename T::T*>()) };

        };

        template <typename T>
        concept ReallocatableAllocator = Allocator<T> && requires(T allocator) {

            { allocator.reallocate(std::declval<typename T::T*>(), std::declval<typename T::INDEX>()) } -> std::same_as<typename T::T*>;

        };

        template <typename T>
        concept NothrowResizableAllocator = Allocator<T> && requires(T allocator) {
            requires (
                (ReallocatableAllocator<T> && noexcept(allocator.reallocate(std::declval<typename T::T*>(), std::declval<typename T::INDEX>())))
                || (!ReallocatableAllocator<T> && noexcept(allocator.allocate(std::declval<typename T::INDEX>())) && noexcept(allocator.deallocate(std::declval<typename T::T*>())))
            );
        };

    }

}