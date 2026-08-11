#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"

#include <cstddef>

#pragma endregion

template <typename T, std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class DynamicBinarySearchTree {
protected:

    struct Node {

        Index Left;
        Index Right;
        T Key;

    };

    [[no_unique_address]] A Alloc;

    Node* Data;
    Index Root = 0;
    Index FreeRoot = 0;
    Index Size = 0;
    Index Capacity;

#pragma region Allocation

    [[nodiscard]] constexpr bool can_allocate(Index count = 1) const noexcept {
        return Size + count <= Capacity;
    }

    [[nodiscard]] const Index allocate() {
        Index index = FreeRoot;
        if (index != 0) index = Data[index - 1].Right;
        else index = ++Size;
        return index;
    }

    [[nodiscard]] constexpr bool can_deallocate(Index count = 1) const noexcept {
        return Size >= count;
    }

    void deallocate(Index index) {
        Data[index - 1].Left = 0;
        Data[index - 1].Right = FreeRoot;
        FreeRoot = index;
        Size--;
    }

#pragma endregion

public:

    DynamicBinarySearchTree(Index initialCapacity) : Capacity(initialCapacity) {
        Data = static_cast<Node*>(Alloc.allocate(initialCapacity * sizeof(Node)));
    }

    ~DynamicBinarySearchTree() {
        Alloc.deallocate(Data);
    }

#pragma region Getters

#pragma region Container

    [[nodiscard]] constexpr Index capacity() const noexcept {
        return Capacity;
    }

    [[nodiscard]] constexpr Node* data() const noexcept {
        return Data;
    }

    [[nodiscard]] constexpr Index size() const noexcept {
        return Size;
    }

#pragma endregion

#pragma region Nodes

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

#pragma endregion

#pragma region Memory Management

    void double_capacity() {
        grow(2 * Capacity);
    }

    void grow(Index newCapacity) {
        if constexpr (std::is_trivially_copyable_v<Node>) {
            if constexpr (ReallocatableAllocator<A>)
                Data = static_cast<Node*>(Alloc.reallocate(Data, newCapacity * sizeof(Node)));
            else {
                Node* newData = static_cast<Node*>(Alloc.allocate(newCapacity * sizeof(Node)));
                std::memcpy(newData, Data, Capacity * sizeof(Node));

                Alloc.deallocate(Data);
                Data = newData;
            }
        }
        else {
            Node* newData = static_cast<Node*>(Alloc.allocate(newCapacity * sizeof(Node)));

            if (can_traverse())
                traverse_pre_order(Root, [&](Index index) {
                    ::new (&newData[index]) T(std::move(Data[index]));
                    if constexpr (!std::is_trivially_destructible_v<T>) Data[index].~T();
                });

            Alloc.deallocate(Data);
            Data = newData;
        }

        Capacity = newCapacity;
    }

    void reserve(Index newCapacity) {
        if (newCapacity > Capacity) grow(newCapacity);
    }

#pragma endregion

#pragma region Modification

#pragma region Discard

    [[nodiscard]] constexpr bool can_discard() const noexcept {
        return Size != 0;
    }

    [[nodiscard]] constexpr bool can_discard(Index index) const noexcept {
        return index > 0 && index <= Size && Data[index - 1].Left != 0;
    }

	void discard(Index index) {
		const Node& node = Data[index - 1];

        Index parentIndex = 0;
        Index leftIndex = node.Left;
        Index rightIndex = node.Right;
        if (node.Left == 0 || node.Right == 0) {
            Index replacementIndex = leftIndex != 0 ? leftIndex : rightIndex;
            if (parentIndex == 0) Root = replacementIndex;
            else if (Data[parentIndex - 1].Left == index) Data[parentIndex - 1].Left = replacementIndex;
            else Data[parentIndex - 1].Right = replacementIndex;
        }

        parentIndex = index;
        leftIndex = Data[rightIndex - 1].Left;
        while (leftIndex != 0) {
            parentIndex = rightIndex;
            rightIndex = leftIndex;
            leftIndex = Data[rightIndex - 1].Left;
        }

        Data[index - 1] = Data[rightIndex - 1];

        Index replacementIndex = Data[rightIndex - 1].Right;
        if (parentIndex == index) Data[parentIndex - 1].Right = replacementIndex;
        else Data[parentIndex - 1].Left = replacementIndex;

        deallocate(rightIndex);
	}

    void discard(const T& key) {
        Index currentIndex = find(key);
        if (currentIndex == 0) return;

        discard(currentIndex);
    }

#pragma endregion

#pragma region Insert

    [[nodiscard]] Index insert(const T& key) {
		if (Root == 0) {
			Root = allocate();
			Data[Root - 1] = { 0, 0, key };
			return Root;
		}

		return insert(key, Root);
    }

    [[nodiscard]] Index insert(const T& key, Index currentIndex) {
        Index parentIndex = find_parent_for(key, currentIndex);

        Index newNodeIndex = allocate();
        Data[newNodeIndex - 1] = { 0, 0, key };

        if (key < Data[parentIndex - 1].Key) Data[parentIndex - 1].Left = newNodeIndex;
        else Data[parentIndex - 1].Right = newNodeIndex;

        return newNodeIndex;
    }

#pragma endregion

#pragma region Remove

    [[nodiscard]] constexpr bool can_remove() const noexcept {
        return can_discard();
    }

    [[nodiscard]] Index remove(const T& key) {
        Index currentIndex = find(key);
        if (currentIndex == 0) return 0;

        discard(currentIndex);

        return currentIndex;
    }

#pragma endregion

#pragma region Upsert

    [[nodiscard]] constexpr bool can_upsert() const noexcept {
        return can_allocate();
    }

    [[nodiscard]] Index upsert(const T& key) {
        Index currentIndex = Root;
        if (currentIndex == 0) {
            Root = allocate();
            Data[Root - 1] = { 0, 0, key };
            return Root;
        }

        while (true) {
            const T& currentKey = Data[currentIndex - 1].Key;
            if (currentKey == key) {
                Data[currentIndex - 1] = { 0, 0, key };
                return 0;
            }
            else if (currentKey > key) {
                Index leftIndex = Data[currentIndex - 1].Left;
                if (leftIndex != 0) currentIndex = leftIndex;
                else {
                    Index index = allocate();
                    Data[index - 1] = { 0, 0, key };
                    Data[currentIndex - 1].Left = index;
                    return index;
                }
            }
            else {
                Index rightIndex = Data[currentIndex - 1].Right;
                if (rightIndex != 0) currentIndex = rightIndex;
                else {
                    Index index = allocate();
                    Data[index - 1] = { 0, 0, key };
                    Data[currentIndex - 1].Right = index;
                    return index;
                }
            }
        }
    }

#pragma endregion

#pragma endregion

#pragma region Traversal

    [[nodiscard]] constexpr bool can_traverse() const noexcept {
        return Root != 0;
    }

    template <std::invocable<Index> Callback>
    void traverse_level_order(Callback callback) const {
		traverse_level_order(callback, Root);
    }

    template <std::invocable<Index> Callback>
    void traverse_level_order(Callback callback, Index currentIndex) const {
        Index head = 0;
        Index top = 0;

        while (head <= top) {
            callback(currentIndex);
            top++;

            Index left = Data[currentIndex - 1].Left;
            if (left != 0) {
                callback(left);
                top++;
            }

            Index right = Data[currentIndex - 1].Right;
            if (right != 0) {
                callback(right);
                top++;
            }

            currentIndex = data[++head];
        }
    }

    template <std::invocable<Index> Callback>
    void traverse_post_order(Callback callback) const {
        traverse_post_order(callback, Root);
    }

    template <std::invocable<Index> Callback>
    void traverse_post_order(Callback callback, Index currentIndex) const {
        Index top = 0;
        while (currentIndex != 0) {
            callback(currentIndex);

            Index right = Data[currentIndex - 1].Right;
            if (right != 0) {
                callback(right);
                top++;
            }

            Index left = Data[currentIndex - 1].Left;
            if (left != 0) {
                callback(left);
                top++;
            }

            currentIndex = data[--top];
        }
    }

    template <std::invocable<Index> Callback>
    void traverse_pre_order(Callback callback) const {
        traverse_pre_order(callback, Root);
    }

    template <std::invocable<Index> Callback>
    void traverse_pre_order(Callback callback, Index currentIndex) const {
        Index top = 0;
        while (currentIndex != 0) {
            callback(currentIndex);

            Index left = Data[currentIndex - 1].Left;
            if (left != 0) {
                callback(left);
                top++;
            }

            Index right = Data[currentIndex - 1].Right;
            if (right != 0) {
                callback(right);
                top++;
            }

            currentIndex = data[--top];
        }
    }

#pragma endregion

#pragma region Search

    [[nodiscard]] Index ceiling(const T& key) {
        return ceiling(key, Root);
    }

    [[nodiscard]] Index ceiling(const T& key, Index currentIndex) {
        Index candidate = currentIndex;
        while (currentIndex != 0) {
            const Node& current = Data[currentIndex - 1];
            if (key == current.Key) return currentIndex;
            else if (key > current.Key) {
                candidate = currentIndex;
                currentIndex = current.Right;
            }
            else currentIndex = current.Left;
        }
        return candidate;
    }

    [[nodiscard]] Index find(const T& key) {
        return find(key, Root);
    }

    [[nodiscard]] Index find(const T& key, Index currentIndex) const {
        while (currentIndex != 0) {
            const Node& current = Data[currentIndex - 1];
            if (current.Key == key) return currentIndex;
            currentIndex = (current.Key > key) ? current.Left : current.Right;
        }
        return 0;
    }

    [[nodiscard]] Index find_parent_for(const T& key) {
		return find_parent_for(key, Root);
    }

    [[nodiscard]] Index find_parent_for(const T& key, Index currentIndex) {
        Index parentIndex = 0;
        while (currentIndex != 0) {
            const Node& current = Data[currentIndex - 1];
            parentIndex = currentIndex;
            if (key <= current.Key) currentIndex = current.Left;
            else currentIndex = current.Right;
        }
		return parentIndex;
    }

    [[nodiscard]] Index floor(const T& key) {
        return floor(key, Root);
    }

    [[nodiscard]] Index floor(const T& key, Index currentIndex) const {
        Index candidateIndex = currentIndex;
        while (currentIndex != 0) {
            const Node& current = Data[currentIndex - 1];
            if (key == current.Key) return currentIndex;
            else if (key < current.Key) {
                candidateIndex = currentIndex;
                currentIndex = current.Left;
            }
            else currentIndex = current.Right;
        }
        return candidateIndex;
    }

#pragma endregion

};
