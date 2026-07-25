#pragma once

#include <concepts>

template <typename A>
concept Allocator = requires(A allocator, void* block, size_t bytes) {
    { allocator.allocate(bytes) } -> std::convertible_to<void*>;
    { allocator.deallocate(block) } -> std::same_as<void>;
};

template<typename A>
concept ReallocatableAllocator = requires(A a, void* block, size_t newSize) {
    { a.reallocate(block, newSize) } -> std::same_as<void*>;
};
