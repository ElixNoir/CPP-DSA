#pragma once

#pragma region Dependencies

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>

#pragma endregion

template <std::unsigned_integral Address = size_t>
class DynamicBumpAllocator {
protected:

    uint8_t* Data;
    Address Capacity;
    Address Size = 0;

public:

    DynamicBumpAllocator(Address initialCapacity = 0x100): Capacity(initialCapacity) {
        Data = new uint8_t[initialCapacity];
    }

    ~DynamicBumpAllocator() {
        delete[] Data;
    }

    #pragma region Methods

    template <typename T>
    bool can_allocate<T>(size_t count = 1) const {
        return Size + sizeof(T) * count <= Capacity;
    }

    Address capacity() const {
        return Capacity;
    }

    Address size() const {
        return Size;
    }

    template <typename T>
    Address allocate<T>(size_t count = 1) {
        if (!can_allocate<T>(count)) reserve((Capacity + sizeof(T) * count) * 2);
        return unsafe_allocate<T>(count);
    }

    template <typename T>
    Address unsafe_allocate<T>(size_t count = 1) {
        Address address = Size;
        Size += sizeof(T) * count;
        return address;
    }

    void reserve(Address newCapacity) {
        if (newCapacity > Capacity) {
            unsafe_grow(newCapacity);
        }
    }

    void shrink_to_fit() {
        unsafe_shrink(Size);
    }

    void unsafe_grow(Address newCapacity) {
        uint8_t* newData = new uint8_t[newCapacity];
        std::memcpy(newData, Data, Capacity);
      
        delete[] Data;
      
        Data = newData;
      
        Capacity = newCapacity;
    }

    void unsafe_shrink(Address newCapacity) {
        uint8_t* newData = new uint8_t[newCapacity];
        std::memcpy(newData, Data, newCapacity);
      
        delete[] Data;
      
        Data = newData;
      
        Capacity = newCapacity;
    }

    template <typename T>
    T& operator[](const Address address) {
        return *reinterpret_cast<T*>(Data + address); // possible overflow
    }

    template <typename T>
    const T& operator[](const Address address) {
        return *reinterpret_cast<T*>(Data + address); // possible overflow
    }

    #pragma endregion

};
