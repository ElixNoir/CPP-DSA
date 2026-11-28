#pragma once

namespace DSA {

	template <typename T>
	class StackIterator {
	public:

#pragma region C++ Iterator Traits

		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using reference = T&;

#pragma endregion

	protected:

#pragma region Properties

		T* address;

#pragma endregion

	public:

#pragma region Constructors & Destructors

		StackIterator(T* address) : address(address) {}

#pragma endregion

#pragma region Operators

		T& operator*() const noexcept {
			return *address;
		}

		T* operator->() const noexcept {
			return address;
		}

		StackIterator<T>& operator++() noexcept {
			++address;
			return *this;
		}

		StackIterator<T>& operator--() noexcept {
			--address;
			return *this;
		}

		bool operator!=(const StackIterator<T>& other) const noexcept {
			return address != other.address;
		}

#pragma endregion

	};

}