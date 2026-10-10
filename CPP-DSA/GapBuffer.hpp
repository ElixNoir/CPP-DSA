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

#pragma region Add & Remove

		void add(const T& value) noexcept(
			std::is_nothrow_copy_constructible_v<T>
		) {
			copy_construct_at(Base::get_data() + start, value);
			start++;
		}

		void add(T&& value) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) {
			move_construct_at(Base::get_data() + start, std::move(value));
			start++;
		}

		void add_copy_construct_many(const T* source, INDEX count) noexcept(
			std::is_nothrow_copy_constructible_v<T>
		) {
			T* const address = Base::get_data() + start;
			copy_construct_range_forward(address, address + count, source);
			start += count;
		}

		void add_move_construct_many(const T* source, INDEX count) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) {
			T* const address = Base::get_data() + start;
			move_construct_range_forward(address, address + count, source);
			start += count;
		}

		void remove_end() noexcept {
			T* const address = Base::get_data() + end;
			destroy_range_forward(address, address + 1);
			end++;
		}

		void remove_end_many(INDEX count) noexcept {
			T* const address = Base::get_data() + end;
			destroy_range_forward(address, address + count);
			end += count;
		}

		void remove_start() noexcept {
			T* const address = Base::get_data() + start;
			destroy_range_forward(address - 1, address);
			start--;
		}

		void remove_start_many(INDEX count) noexcept {
			T* const address = Base::get_data() + start;
			destroy_range_forward(address - count, address);
			start -= count;
		}

#pragma endregion

#pragma region Back, Forward, & Position

		void back(INDEX position) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) {
			T* const address = Base::get_data();
			const INDEX shift = start - position;
			if constexpr (std::is_nothrow_move_constructible_v<T>) {
				end -= shift;
				start -= shift;
				move_construct_range_backward(address + end, address + end + shift, address + start);
			}
			else {
				move_construct_range_backward(address + end - shift, address + end, address + start - shift);
				end -= shift;
				start -= shift;
			}
		}

		void forward(INDEX position) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) {
			T* const address = Base::get_data();
			const INDEX shift = position - start;
			move_construct_range_forward(address + start, address + start + shift, address + end);
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

#pragma endregion

	};

}