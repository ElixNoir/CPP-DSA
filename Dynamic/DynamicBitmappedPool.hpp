#pragma region Dependencies

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>

#pragma endregion

template <typename T>
class DynamicBitmappedPool {
protected:

    T* Data;
    size_t* FreeMasks;
    size_t Capacity;
    size_t Size = 0;
    size_t Top = 0;
    
    static constexpr unsigned int Bits = 8 * sizeof(size_t);
    static constexpr unsigned int BitShift = std::bit_width(Bits) - 1;

public:

    DynamicBitmappedPool(size_t initialCapacity = Bits): Capacity(initialCapacity) {
        Data = new T[initialCapacity];
        size_t freeMaskCount = (initialCapacity + Bits - 1) >> BitShift;
        FreeMasks = new size_t[freeMaskCount]{};
    }

    ~DynamicBitmappedPool() {
        delete[] Data;
        delete[] FreeMasks;
    }
    
    #pragma region Methods

    bool can_allocate(const size_t count = 1) const noexcept {
        return Top + count <= Capacity;
    }

    /*bool can_deallocate(const size_t count = 1) const noexcept {
        return Size >= count;
    }*/
    
    size_t capacity() const noexcept {
        return Capacity;
    }

    /*
    size_t size() const noexcept {
        return Size;
    }
    */

    size_t top() const noexcept {
        return Top;
    }

    void reserve(size_t newCapacity) {
        if (newCapacity > Capacity) unsafe_grow(newCapacity);
    }

    void shrink_to_fit() {
        unsafe_shrink(Size);
        if (Size > Capacity) Size = Capacity;
    }

    void unsafe_grow(const size_t newCapacity) {
        size_t newFreeMaskCount = (newCapacity + Bits - 1) >> BitShift;
        
        T* newData = new T[newCapacity]; // May consider a method of reallocation in the future so expansion and shrinking in-place is available
        size_t* newFreeMasks = new size_t[newFreeMaskCount]{};
        
        std::memcpy(newData, Data, Capacity * sizeof(T));
        std::memcpy(newFreeMasks, FreeMasks, ((Capacity + Bits - 1) >> BitShift) * sizeof(size_t));
        
        delete[] Data;
        delete[] FreeMasks;
      
        Data = newData;
        FreeMasks = newFreeMasks;

        Capacity = newCapacity;
    }

    void unsafe_shrink(const size_t newCapacity) {
        size_t newFreeMaskCount = (newCapacity + Bits - 1) >> BitShift;
        
        T* newData = new T[newCapacity]; // May consider a method of reallocation in the future so expansion and shrinking in-place is available
        size_t* newFreeMasks = new size_t[newFreeMaskCount];
        std::memcpy(newData, Data, newCapacity * sizeof(T));
        std::memcpy(newFreeMasks, FreeMasks, ((newCapacity + Bits - 1) >> BitShift) * sizeof(size_t));
        
        delete[] Data;
        delete[] FreeMasks;
      
        Data = newData;
        FreeMasks = newFreeMasks;

        Capacity = newCapacity;
    }

    size_t allocate() {
        size_t freeMaskCount = (Capacity + Bits - 1) >> BitShift;

        Size++;

        for (size_t index = 0; index < freeMaskCount; index++) {
            size_t mask = FreeMasks[index];
            if (mask != std::numeric_limits<size_t>::max()) {
                size_t bit = std::countr_one(mask);
                FreeMasks[index] |= size_t(1) << bit;
                return (index << BitShift) | bit;
            }
        }
    
        unsafe_grow(Capacity * 2);
    
        FreeMasks[freeMaskCount] = 1;
    
        return freeMaskCount << BitShift;
    }

    void deallocate(const size_t address) {
        FreeMasks[address >> BitShift] &= ~(size_t(1) << (address & (Bits - 1)));
    }

    T& operator[](const size_t address) {
        return Data[address];
    }
    
    #pragma endregion

};
