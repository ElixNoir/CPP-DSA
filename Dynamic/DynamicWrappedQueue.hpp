#pragma once

#pragma region Dependencies

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>

#pragma endregion

template <typename T, std::unsigned_integral Address = size_t>
class DynamicWrappedQueue {
protected:

    T* Data;
    Address Capacity;
    Address Size = 0;
    Address Back = 0;
    Address Front = 0;

public:

    DynamicWrappedQueue(Address initialCapacity = 16) {
        Data = new T[initialCapacity];
    }

    ~DynamicWrappedQueue() {
        delete[] Data;
    }

    #pragma region Methods

    bool can_dequeue() const noexcept {
        return Size != 0;
    }

    bool can_enqueue() const noexcept {
        return Size < Capacity;
    }

    Address back() const noexcept {
        return Back;
    }

    Address front() const noexcept {
        return Front;
    }

    Address size() const noexcept {
        return Size;
    }

    T dequeue() {
        if (!can_dequeue()) unsafe_grow(2 * Capacity);
        return unsafe_dequeue(value);
    }

    T unsafe_dequeue() {
        T& value = Data[Front];
      
        Front = (Front + 1) % Capacity;
        Size--;
    
        return value;
    }

    void enqueue(const T& value) {
        if (!can_enqueue()) unsafe_grow(2 * Capacity);
        unsafe_enqueue(value);
    }

    void unsafe_enqueue(const T& value) {
        Data[Back] = value;
    
        Back = (Back + 1) % Capacity;
        Size++;
    }

    void reserve(Address newCapacity) {
        if (newCapacity > Capacity) unsafe_grow(newCapacity);
    }

    void unsafe_grow(Address newCapacity) {
        T* newData = new T[newCapacity];

        if constexpr (std::is_trivially_copyable_v<T>) {
            if (Back > Front) std::memcpy(newData, Data + Front, Size * sizeof(T));
            else if (Size) {
                Address truncation = Capacity - Front;
                std::memcpy(newData, Data + Front, truncation * sizeof(T));
                std::memcpy(newData + truncation, Data, Back * sizeof(T));
            }
        } else for (Address i = 0; i < Size; ++i) newData[i] = Data[(Front + i) % Capacity];
    
        delete[] Data;
    
        Data = newData;
        Capacity = newCapacity;
    
        Front = 0;
        Back = Size;
    }

    #pragma endregion

};
