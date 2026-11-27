#pragma once

#include "LinkedListNode.hpp"

namespace DSA {

	template <concepts::LinkedListNode T_>
	class LinkedListIterator {
	public:

		using INDEX = T_::INDEX;
		using T = T_;

#pragma region C++ Iterator Traits

		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using reference = T&;

#pragma endregion

	protected:

#pragma region Properties

		T* data;
		INDEX index;
		INDEX remaining;

#pragma endregion

	public:

#pragma region Constructors & Destructors

		constexpr LinkedListIterator(T* data, INDEX first, INDEX size) noexcept
			: data(data), index(first), remaining(size) {}

#pragma endregion

#pragma region Operators

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

#pragma endregion

	};

}