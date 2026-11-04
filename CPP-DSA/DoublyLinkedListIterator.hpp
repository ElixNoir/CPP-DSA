#pragma once

#include "DoublyLinkedListNode.hpp"

/*
namespace DSA {

    template <concepts::DoublyLinkedListNode _T>
    class DoublyLinkedListIterator {
    public:

        using INDEX = typename _T::INDEX;
        using T = _T;

    private:

        T* data;
        INDEX index;
        INDEX remaining;

    public:

        DoublyLinkedListIterator(T* data, INDEX first, INDEX size) noexcept
            : data(data), index(first), remaining(size) {
        }

        T& operator*() const noexcept {
            return data[index];
        }

        DoublyLinkedListIterator& operator++() noexcept {
            index = data[index].next;
            --remaining;
            return *this;
        }

        DoublyLinkedListIterator& operator--() noexcept {
            index = data[index].previous;
            --remaining;
            return *this;
        }

        bool operator!=(const DoublyLinkedListIterator& other) const noexcept {
            return remaining != other.remaining;
        }

    };

}
*/