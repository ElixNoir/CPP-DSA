#pragma once

#include "Container.hpp"

namespace DSA {

	template <concepts::StoragePolicy ContainerConfiguration, typename T>
		requires std::is_nothrow_destructible_v<T>
	class GapBuffer : public ContainerConfiguration::template STRUCTURE<T> {
	public:

		using Base = ContainerConfiguration::template STRUCTURE<T>;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

		INDEX end;
		INDEX start = 0;

#pragma region Methods

		void helper_destroy(INDEX capacity = Base::default_capacity()) noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			T* const data = Base::get_data();
			destroy_range_forward(data, data + start);
			destroy_range_forward(data + end, data + capacity);
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		using Base::Base;

		~GapBuffer() noexcept
			requires concepts::Container<Base>
		{
			destroy();
		}

		constexpr void destroy(INDEX capacity = Base::default_capacity()) noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy(capacity);
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

		[[nodiscard]] constexpr INDEX get_size(INDEX capacity = Base::default_capacity()) const noexcept {
			return capacity - get_gap_size();
		}

		[[nodiscard]] constexpr INDEX get_start() const noexcept {
			return start;
		}

#pragma endregion

#pragma region Checks

		[[nodiscard]] constexpr bool is_empty(INDEX capacity = Base::default_capacity())
			const noexcept
			requires concepts::Storage<Base>
		{
			return get_size(capacity) == 0;
		}

		[[nodiscard]] constexpr bool is_full(INDEX capacity = Base::default_capacity())
			const noexcept
			requires concepts::Storage<Base>
		{
			return get_size(capacity) == capacity;
		}

#pragma endregion

#pragma region Add & Remove

		[[nodsicard]] constexpr bool can_add(INDEX count = 1, INDEX capacity = Base::default_capacity())
			const noexcept
			requires concepts::Storage<Base>
		{
			return get_gap_size(capacity) >= count;
		}

		constexpr void add(const T& value)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			copy_construct_at(Base::get_data() + start, value);
			++start;
		}

		constexpr void add(T&& value)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			move_construct_at(Base::get_data() + start, std::move(value));
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
			move_construct_range_forward(address, address + count, source);
			start += count;
		}

		[[nodsicard]] constexpr bool can_remove(INDEX count = 1)
			const noexcept
			requires concepts::Container<Base>
		{
			return can_remove(Base::get_capacity(), count);
		}

		[[nodsicard]] constexpr bool can_remove(INDEX capacity, INDEX count = 1)
			const noexcept
			requires concepts::Storage<Base>
		{
			return get_size(capacity) >= count;
		}

		constexpr void remove_end() noexcept {
			T* const address = Base::get_data() + end;
			destroy_range_forward(address, address + 1);
			++end;
		}

		constexpr void remove_end_many(INDEX count) noexcept {
			T* const address = Base::get_data() + end;
			destroy_range_forward(address, address + count);
			end += count;
		}

		constexpr void remove_start() noexcept {
			T* const address = Base::get_data() + start;
			destroy_range_forward(address - 1, address);
			--start;
		}

		constexpr void remove_start_many(INDEX count) noexcept {
			T* const address = Base::get_data() + start;
			destroy_range_forward(address - count, address);
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
				move_construct_range_backward(address + end, address + end + shift, address + start);
			}
			else {
				move_construct_range_backward(address + end - shift, address + end, address + start - shift);
				end -= shift;
				start -= shift;
			}
		}

		constexpr void forward(INDEX position)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T* const address = Base::get_data();
			const INDEX shift = position - start;
			move_construct_range_forward(address + start, address + start + shift, address + end);
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

		void double_capacity(INDEX oldCapacity = Base::default_capacity())
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			grow(oldCapacity << 1);
		}

		void grow(INDEX newCapacity, INDEX oldCapacity = Base::default_capacity())
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			using ALLOCATOR = Base::ALLOCATOR;

			T* oldData = Base::get_data();
			T* newData = ALLOCATOR::allocate(newCapacity);

			try {
				const INDEX rightSize = oldCapacity - end;
				move_construct_range_forward(newData, newData + start, oldData);
				move_construct_range_forward(newData + newCapacity, newData + newCapacity + rightSize, oldData + end);
			}
			catch (...) {
				ALLOCATOR::deallocate(newData);
				throw;
			}
			destroy_range_forward(oldData, oldData + start);
			destroy_range_forward(oldData + end, oldData + oldCapacity);

			ALLOCATOR::deallocate(oldData);

			data = reinterpret_cast<std::byte*>(newData);
			end = newCapacity - rightSize;
			if constexpr (concepts::Container<Base>)
				Base::set_capacity(newCapacity);
		}

		void reserve(INDEX newCapacity, INDEX oldCapacity = Base::default_capacity())
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			if (newCapacity > oldCapacity)
				grow(newCapacity, oldCapacity);
		}

#pragma endregion

#pragma endregion

	};

}