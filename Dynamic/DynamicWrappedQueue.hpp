#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

#pragma endregion

template <typename T, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class DynamicWrappedQueue {
protected:

    [[no_unique_address]] A Alloc;

    T* Data;
    Index Capacity;
    Index Size = 0;
    Index Back = 0;
    Index Front = 0;

    void grow(Index newCapacity) {
        if constexpr (ReallocatableAllocator<A> && std::is_trivially_copyable_v<T>)
            Data = static_cast<T*>(Alloc.reallocate(Data, newCapacity * sizeof(T)));
        else if constexpr (std::is_trivially_copyable_v<T>) {
            if (Back > Front) std::memcpy(newData, Data + Front, Size * sizeof(T));
            else if (Size) {
                Index truncation = Capacity - Front;
                std::memcpy(newData, Data + Front, truncation * sizeof(T));
                std::memcpy(newData + truncation, Data, Back * sizeof(T));
            }
        } else {
            T* newData = static_cast<T*>(Alloc.allocate(newCapacity * sizeof(T)));

            if (Back > Front) {
                for (Index index = Front; index < Back; index++) {
                    ::new (&newData[index]) T(std::move(Data[index]));
                    if constexpr (!std::is_trivially_destructible_v<T>) Data[index].~T();
                }
            } else if (Size != 0) {
                for (Index index = Front; index < Capacity - Front; index++) {
                    ::new (&newData[index]) T(std::move(Data[index]));
                    if constexpr (!std::is_trivially_destructible_v<T>) Data[index].~T();
                }
    
                for (Index index = Capacity; index < Back; index++) {
                    ::new (&newData[index]) T(std::move(Data[index]));
                    if constexpr (!std::is_trivially_destructible_v<T>) Data[index].~T();
                }
            }

            Alloc.deallocate(Data);
            Data = newData;
        }

        Capacity = newCapacity;

        Back = Size;
        Front = 0;
    }

public:

    DynamicWrappedQueue(Index initialCapacity = 16) {
        Data = static_cast<T*>(Alloc.allocate(initialCapacity * sizeof(T)));
    }

    ~DynamicWrappedQueue() {
        if constexpr (!std::is_trivially_destructible_v<T>)
            for (Index index = 0; index < Size; index++)
                Data[index].~T();
        
        Alloc.deallocate(Data);
    }

#pragma region Methods

#pragma region Getters

    [[nodiscard]] constexpr Index back() const noexcept {
        return Back;
    }

    [[nodiscard]] constexpr Index front() const noexcept {
        return Front;
    }

    [[nodiscard]] constexpr Index size() const noexcept {
        return Size;
    }

#pragma endregion

#pragma region Memory Management

    void double_capacity() {
        grow(2 * newCapacity);
    }

    void reserve(Index newCapacity) {
        if (newCapacity > Capacity) grow(newCapacity);
    }

#pragma endregion

#pragma region Discard

    [[nodiscard]] constexpr bool can_discard(Index count = 1) const noexcept {
        return Size >= count;
    }

    void discard() {
        Front = (Front + 1) % Capacity;
        Size--;
    }

    void discard(Index count) {
        Front = (Front + count) % Capacity;
        Size -= count;
    }

#pragma endregion

#pragma region Dequeue and Enqueue

    [[nodiscard]] constexpr bool can_dequeue(Index count = 1) const noexcept {
        return can_discard(count);
    }

    [[nodiscard]] T dequeue() {
        T& value = Data[Front];
      
        Front = (Front + 1) % Capacity;
        Size--;
    
        return value;
    }

    [[nodiscard]] constexpr bool can_enqueue(Index count = 1) const noexcept {
        return Size + count <= Capacity;
    }

    void enqueue(const T& value) {
        Data[Back] = value;
    
        Back = (Back + 1) % Capacity;
        Size++;
    }

#pragma endregion

#pragma endregion

};
