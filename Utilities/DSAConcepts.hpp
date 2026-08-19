#pragma once

#pragma region Dependencies

#include <concepts>
#include <cstring>

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
class Container : public StorageDescriptor<Index> {
protected:

    T* Data;

public:

#pragma region Methods

#pragma region Getters

    [[nodiscard]] constexpr data() const noexcept {
        return Data;
    }

#pragma endregion

#pragma region Memory Management

#pragma region Grow

    void double_capacity() {
        grow(2 * Capacity);
    }

    void grow(Index newCapacity) {
        if constexpr (ReallocatableAllocator<A>)
            Data = static_cast<T*>(Alloc.reallocate(Data, newCapacity));
        else {
            T* newData = static_cast<T*>(Alloc.allocate(newCapacity * sizeof(T)));
            std::memcpy(newData, Data, Capacity * sizeof(T));
            Alloc.deallocate(Data);
            Data = newData;
        }
        
        Capacity = newCapacity;
    }

    void reserve(Index newCapacity) {
        if (newCapacity > Capacity) grow(newCapacity);
    }

#pragma endregion

#pragma region Shrink

    void resize(Index newCapacity) {
        if (newCapacity < Capacity) shrink(newCapacity);
        else if (newCapacity > Capacity) grow(newCapacity);
    }
    
    void shrink(Index newCapacity) {
        if constexpr (ReallocatableAllocator<A>)
            Data = static_cast<T*>(Alloc.reallocate(Data, newCapacity));
        else {
            T* newData = static_cast<T*>(Alloc.allocate(newCapacity * sizeof(T)));
            std::memcpy(newData, Data, newCapacity * sizeof(T));
            Alloc.deallocate(Data);
            Data = newData;
        }
        
        Capacity = newCapacity;
    }

#pragma endregion

#pragma endregion

#pragma endregion

};

#pragma endregion
