#pragma once

#include "Container.hpp"

namespace DSA {

	template <concepts::Container CONTAINER>
		requires std::is_nothrow_destructible_v<typename CONTAINER::T>
	class GapBuffer : public CONTAINER {
	public:

		using Base = CONTAINER;
		using typename Base::INDEX;
		using typename Base::T;

	private:

		using Base::resize;
		using Base::shrink;

	protected:

		using Base::data;

		INDEX end;
		INDEX start = 0;

	public:

#pragma region Methods

#pragma region GapBuffer

		void back(INDEX position) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) {
			const INDEX shift = start - position;
			end -= shift;
			start -= shift;
			move_construct_range_backward(data + end, data + start, shift);
		}

		void forward(INDEX position) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) {
			const INDEX shift = position - start;
			move_construct_range_forward(data + start, data + end, shift);
			end += shift;
			start += shift;
		}

		void position(INDEX position) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) {
			if (position < start)
				back(position);
			else if (position > start)
				forward(position);
		}

		void add(const T& value) noexcept(
			std::is_nothrow_copy_constructible_v<T>
		) {
			copy_construct_at(data + start++, value);
		}

		void add(T&& value) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) {
			move_construct_at(data + start++, move(value));
		}

		void add_copy_construct_many(const T* source, INDEX count) noexcept(
			std::is_nothrow_copy_constructible_v<T>
		) {
			copy_construct_range_forward(data + start, source, count);
			start += count;
		}

		void add_move_construct_many(const T* source, INDEX count) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) {
			move_construct_range_forward(data + start, source, count);
			start += count;
		}

		void remove_end() noexcept {
			T* const address = Base::get_data() + ++end;
			destroy_range_forward(address - 1, address);
		}

		void remove_end_many(INDEX count) noexcept {
			end += count;
			T* const address = Base::get_data() + end;
			destroy_range_forward(address - count, address);
		}

		void remove_start() noexcept {
			T* const address = Base::get_data() + --start;
			destroy_range_forward(address, address + 1);
		}

		void remove_start_many(INDEX count) noexcept {
			start -= count;
			T* const address = Base::get_data() + start;
			destroy_range_forward(address, address + count);
		}

#pragma region Checks

		[[nodsicard]] constexpr bool can_add(INDEX count = 1) const noexcept {
			return gap_size() >= count;
		}

		[[nodsicard]] constexpr bool can_remove(INDEX count = 1) const noexcept {
			return size() >= count;
		}

		[[nodiscard]] constexpr bool is_empty() const noexcept {
			return size() == 0;
		}

		[[nodiscard]] constexpr bool is_full() const noexcept {
			return size() == Base::get_capacity();
		}

#pragma endregion

#pragma region Getters

		[[nodiscard]] constexpr T* data() const noexcept {
			return data;
		}

		[[nodiscard]] constexpr INDEX end() const noexcept {
			return end;
		}

		[[nodiscard]] constexpr INDEX gap_size() const noexcept {
			return end - start;
		}

		[[nodiscard]] constexpr INDEX size() const noexcept {
			return Base::get_capacity() - gap_size();
		}

		[[nodiscard]] constexpr INDEX start() const noexcept {
			return start;
		}

#pragma endregion

#pragma region Helpers

		//constexpr void empty() noexcept {
			
		//}

#pragma endregion

#pragma region Memory Management

		void double_capacity() noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
		) {
			grow(end << 1);
		}

		void grow(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
		) {
			using ALLOCATOR = CONTAINER::ALLOCATOR;

			T* oldData = Base::get_data();
			T* newData = ALLOCATOR::allocate(newCapacity);

			try {
				const INDEX rightSize = Base::get_capacity() - end;
				move_construct_range_forward(newData, oldData, start);
				move_construct_range_forward(newData + newCapacity - rightSize, oldData + end, rightSize);
			}
			catch (...) {
				ALLOCATOR::deallocate(newData);
				throw;
			}
			destroy_range_forward(oldData, oldData + start);
			destroy_range_forward(oldData + end, oldData + Base::get_capacity());

			ALLOCATOR::deallocate(oldData);

			data = reinterpret_cast<std::byte*>(newData);
			end = newCapacity - rightSize;
			Base::capacity = newCapacity;
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

#pragma region Setters

#pragma endregion

#pragma endregion

#pragma endregion

	};

}