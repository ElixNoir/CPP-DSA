#pragma once

#pragma region Dependencies

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>

#pragma endregion

template <std::unsigned_integral Address = size_t>
class DynamicStackBuffer {
protected:

    uint8_t* Data;
    Address Capacity;
    Address Size = 0;

public:

    DynamicStackBuffer(Address initialCapacity): Capacity(initialCapacity) {
        Data = new uint8_t[initialCapacity];
    }

    ~DynamicStackBuffer() {
        delete[] Data;
    }

    #pragma region Methods

    template <typename T>
    bool can_pop<T>(const size_t count = 1) const noexcept {
        return Size - count * sizeof(T) >= 0;
    }

    template <typename T>
    bool can_push<T>() const noexcept {
        return Size + sizeof(T) <= Capacity;
    }

    template <typename T>
    T& unsafe_peek<T>() const {
        return *reinterpret_cast<T*>(Data + Size - sizeof(T));
    }

    template <typename T>
    T unsafe_pop<T>() {
        Size -= sizeof(T);
        return *reinterpret_cast<T*>(Data + Size);
    }

    template <typename T>
    T& unsafe_pop_reference<T>() { // dangerous
        Size -= sizeof(T);
        return *reinterpret_cast<T*>(Data + Size);
    }

    template <typename T>
    void push(T& value) {
        if (!can_push<T>()) unsafe_grow(Capacity * 2);
        unsafe_push(value);
    }

    template <typename T>
    void unsafe_push(T& value) {
        std::memcpy(Data + Size, &value, sizeof(T));
        Size += sizeof(T);
    }

    void reserve(Address newCapacity) {
        if (newCapacity > Capacity) unsafe_grow(newCapacity);
    }

    void shrink_to_fit() {
        unsafe_shrink(Size);
    }

    void unsafe_grow(const size_t newCapacity) {
        uint8_t* newData = new uint8_t[newCapacity]; // May consider a method of reallocation in the future so expansion and shrinking in-place is available
        std::memcpy(newData, Data, Capacity * sizeof(T));
        
        delete[] Data;
      
        Data = newData;

        Capacity = newCapacity;
    }

    void unsafe_shrink(const size_t newCapacity) {
        uint8_t* newData = new uint8_t[newCapacity]; // May consider a method of reallocation in the future so expansion and shrinking in-place is available
        std::memcpy(newData, Data, newCapacity * sizeof(T));
        
        delete[] Data;
      
        Data = newData;

        Capacity = newCapacity;
    }

    #pragma endregion

};
