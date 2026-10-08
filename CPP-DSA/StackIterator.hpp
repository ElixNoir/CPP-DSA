#pragma once

namespace DSA {

	template <typename T>
	class StackIterator {
	protected:

		T* address;

	public:

		StackIterator(T* address) : address(address) {}

		T& operator*() const noexcept {
			return *address;
		}

		T* operator->() const noexcept {
			return address;
		}

		StackIterator<T>& operator++() noexcept {
			address++;
			return *this;
		}

		StackIterator<T>& operator--() noexcept {
			address--;
			return *this;
		}

		bool operator!=(const StackIterator<T>& other) const noexcept {
			return address != other.address;
		}

	};

}