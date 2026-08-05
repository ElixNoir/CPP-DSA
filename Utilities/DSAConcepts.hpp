#pragma once

#pragma region Dependencies

#include <concepts>

#pragma endregion

#pragma region Allocation

template <typename A>
concept Allocator = requires(A allocator, void* block, size_t bytes) {
    { allocator.allocate(bytes) } -> std::convertible_to<void*>;
    { allocator.deallocate(block) } -> std::same_as<void>;
};

template <typename A>
concept ReallocatableAllocator = requires(A a, void* block, size_t newSize) {
    { a.reallocate(block, newSize) } -> std::same_as<void*>;
};

#pragma endregion

#pragma region Metaprogramming

template <typename T, typename U = T>
concept Comparable = requires(const T& t, const U& u) {
    { t == u } -> std::same_as<bool>;
    { t != u } -> std::same_as<bool>;
    { t <  u } -> std::same_as<bool>;
    { t <= u } -> std::same_as<bool>;
    { t >  u } -> std::same_as<bool>;
    { t >= u } -> std::same_as<bool>;
};

#pragma endregion
