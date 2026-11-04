#pragma once

#include "LinkedListNode.hpp"

namespace DSA {

    template <concepts::LinkedListNode _T>
    class LinkedListIterator {
    public:

        using iterator_category = std::forward_iterator_tag;

        using INDEX = typename _T::INDEX;
        using T = _T;

    protected:

        T* data;
        INDEX index;
        INDEX remaining;

    public:

        LinkedListIterator(T* data, INDEX first, INDEX size) noexcept
            : data(data), index(first), remaining(size) {
        }

        T& operator*() const noexcept {
            return data[index];
        }

        LinkedListIterator& operator++() noexcept {
            index = data[index].next;
            --remaining;
            return *this;
        }

        bool operator!=(const LinkedListIterator& other) const noexcept {
            return remaining != other.remaining;
        }

    };

}