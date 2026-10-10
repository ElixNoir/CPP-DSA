#pragma once

#include "Bitset.hpp"
#include "IncrementalContainer.hpp"
#include "Stack.hpp"

namespace DSA {

	template <concepts::Container CONTAINER>
	class Pool : public IncrementalContainer<CONTAINER> {
	public:

		using Base = CONTAINER;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

		using Base::data;
		using Base::size;

		INDEX free_top = 0;
		Stack<DynamicContainer<T, INDEX>> free_stack;

		void helper_destroy() noexcept requires (
			!std::is_trivially_destructible_v<T>
			&& concepts::DynamicContainer<CONTAINER>
		) {
			Bitset<CONTAINER> bitset((Base::get_capacity() + Bitmask<uintmax_t>::BitCount - 1) >> Bitmask<uintmax_t>::BitShift);
			bitset.zero();
			for (INDEX index : free_stack)
				bitset.get(index >> Bitmask<uintmax_t>::BitShift).set(index & (Bitmask<uintmax_t>::BitCount - 1));

			INDEX count = size;
			for (INDEX index : bitset.zeros()) {
				if (count == 0)
					break;
				destroy_at(Base::get_data() + index);
				count--;
			}
		}

		/*
		void helper_destroy() noexcept requires (
			!std::is_trivially_destructible_v<T>
			&& concepts::StaticContainer<CONTAINER>
		) {
			Bitset<CONTAINER> bitset;
			bitset.zero();
			for (INDEX index : free_stack)
				bitset.get(index >> Bitmask<uintmax_t>::BitShift).set(index & (Bitmask<uintmax_t>::BitCount - 1));

			INDEX count = size;
			for (INDEX index : bitset.zeros()) {
				if (count == 0)
					break;
				destroy_at(Base::get_data() + index);
				count--;
			}
		}
		*/

		void helper_resize() noexcept(
			std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
			&& !concepts::ResizableContainer<CONTAINER>
		) {
			using ALLOCATOR = CONTAINER::ALLOCATOR;

			Bitset<CONTAINER> occupied((Base::get_capacity() + Bitmask<uintmax_t>::BitCount - 1) >> Bitmask<uintmax_t>::BitShift);
			occupied.zero();
			for (INDEX index : free_stack)
				occupied.get(index >> Bitmask<uintmax_t>::BitShift).set(index & (Bitmask<uintmax_t>::BitCount - 1));

			T* const oldData = Base::get_data();
			T* const newData = ALLOCATOR::allocate(newCapacity);

			INDEX count = size;

			try {
				for (INDEX index : occupied.zeros()) {
					if (count == 0)
						break;
					move_construct_at(newData + index, std::move(oldData[index]));
					count--;
				}
			}
			catch (...) {
				if constexpr (!std::is_trivially_destructible_v<T>) {
					for (INDEX index : occupied.zeros()) {
						if (count == size)
							break;
						destroy_at(newData + index);
						count++;
					}
				}

				ALLOCATOR::deallocate(newData);

				throw;
			}

			if constexpr (!std::is_trivially_destructible_v<T>) {
				for (INDEX index : occupied.zeros()) {
					if (count == size)
						break;

					destroy_at(oldData + index);
					count++;
				}
			}

			ALLOCATOR::deallocate(oldData);

			data = reinterpret_cast<std::byte*>(newData);
			Base::capacity = newCapacity;
		}

	public:

#pragma region Constructors & Destructors

		~Pool() noexcept requires (!std::is_trivially_destructible_v<T>) {
			helper_destroy();
		}

		~Pool() = default;

#pragma endregion

#pragma region Methods

#pragma region IncrementalContainer

		constexpr INDEX add(const T& value) noexcept(
			std::is_nothrow_copy_constructible_v<T>
		) {
			INDEX index;
			if (!free_stack.is_empty()) {
				index = free_stack.pop();
				try {
					copy_construct_at(Base::get_data() + index, value);
				}
				catch (...) {
					free_stack.push(index);
					throw;
				}
			}
			else {
				index = free_top++;
				try {
					copy_construct_at(Base::get_data() + index, value);
				}
				catch (...) {
					free_top--;
					throw;
				}
			}

			size++;

			return index;
		}

		constexpr INDEX add(T&& value) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) {
			INDEX index;
			if (!free_stack.is_empty()) {
				index = free_stack.pop();
				try {
					move_construct_at(Base::get_data() + index, value);
				}
				catch (...) {
					free_stack.push(index);
					throw;
				}
			}
			else {
				index = free_top++;
				try {
					move_construct_at(Base::get_data() + index, value);
				}
				catch (...) {
					free_top--;
					throw;
				}
			}

			size++;

			return index;
		}

		constexpr void remove(INDEX index) noexcept {
			destroy_at(Base::get_data() + index);
			free_stack.push(index);
			size--;
		}

		constexpr void empty() noexcept {
			helper_destroy();
			free_stack.empty();
			size = 0;
		}

#pragma endregion

#pragma region Pool

		constexpr INDEX allocate(const T& value) noexcept(
			std::is_nothrow_copy_constructible_v<T>
		) {
			return add(value);
		}

		constexpr INDEX allocate(T&& value) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) {
			return add(std::move(value));
		}

		constexpr void deallocate(INDEX index) noexcept {
			remove(index);
		}

#pragma region Memory Management

		void double_capacity() noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
		) {
			grow(Base::get_capacity() << 1);
		}
		using Base::double_capacity;

		void grow(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
		) {
			helper_resize(newCapacity);
			free_stack.grow(newCapacity);
		}

		void reserve(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
		) {
			if (newCapacity > Base::get_capacity())
				grow(newCapacity);
		}

#pragma endregion

#pragma endregion

	};

}