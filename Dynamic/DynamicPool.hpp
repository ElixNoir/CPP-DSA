#pragma region Dependencies

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>

#pragma endregion

template <typename T, std::unsigned_integral Address = size_t>
class DynamicPool {
protected:

    T* Data;
    Address* FreeList;
    Address Capacity;
    Address Size = 0;
    Address Top = 0;
    Address FreeTop = 0;

public:

    DynamicPool(Address initialCapacity = 16): Capacity(initialCapacity) {
        Data = new T[initialCapacity];
        FreeMasks = new Address[initialCapacity];
    }

    ~DynamicPool() {
        delete[] Data;
        delete[] FreeMasks;
    }
    
    #pragma region Methods

    bool can_allocate(const Address count = 1) const noexcept {
        return FreeTop != 0 || Top + count <= Capacity;
    }
    
    Address capacity() const noexcept {
        return Capacity;
    }

    Address size() const noexcept {
        return Size;
    }

    Address top() const noexcept {
        return Top;
    }

    Address allocate() {
        if (!can_allocate()) resize(2 * Capacity);
        return unsafe_allocate();
    }

    Address unsafe_allocate() {
        Size++;
        return FreeTop != 0 ? FreeList[--FreeTop] : Top++;
    }

    void unsafe_deallocate(const Address address) {
        Size--;
        FreeList[FreeTop++] = address;
    }

    void reserve(const Address newCapacity) {
        if (newCapacity > Capacity) unsafe_grow(newCapacity);
    }

    void shrink_to_fit() {
        unsafe_shrink(Size);
        if (Size > Capacity) Size = Capacity;
    }

    void unsafe_grow(const Address newCapacity) {
        T* newData = new T[newCapacity]; // May consider a method of reallocation in the future so expansion and shrinking in-place is available
        Address* newFreeMasks = new Address[newCapacity]{};
        
        std::memcpy(newData, Data, Capacity * sizeof(T));
        std::memcpy(newFreeMasks, FreeMasks, Capacity * sizeof(Address));
        
        delete[] Data;
        delete[] FreeMasks;
      
        Data = newData;
        FreeMasks = newFreeMasks;

        Capacity = newCapacity;
    }

    void unsafe_shrink(const Address newCapacity) {
        T* newData = new T[newCapacity]; // May consider a method of reallocation in the future so expansion and shrinking in-place is available
        Address* newFreeMasks = new Address[newCapacity]{};
        
        std::memcpy(newData, Data, newCapacity * sizeof(T));
        std::memcpy(newFreeMasks, FreeMasks, newCapacity * sizeof(Address));
        
        delete[] Data;
        delete[] FreeMasks;
      
        Data = newData;
        FreeMasks = newFreeMasks;

        Capacity = newCapacity;
    }

    T& operator[](const Address address) {
        return Data[address];
    }
    
    #pragma endregion

};
