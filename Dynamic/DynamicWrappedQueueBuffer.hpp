#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"
#include "TypeTraits.hpp"

#include <cstring>

#pragma endregion

/// <summary>
/// A dynamic queue of bytes representable as any trivially copyable type.
/// Does not do any runtime alignment or padding.
/// </summary>
/// <typeparam name="Index">The internal index type, exposed for size tuning.</typeparam>
/// <typeparam name="A">The allocator to use when allocating, deallocating, or reallocating.</typeparam>
template <std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class DynamicQueueBuffer {
protected:

    [[no_unique_address]] A Alloc;

    std::byte* Data;
    Index Capacity;
    Index Size = 0;
    Index Back = 0;
    Index Front = 0;

public:

    DynamicQueueBuffer(Index initialCapacity) : Capacity(initialCapacity) {
        Data = static_cast<std::byte*>(Alloc.allocate(initialCapacity));
    }

    ~DynamicQueueBuffer() {
        Alloc.deallocate(Data);
    }

#pragma region Methods

    template <typename T>
    [[nodiscard]] T& operator[](Index index) {
        return *reinterpret_cast<T*>(Data + index);
    }

#pragma region Getters

    [[nodiscard]] constexpr Index back() const noexcept {
        return Back;
    }

    [[nodiscard]] constexpr Index capacity() const noexcept {
        return Capacity;
    }

    [[nodiscard]] constexpr std::byte* data() const noexcept {
        return Data;
    }

    [[nodiscard]] constexpr Index front() const noexcept {
        return Front;
    }

    [[nodiscard]] constexpr bool is_empty() const noexcept {
        return Size == 0;
    }

    [[nodiscard]] constexpr Index size() const noexcept {
        return Size;
    }

#pragma endregion

#pragma region Memory Management

    void double_capacity() {
        grow(2 * Capacity);
    }

    void grow(size_t newCapacity) {
        std::byte* newData = static_cast<std::byte*>(Alloc.allocate(newCapacity));
        if (Size != 0) {
            if (Back > Front)
                std::memcpy(newData, Data + Front, Size);
            else if (Back < Front) {
                Index truncation = Capacity - Front;
                std::memcpy(newData, Data + Front, truncation);
                std::memcpy(newData + truncation, Data, Back);
            }
        }
        
        Alloc.deallocate(Data);
        Back = Size;
        Capacity = newCapacity;
        Data = newData;
        Front = 0;
    }

    void reserve(Index newCapacity) {
        if (newCapacity > Capacity) grow(newCapacity);
    }

    void shrink_to_fit() {
        std::byte* newData;
        if (Size != 0) {
            if (Back > Front) {
                newData = static_cast<std::byte*>(Alloc.allocate(Size));
                std::memcpy(newData, Data + Front, Size);
            } else if (Back < Front) {
                newData = static_cast<std::byte*>(Alloc.allocate(Size));
                Index truncation = Capacity - Front;
                std::memcpy(newData, Data + Front, truncation);
                std::memcpy(newData + truncation, Data, Back);
            }
        }
      
        Alloc.deallocate(Data);
        Back = Size;
        Capacity = Size;
        Data = newData;
        Front = 0;
    }

#pragma endregion

#pragma region Discard

    template <typename T = std::byte>
    [[nodiscard]] constexpr bool can_discard(Index count = 1) const noexcept {
        return Size >= count * sizeof(T);
    }

    template <typename T = std::byte>
    void discard_back(Index count = 1) {
        if (Back >= count * sizeof(T)) Back -= count * sizeof(T);
        else Back += Capacity - count * sizeof(T);
        Size -= count * sizeof(T);
    }

    template <typename T = std::byte>
    void discard_front(Index count = 1) {
        if (Front + count * sizeof(T) >= Capacity) Front += count * sizeof(T);
        else Front -= Capacity;
        Size -= count * sizeof(T);
    }

#pragma endregion

#pragma region Dequeue

    template <typename T = std::byte>
    [[nodiscard]] constexpr bool can_dequeue(Index count = 1) const noexcept {
        return can_discard<T>(count);
    }

    template <typename T = std::byte>
        requires (std::is_trivially_copyable_v<T>)
    [[nodiscard]] T dequeue_back() {
        Size -= sizeof(T);
        
        T value;
        if (Back >= sizeof(T)) {
            Back -= sizeof(T);
            std::memcpy(&value, Data + Back, sizeof(T));
        } else {
            Index truncation = sizeof(T) - Back;
            std::memcpy(&value, Data + Back, truncation);
            std::memcpy(reinterpret_cast<std::byte*>(&value) + truncation, Data, Back);
            Back += Capacity - truncation;
        }
        
        return value;
    }

    template <typename T = std::byte>
        requires (std::is_trivially_copyable_v<T>)
    [[nodiscard]] T dequeue_front() {
        Size -= sizeof(T);
        
        T value;
        if (Front <= Capacity - sizeof(T)) {
            std::memcpy(&value, Data + Front, sizeof(T));
            Front += sizeof(T);
        } else {
            Index truncation = Capacity - Front;
            std::memcpy(&value, Data + Front, truncation);
            Front = sizeof(T) - truncation;
            std::memcpy(reinterpret_cast<std::byte*>(&value) + truncation, Data, Front);
        }
        
        return value;
    }

#pragma endregion

#pragma region Enqueue

    template <typename T = std::byte>
    [[nodiscard]] constexpr bool can_enqueue(Index count = 1) const noexcept {
        return Size + count * sizeof(T) <= Capacity;
    }

    template <typename T = std::byte>
        requires (std::is_trivially_copyable_v<T>)
    void enqueue_back(const T& value) {
        Size += sizeof(T);
        
        const auto* source = reinterpret_cast<const std::byte*>(&value);
        if (Back <= Capacity - sizeof(T)) {
            std::memcpy(Data + Back, &value, sizeof(T));
            Back += sizeof(T);
        } else {
            Index truncation = Capacity - Back;
            std::memcpy(Data + Back, source, truncation);
            Back = sizeof(T) - truncation;
            std::memcpy(Data, source + truncation, Back);
        }
    }

    template <typename T = std::byte>
        requires (std::is_trivially_copyable_v<T>)
    void enqueue_front(const T& value) {
        Size += sizeof(T);

        const auto* source = reinterpret_cast<const std::byte*>(&value);
        if (sizeof(T) <= Front) {
            Front -= sizeof(T);
            std::memcpy(Data + Front, source, sizeof(T));
        } else {
            Index head = Front;
            Index tail = sizeof(T) - Front;
            Front = Capacity - tail;
            std::memcpy(Data + Front, source, tail);
            std::memcpy(Data, source + tail, sizeof(T) - head);
        }
    }

#pragma endregion

#pragma endregion

};
