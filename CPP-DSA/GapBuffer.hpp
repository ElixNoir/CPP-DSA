#pragma once

#include "Container.hpp"

namespace DSA {

	template <concepts::ContainerPolicy ContainerPolicy, typename T>
		requires std::is_nothrow_destructible_v<T>
	class GapBuffer : public ContainerPolicy::template STRUCTURE<T> { //! incomplete
	public:

		using Base = ContainerPolicy::template STRUCTURE<T>;
		using typename Base::ALLOCATOR;
		using typename Base::INDEX;
		using typename Base::T;

		constexpr static bool is_dynamic = Base::is_dynamic;

	protected:

#pragma region Properties

		INDEX end = 0;
		INDEX start = 0;

#pragma endregion

#pragma region Methods

		constexpr void helper_resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires is_dynamic
		{
			T* const oldData = Base::get_data();
			T* const newData = ALLOCATOR::allocate(newCapacity);

			try {
				const INDEX rightSize = oldCapacity - end;
				move_construct_range(newData, newData + start, oldData);
				move_construct_range(newData + newCapacity, newData + newCapacity + rightSize, oldData + end);
			}
			catch (...) {
				ALLOCATOR::deallocate(newData);
				throw;
			}
			destroy_range(oldData, oldData + start);
			const INDEX capacity = Base::get_capacity();
			destroy_range(oldData + end, oldData + capacity);

			ALLOCATOR::deallocate(oldData);

			data = reinterpret_cast<std::byte*>(newData);
			end = newCapacity - rightSize;
			if constexpr (concepts::Container<Base>)
				Base::set_capacity(newCapacity);
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		using Base::Base;

		~GapBuffer() noexcept = default;

		~GapBuffer() noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			destroy();
		}

		constexpr void destroy() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>) {
				const INDEX capacity = Base::get_capacity();
				T* const data = Base::get_data();
				destroy_range(data, data + start);
				destroy_range(data + end, data + capacity);
			}
		}

#pragma endregion

#pragma region Methods

#pragma region Getters

		[[nodiscard]] constexpr INDEX get_end() const noexcept {
			return end;
		}

		[[nodiscard]] constexpr INDEX get_gap_size() const noexcept {
			return end - start;
		}

		[[nodiscard]] constexpr INDEX get_size() const noexcept {
			const INDEX capacity = Base::get_capacity();
			return capacity - get_gap_size();
		}

		[[nodiscard]] constexpr INDEX get_start() const noexcept {
			return start;
		}

#pragma endregion

#pragma region Checks

		[[nodiscard]] constexpr bool is_empty() const noexcept {
			return get_size(capacity) == 0;
		}

		[[nodiscard]] constexpr bool is_full() const noexcept {
			const INDEX capacity = Base::get_capacity();
			return get_size(capacity) == capacity;
		}

#pragma endregion

#pragma region Add & Remove

		[[nodsicard]] constexpr bool can_add(INDEX count = 1) const noexcept {
			const INDEX capacity = Base::get_capacity();
			return get_gap_size(capacity) >= count;
		}

		constexpr void add(const T& data)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			copy_construct_at(Base::get_data() + start, data);
			++start;
		}

		constexpr void add(T&& data)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			move_construct_at(Base::get_data() + start, std::move(data));
			++start;
		}

		constexpr void add_copy_construct_many(const T* source, INDEX count)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			T* const address = Base::get_data() + start;
			copy_construct_range_forward(address, address + count, source);
			start += count;
		}

		constexpr void add_move_construct_many(const T* source, INDEX count)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T* const address = Base::get_data() + start;
			move_construct_range(address, address + count, source);
			start += count;
		}

		[[nodsicard]] constexpr bool can_remove(INDEX count = 1)
			const noexcept
			requires concepts::Container<Base>
		{
			return can_remove(Base::get_capacity(), count);
		}

		[[nodsicard]] constexpr bool can_remove(INDEX count = 1) const noexcept {
			return get_size(capacity) >= count;
		}

		constexpr void remove_end() noexcept {
			T* const address = Base::get_data() + end;
			destroy_range(address, address + 1);
			++end;
		}

		constexpr void remove_end_many(INDEX count) noexcept {
			T* const address = Base::get_data() + end;
			destroy_range(address, address + count);
			end += count;
		}

		constexpr void remove_start() noexcept {
			T* const address = Base::get_data() + start;
			destroy_range(address - 1, address);
			--start;
		}

		constexpr void remove_start_many(INDEX count) noexcept {
			T* const address = Base::get_data() + start;
			destroy_range(address - count, address);
			start -= count;
		}

#pragma endregion

#pragma region Back, Forward, & Position

		constexpr void back(INDEX position) 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T* const address = Base::get_data();
			const INDEX shift = start - position;
			if constexpr (std::is_nothrow_move_constructible_v<T>) {
				end -= shift;
				start -= shift;
				reconstruct_range<Direction::Backward>(address + end, address + end + shift, address + start);
			}
			else {
				move_construct_range<Direction::Backward>(address + end - shift, address + end, address + start - shift);
				destroy_range(address + start - shift, address + start - shift);
				end -= shift;
				start -= shift;
			}
		}

		constexpr void forward(INDEX position)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T* const address = Base::get_data();
			const INDEX shift = position - start;
			move_construct_range(address + start, address + start + shift, address + end); // ????
			end += shift;
			start += shift;
		}

		constexpr void position(INDEX position)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			if (position < start)
				back(position);
			else if (position > start)
				forward(position);
		}

#pragma endregion

#pragma region Memory Management

		constexpr void double_capacity()
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			grow(capacity << 1, capacity);
		}

		constexpr void grow(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires is_dynamic
		{
			helper_resize(newCapacity);
		}

		constexpr void reserve(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity > capacity)
				grow(newCapacity);
		}

#pragma endregion

#pragma endregion

	};

}
