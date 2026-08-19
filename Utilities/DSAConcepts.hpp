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

#pragma region Structure

template <std::unsigned_integral Index = size_t>
class StorageDescriptor {
protected:

    Index Size = 0;
    Index Capacity;

public:

#pragma region Methods

#pragma region Getters

    [[nodiscard]] constexpr capacity() const noexcept {
        return Capacity;
    }

    [[nodiscard]] constexpr size() const noexcept {
        return Size;
    }

#pragma endregion

    [[nodiscard]] constexpr can_add(Index count = 1) const noexcept {
        return count <= Capacity - Size;
    }

    [[nodiscard]] constexpr can_remove(Index count = 1) const noexcept {
        return Size >= count;
    }

    [[nodiscard]] constexpr is_empty() const noexcept {
        return Size == 0;
    }

    [[nodiscard]] constexpr is_full() const noexcept {
        return Size == Capacity;
    }

#pragma endregion

};

template <typename T, std::unsigned_integral Index = size_t>
class AoSContainer : public StorageDescriptor<Index> {
protected:

    T* Data;

public:

#pragma region Methods

#pragma region Getters

    [[nodiscard]] constexpr data() const noexcept {
        return Data;
    }

#pragma endregion

#pragma endregion

};

/*template <typename T, std::unsigned_integral Index = size_t>
class SoAContainer : public StorageDescriptor<Index> {
protected:

public:

};*/

#pragma endregion
