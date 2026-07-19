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
        Data = static_cast<T*>(::operator new[](initialCapacity * sizeof(T)));
    }

    ~DynamicStack() {
        if constexpr (!std::is_trivially_destructible_v<T>)
            for (Address i = 0; i < Count; ++i) Data[i].~T();
        ::operator delete[](Data);
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

    #pragma region Getters

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

    #pragma endregion


    #pragma region Setters

    template <typename... Args>
    void emplace(Args&&... args) {
        if (Size == Capacity) unsafe_grow(Capacity * 2);
        unsafe_emplace(std::forward<Args>(args)...);
    }

    void push(const T& value) {
        if (Size == Capacity) unsafe_grow(Capacity * 2);
        unsafe_push(value);
    }

    void push(T&& value) {
        if (Size == Capacity) unsafe_grow(Capacity * 2);
        unsafe_push(value);
    }

    template <typename... Args>
    void unsafe_emplace(Args&&... args) {
        Data[Size++] = T(std::forward<Args>(args)...);
    }

    void unsafe_push(const T& value) {
        Data[Size++] = value;
    }

    void unsafe_push(T&& value) {
        Data[Size++] = std::move(value);
    }

    #pragma endregion


    #pragma region Memory Management

    void reserve(Address newCapacity) {
        if (newCapacity > Capacity) unsafe_grow(newCapacity);
    }

    void shrink_to_fit() {
        unsafe_shrink(Size);
    }

    void unsafe_grow(Address newCapacity) {
        T* newData = static_cast<T*>(::operator new[](newCapacity * sizeof(T)));
        if constexpr (std::is_trivially_copyable_v<T>)
            std::memcpy(newData, Data, Capacity * sizeof(T));
        else {
            for (Address index = 0; index < Capacity; index++) {
                ::new (static_cast<void*>(&newData[index])) T(std::move(Data[index]));
                Data[index].~T();
            }
        }
      
        ::operator delete[](Data);
        Data = newData;
      
        Capacity = newCapacity;
    }

    void unsafe_shrink(Address newCapacity) {
        T* newData = static_cast<T*>(::operator new[](newCapacity * sizeof(T)));
        if constexpr (std::is_trivially_copyable_v<T>)
            std::memcpy(newData, Data, newCapacity * sizeof(T));
        else {
            for (Address index = 0; index < newCapacity; index++) {
                ::new (static_cast<void*>(&newData[index])) T(std::move(Data[index]));
                Data[index].~T();
            }
        }
      
        ::operator delete[](Data);
        Data = newData;

        Capacity = newCapacity;
    }

    #endregion


    T& operator[](const Address address) {
        return Data[address];
    }

    #pragma endregion

};
