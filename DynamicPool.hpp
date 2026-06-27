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
            T* newData = new T[newCapacity]; // May consider a method of reallocation in the future so expansion in-place is available
            std::memcpy(newData, Data, Capacity * sizeof(T));
            
            size_t freeMaskCount = (newCapacity + Bits - 1) >> BitShift;
            size_t* newFreeMasks = new size_t[freeMaskCount];
            std::fill_n(newFreeMasks, freeMaskCount, std::numeric_limits<size_t>::max());
            std::memcpy(newFreeMasks, FreeMasks, ((Capacity + Bits - 1) >> BitShift) * sizeof(size_t));
            
            delete[] Data;
            delete[] FreeMasks;
            
            Data = newData;
            FreeMasks = newFreeMasks;
            Capacity = newCapacity;
        }
    }

    void resize(size_t newSize) {
        T* newData = new T[newSize]; // May consider a method of reallocation in the future so shrinking in-place is available
        std::memcpy(newData, Data, newSize * sizeof(T));

        size_t freeMaskCount = (newSize + Bits - 1) >> BitShift;
        size_t* newFreeMasks = new size_t[freeMaskCount];
        std::memcpy(newFreeMasks, FreeMasks, freeMaskCount * sizeof(size_t));
        
        delete[] Data;
        delete[] FreeMasks;
      
        Data = newData;
        FreeMasks = newFreeMasks;
    }

    void shrink_to_fit() {
        resize(Top);
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
