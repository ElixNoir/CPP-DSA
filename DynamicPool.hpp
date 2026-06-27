#pragma region Dependencies

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>

#pragma endregion

template <typename T>
class DynamicPool {
protected:

    T* Data;
    size_t* FreeMasks;
    size_t Capacity;
    size_t Top = 0;
    
    static constexpr unsigned int Bits = 8 * sizeof(size_t);
    static constexpr unsigned int BitShift = std::bit_width(Bits) - 1;

public:

    DynamicPool(size_t initialCapacity = Bits): Capacity(initialCapacity) {
        Data = new T[initialCapacity];
        size_t freeMaskCount = (initialCapacity + Bits - 1) >> BitShift;
        FreeMasks = new size_t[freeMaskCount];
        std::fill_n(FreeMasks, freeMaskCount, std::numeric_limits<size_t>::max());
    }

    ~DynamicPool() {
        delete[] Data;
        delete[] FreeMasks;
    }
    
    #pragma region Methods

    bool can_allocate() const {
        return Top < Capacity;
    }
    
    bool can_allocate(size_t count) const {
        return Top + count <= Capacity;
    }
    
    size_t capacity() const {
        return Capacity;
    }

    size_t top() const {
        return Top;
    }

    void reserve(size_t newCapacity) {
        if (newCapacity > Capacity) {
            unsafe_grow(newCapacity);
        }
    }

    void shrink_to_fit() {
        unsafe_shrink(Size);
        if (Size > newCapacity) Size = newCapacity;
    }

    void unsafe_grow(size_t newCapacity) {
        size_t newFreeMaskCount = (newCapacity + Bits - 1) >> BitShift;
        
        T* newData = new T[newCapacity]; // May consider a method of reallocation in the future so expansion and shrinking in-place is available
        size_t* newFreeMasks = new size_t[newFreeMaskCount];
        
        std::memcpy(newData, Data, Capacity * sizeof(T));
        std::fill_n(newFreeMasks, freeMaskCount, std::numeric_limits<size_t>::max());
        std::memcpy(newFreeMasks, FreeMasks, ((Capacity + Bits - 1) >> BitShift) * sizeof(size_t));
        
        delete[] Data;
        delete[] FreeMasks;
      
        Data = newData;
        FreeMasks = newFreeMasks;

        Capacity = newCapacity;
    }

    void unsafe_shrink(size_t newCapacity) {
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
        if (!can_allocate()) reserve(Capacity * 2);
        return unsafe_allocate();
    }

    size_t unsafe_allocate() {
        size_t freeMaskIndex = Top++ >> BitShift;
        size_t address = std::countr_zero(FreeMasks[freeMaskIndex]);
        FreeMasks[freeMaskIndex] ^= size_t(1) << address;
        return (freeMaskIndex << BitShift) | address;
    }

    void deallocate(size_t address) {
        FreeMasks[address >> BitShift] ^= (size_t(1) << (address & (Bits - 1)));
    }

    T& operator[](size_t address) {
        return Data[address];
    }
    
    #pragma endregion

};
