#pragma once

#include "Container.hpp"

namespace DSA {

	template <concepts::StoragePolicy StoragePolicy, typename T>
		requires std::is_nothrow_destructible_v<T>
	class GapBuffer : public StoragePolicy::template STRUCTURE<T> {
	public:

		using Base = StoragePolicy::template STRUCTURE<T>;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

		INDEX end;
		INDEX start = 0;

#pragma region Methods

		void helper_destroy(Base::Capacity explicitCapacity = {}) noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			T* const data = Base::get_data();
			destroy_range_forward(data, data + start);
			destroy_range_forward(data + end, data + capacity);
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		using Base::Base;

		~GapBuffer() noexcept = default;

		~GapBuffer() noexcept
			requires concepts::HasCapacity<Base>
		{
			destroy();
		}

		constexpr void destroy(Base::Capacity explicitCapacity = {}) noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy(explicitCapacity);
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

		[[nodiscard]] constexpr INDEX get_size(Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			return capacity - get_gap_size();
		}

		[[nodiscard]] constexpr INDEX get_start() const noexcept {
			return start;
		}

#pragma endregion

#pragma region Checks

		[[nodiscard]] constexpr bool is_empty(Base::Capacity explicitCapacity = {}) const noexcept {
			return get_size(explicitCapacity) == 0;
		}

		[[nodiscard]] constexpr bool is_full(Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			return get_size(explicitCapacity) == capacity;
		}

#pragma endregion

#pragma region Add & Remove

		[[nodsicard]] constexpr bool can_add(INDEX count = 1, Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
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
			move_construct_range_forward(address, address + count, source);
			start += count;
		}

		[[nodsicard]] constexpr bool can_remove(INDEX count = 1)
			const noexcept
			requires concepts::Container<Base>
		{
			return can_remove(Base::get_capacity(), count);
		}

		[[nodsicard]] constexpr bool can_remove(INDEX count = 1, Base::Capacity explicitCapacity = {}) const noexcept {
			return get_size(explicitCapacity) >= count;
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

		void double_capacity(Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			grow(capacity << 1, explicitCapacity);
		}

		void grow(INDEX newCapacity, Base::Capacity explicitCapacity = {})
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
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			destroy_range_forward(oldData + end, oldData + capacity);

			ALLOCATOR::deallocate(oldData);

			data = reinterpret_cast<std::byte*>(newData);
			end = newCapacity - rightSize;
			if constexpr (concepts::Container<Base>)
				Base::set_capacity(newCapacity);
		}

		void reserve(INDEX newCapacity, Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			if (newCapacity > capacity)
				grow(newCapacity, explicitCapacity);
		}

#pragma endregion

#pragma endregion

	};

}
