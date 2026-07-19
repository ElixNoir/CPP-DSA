#pragma once

#pragma region Dependencies

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>

#pragma endregion

template <typename T, std::unsigned_integral Address = size_t>
class DynamicStack {
protected:

    T* Data;
    Address Capacity;
    Address Size = 0;

public:

    DynamicStack(Address initialCapacity = 16): Capacity(initialCapacity) {
        Data = new T[initialCapacity];
    }

    ~DynamicStack() {
        delete[] Data;
    }

    #pragma region Methods

    template <typename T>
    bool can_pop<T>(const size_t count = 1) const noexcept {
        return Size >= count;
    }

    template <typename T>
    bool can_push<T>(const size_t count = 1) const noexcept {
        return Size + count <= Capacity;
    }

    Address capacity() const {
        return Capacity;
    }

    Address size() const {
        return Size;
    }

    T& unsafe_peek() const {
        return Data[Size - 1];
    }

    T unsafe_pop() {
        return Data[Size--];
    }

    T& unsafe_pop_reference() { // dangerous
        return Data[Size--];
    }

    void push(T& value) {
        if (Size == Capacity) unsafe_grow(Capacity * 2);
        unsafe_push(value);
    }

    void unsafe_push(T& value) {
        Data[Size++] = value;
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
        T* newData = new T[newCapacity];
        std::memcpy(newData, Data, Capacity * sizeof(T));
      
        delete[] Data;
      
        Data = newData;
      
        Capacity = newCapacity;
    }

    void unsafe_shrink(Address newCapacity) {
        T* newData = new T[newCapacity];
        std::memcpy(newData, Data, newCapacity * sizeof(T));
      
        delete[] Data;
      
        Data = newData;
      
        Capacity = newCapacity;
    }

    T& operator[](const Address address) {
        return Data[address];
    }

    #pragma endregion

};
