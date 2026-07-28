#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"
#include "TypeTraits.hpp"

#include <cstddef>
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

    uint8_t* Data;
    Index Capacity;
    Index Size = 0;
    Index Back = 0;
    Index Front = 0;

public:

    DynamicQueueBuffer(Index initialCapacity) : Capacity(initialCapacity) {
        Data = static_cast<uint8_t*>(Alloc.allocate(initialCapacity));
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

    [[nodiscard]] constexpr uint8_t* data() const noexcept {
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
        if constexpr (ReallocatableAllocator<A>) {
            if (Back > Front) {
                std::memcpy(Data, Data + Front, Size * sizeof(uint8_t));
                Back -= Front;
                Front = 0;
            } else if (Back < Front) {
                Index truncation = Capacity - Front;
                std::memcpy(newData, Data + Front, truncation * sizeof(uint8_t));
                std::memcpy(newData + truncation, Data, Back * sizeof(uint8_t));
                Back += truncation;
                Front = 0;
            }
          
            Data = static_cast<uint8_t*>(Alloc.reallocate(Data, Size * sizeof(uint8_t)));
        } else {
            uint8_t* newData = static_cast<uint8_t*>(Alloc.allocate(newCapacity * sizeof(uint8_t)));
            if (Back > Front) {
                std::memcpy(newData, Data + Front, Size * sizeof(uint8_t));
                Back -= Front;
                Front = 0;
            } else if (Back < Front) {
                Index truncation = Capacity - Front;
                std::memcpy(newData, Data + Front, truncation * sizeof(uint8_t));
                std::memcpy(newData + truncation, Data, Back * sizeof(uint8_t));
                Back += truncation;
                Front = 0;
            }
            Alloc.deallocate(Data);
            Data = newData;
        }
  
        Capacity = newCapacity;
    }

    void reserve(Index newCapacity) {
        if (newCapacity > Capacity) grow(newCapacity);
    }

    void shrink_to_fit() {
        if constexpr (ReallocatableAllocator<A>) {
            if (Back > Front) {
                std::memcpy(Data, Data + Front, Size * sizeof(uint8_t));
                Back -= Front;
                Front = 0;
            } else if (Back < Front) {
                Index truncation = Capacity - Front;
                std::memcpy(newData, Data + Front, truncation * sizeof(uint8_t));
                std::memcpy(newData + truncation, Data, Back * sizeof(uint8_t));
                Back += truncation;
                Front = 0;
            }
          
            Data = static_cast<uint8_t*>(Alloc.reallocate(Data, Size * sizeof(uint8_t)));
        } else {
            uint8_t* newData;
            if (Back > Front) {
                newData = static_cast<uint8_t*>(Alloc.allocate(Size * sizeof(uint8_t)));
                std::memcpy(Data, Data + Front, newCapacity * sizeof(uint8_t));
                Back -= Front;
                Front = 0;
            } else if (Back < Front) {
                newData = static_cast<uint8_t*>(Alloc.allocate(Size * sizeof(uint8_t)));
                Index truncation = Capacity - Front;
                std::memcpy(newData, Data + Front, truncation * sizeof(uint8_t));
                std::memcpy(newData + truncation, Data, Back * sizeof(uint8_t));
                Back += truncation;
                Front = 0;
            }
          
            Alloc.deallocate(Data);
            Data = newData;
        }
  
        Capacity = Size * sizeof(uint8_t);
    }

#pragma endregion

#pragma region Discard

    template <typename T>
    [[nodiscard]] constexpr bool can_discard(Index count) const noexcept {
        return Size >= count * sizeof(T);
    }

    template <typename T = uint8_t>
    void discard_back(const Index count = 1) {
        if (count * sizeof(T) <= Back) Back -= count * sizeof(T);
        else Back += Capacity - count * sizeof(T);
        Size -= sizeof(T);
    }

    template <typename T = uint8_t>
    void discard_front(const Index count = 1) {
        sum_t<Index, Index> newFront = Front + count * sizeof(T);
        if (newFront >= Capacity) Front = newFront - Capacity;
        else Front = newFront;
        Size -= sizeof(T);
    }

#pragma endregion

#pragma region Peek

    template <typename T>
    [[nodiscard]] constexpr bool can_peek(const Index count = 1) const noexcept {
        return can_discard<T>(count);
    }

    template <typename T>
    [[nodiscard]] T& peek_back() {
        return *reinterpret_cast<T*>(Data + Back - sizeof(T));
    }

    template <typename T>
    [[nodiscard]] T& peek_front() {
        return *reinterpret_cast<T*>(Data + Front);
    }

#pragma endregion

#pragma region Pop

    template <typename T>
    [[nodiscard]] constexpr bool can_dequeue(const Index count = 1) const noexcept {
        return can_discard<T>(count);
    }

    template <typename T>
        requires (std::is_trivially_copyable_v<T>)
    [[nodiscard]] T dequeue_back() {
        discard_back<T>();
        T value;
        std::memcpy(&value, Data + Back, sizeof(T));
        return value;
    }

    template <typename T>
        requires (std::is_trivially_copyable_v<T>)
    [[nodiscard]] T dequeue_front() {
        T value;
        std::memcpy(&value, Data + Front, sizeof(T));
        discard_front<T>();
        return value;
    }

#pragma endregion

#pragma region Push

    template <typename T>
    [[nodiscard]] constexpr bool can_enqueue(const Index count = 1) const noexcept {
        return Size + count * sizeof(T) <= Capacity;
    }

    template <typename T>
        requires (std::is_trivially_copyable_v<T>)
    void enqueue_back(const T& value) {
        std::memcpy(Data + Back, &value, sizeof(T));
        Size += sizeof(T);
        sum_t<Index, Index> newBack = Back + count * sizeof(T);
        if (newBack >= Capacity) Back = newBack - Capacity;
        else Back = newFront;
    }

    template <typename T>
        requires (std::is_trivially_copyable_v<T>)
    void enqueue_front(const T& value) {
        if (sizeof(T) <= Front) Front -= sizeof(T);
        else Front += Capacity - sizeof(T);
        Size += sizeof(T);
        std::memcpy(Data + Back, &value, sizeof(T));
    }

#pragma endregion

#pragma endregion

};
