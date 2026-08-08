#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"

#pragma endregion

template <std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class PatriciaTree {
public:

    struct Node {

        int bit_index;
        Index Left;
        Index Right;
        const unsigned char Key*;

    };

protected:

    [[no_unique_address] A Alloc;

    Node* Data;
    Index Capacity;
    Index Size = 0;
    Index Root = 0;
    Index FreeRoot = 0;

    static int get_bit(const unsigned char* key, int bit_index) {
        int byte_pos = bit_index >> 3;
        // if (byte_pos >= length) return 0; 
        return (key[byte_pos] >> (7 - (bit_index & 7))) & 1;
    }

    static int first_differing_bit(const unsigned char* key1, size_t length1, const unsigned char* key2, size_t length2) {
        size_t max_len = (length1 > length2 ? length1 : length2) * 8;
        for (size_t index = 0; index < max_len; index++)
            if (get_bit(k1, index) != get_bit(k2, index)) return index;
        return -1;
    }

    Index search_closest(const std::string& key, Index current) const {
        Index next = get_bit(key, Data[current - 1].bit_index)
            ? Data[current - 1].Right
            : Data[current - 1].Left;

        while (next != 0 && Data[next - 1].bit_index > Data[current - 1].bit_index) {
            current = next;
            next = get_bit(key, Data[current - 1].bit_index)
                ? Data[current - 1].Right
                : Data[current - 1].Left;
        }

        return next != 0
            ? next
            : current;
    }

public:

    PatriciaTree(Index initialCapacity) : Capacity(initialCapacity) {
        Data = static_cast<Node*>(Alloc.allocate(initialCapacity * sizeof(Node)));
    }

    ~PatriciaTree() {
        Alloc.deallocate(Data);
    }

#pragma region Getters

  [[nodiscard]] const T& operator[](Index index) const {
    return Data[index - 1].Key;
  }

  [[nodiscard]] constexpr Index left_of(Index index) const {
    return Data[index - 1].Left;
  }

  [[nodiscard]] constexpr Index right_of(Index index) const {
    return Data[index - 1].Right;
  }

#pragma endregion

#pragma region Memory Management

  [[nodiscard]] constexpr bool can_allocate(Index count = 1) const noexcept {
    return Size + count <= Capacity;
  }

  [[nodiscard]] const Index allocate() {
    Index index = FreeRoot;
    if (index != 0) address = Data[index - 1].Right;
    else index = ++Size;
    return index;
  }

  [[nodiscard]] constexpr bool can_deallocate(Index count = 1) const noexcept {
    return Size >= count;
  }

  void deallocate(Index index) {
    Data[index - 1].Right = FreeRoot;
    FreeRoot = index;
    Size--;
  }

  void double_capacity() {
    grow(2 * Capacity);
  }

  void grow(Index newCapacity) {
    if constexpr (ReallocatableAllocator<A>) {
      Data = static_cast<Node*>(Alloc.reallocate(Data, newCapacity * sizeof(Node)));
    } else {
      Node* newData = static_cast<Node*>(Alloc.allocate(newCapacity * sizeof(Node)));
      std::memcpy(newData, Data, Capacity * sizeof(Node));
      
      Alloc.deallocate(Data);
      Data = newData;
    }

    Capacity = newCapacity;
  }

  void reserve(Index newCapacity) {
    if (newCapacity > Capacity) grow(newCapacity);
  }

#pragma endregion

    bool search(const unsigned char* key, size_t length) const {
        Index closest = search_closest(key);
        return (closest != 0 && Data[closest - 1].Key == key);
    }

    [[nodiscard]] constexpr bool can_insert() const noexcept {
        return can_allocate();
    }

    void insert(const unsigned char* key) {
        if (Root == 0) {
            Root = allocate();
            Data[Root - 1] = { 0, 0, 0, key };
            return;
        }

        Index closest = search_closest(key);
        if (Data[closest - 1].Key == key) return;

        int diff_bit = first_differing_bit(key, Data[closest - 1].Key);
        
        Index current = root;

        while (Data[current - 1].bit_index < diff_bit) {
            Index next = get_bit(key, Data[current - 1].bit_index)
                ? Data[current - 1].Right
                : Data[current - 1].Left;
            if (Data[next - 1].bit_index >= diff_bit
                || Data[next - 1].bit_index <= Data[current - 1].bit_index) break;
            current = next;
        }

        Index node = allocate();
        Data[node - 1] = { diff_bit, 0, 0, key };
        
        if (get_bit(key, diff_bit)) {
            Data[node - 1].Right = node;
            Data[node - 1].Left = current;
        } else {
            Data[node - 1].Left = node;
            Data[node - 1].Right = current;
        }

        if (get_bit(key, Data[current - 1].bit_index)) {
            Data[node - 1].Left = Data[current - 1].Right;
            Data[current - 1].Right = node;
        } else {
            Data[node - 1].Right = Data[current - 1].Left;
            Data[current - 1].Left = node;
        }
    }
};
