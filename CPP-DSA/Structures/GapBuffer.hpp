#pragma once

#include "Container.hpp"

namespace DSA {

	template <concepts::ContainerPolicy ContainerPolicy, typename T_>
		requires std::is_nothrow_destructible_v<T_>
	class GapBuffer : public ContainerPolicy::template STRUCTURE<T_> {
	public:

		using Base = ContainerPolicy::template STRUCTURE<T_>;
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

		template <ResizeOperation RESIZE_OPERATION>
			requires (RESIZE_OPERATION != ResizeOperation::UNKNOWN)
		constexpr void helper_resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			const INDEX rightSize = capacity - end;

			if constexpr (concepts::ReallocatableStorage<Base>) {
				T* const newData = ALLOCATOR::reallocate(Base::get_data(), newCapacity);
				if (!newData)
					return;

				const INDEX newEnd = newCapacity - rightSize;

				if (rightSize)
					std::memmove(
						newData + newEnd,
						newData + end,
						rightSize * sizeof(T)
					);

				end = newEnd;
				Base::set_capacity(newCapacity);

				return;
			}

			T* const oldData = Base::get_data();
			T* const newData = ALLOCATOR::allocate(newCapacity);

			if constexpr (std::is_nothrow_move_constructible_v<T>) {
				reconstruct_range(
					newData,
					newData + start,
					oldData
				);
				reconstruct_range(
					newData + newCapacity - rightSize,
					newData + newCapacity,
					oldData + end
				);
			}
			else {
				try {
					move_construct_range(
						newData,
						newData + start,
						oldData
					);
				}
				catch (...) {
					ALLOCATOR::deallocate(newData);
					throw;
				}

				try {
					move_construct_range(
						newData + newCapacity - rightSize,
						newData + newCapacity,
						oldData + end
					);
				}
				catch (...) {
					destroy_range(newData, newData + start);
					ALLOCATOR::deallocate(newData);
					throw;
				}

				destroy_range(oldData, oldData + start);
				destroy_range(oldData + end, oldData + capacity);
			}

			ALLOCATOR::deallocate(oldData);

			end = newCapacity - rightSize;
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

		[[nodiscard]] constexpr INDEX get_gap_size() const noexcept {
			return end - start;
		}

		[[nodiscard]] constexpr INDEX get_size() const noexcept {
			return Base::get_capacity() - get_gap_size();
		}

#pragma endregion

#pragma region Checks

		[[nodiscard]] constexpr bool is_empty() const noexcept {
			return get_size() == 0;
		}

		[[nodiscard]] constexpr bool is_full() const noexcept {
			return get_gap_size() == 0;
		}

#pragma endregion

#pragma region Add & Remove

		[[nodiscard]] constexpr bool can_add(INDEX count = 1) const noexcept {
			return get_gap_size() >= count;
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
			T* const data = Base::get_data() + start;
			copy_construct_range_forward(data, data + count, source);
			start += count;
		}

		constexpr void add_move_construct_many(const T* source, INDEX count)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T* const data = Base::get_data() + start;
			move_construct_range(data, data + count, source);
			start += count;
		}

		[[nodiscard]] constexpr bool can_remove(INDEX count = 1) const noexcept {
			return get_size() >= count;
		}

		constexpr void remove_end() noexcept {
			T* const data = Base::get_data() + end;
			destroy_range(data, data + 1);
			++end;
		}

		constexpr void remove_end_many(INDEX count) noexcept {
			T* const data = Base::get_data() + end;
			destroy_range(data, data + count);
			end += count;
		}

		constexpr void remove_start() noexcept {
			T* const data = Base::get_data() + start;
			destroy_range(data - 1, data);
			--start;
		}

		constexpr void remove_start_many(INDEX count) noexcept {
			T* const data = Base::get_data() + start;
			destroy_range(data - count, data);
			start -= count;
		}

#pragma endregion

#pragma region Back, Forward, & Position

		constexpr void back(INDEX position)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T* const data = Base::get_data();

			const INDEX shift = start - position;
			if (shift == 0)
				return;

			if constexpr (std::is_nothrow_move_constructible_v<T>) {
				start -= shift;
				end -= shift;
				reconstruct_range<Direction::Backward>(
					data + end,
					data + end + shift,
					data + start
				);
			}
			else {
				move_construct_range<Direction::Backward>(
					data + end - shift,
					data + end,
					data + start - shift
				);
				destroy_range(data + start - shift, data + start);
				start -= shift;
				end -= shift;
			}
		}

		constexpr void forward(INDEX position)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T* const data = Base::get_data();

			const INDEX shift = position - start;
			if (shift == 0)
				return;

			if constexpr (std::is_nothrow_move_constructible_v<T>) {
				reconstruct_range<Direction::Forward>(
					data + start,
					data + start + shift,
					data + end
				);
			}
			else {
				move_construct_range<Direction::Forward>(
					data + start,
					data + start + shift,
					data + end
				);
				destroy_range(data + end, data + end + shift);
			}

			start += shift;
			end += shift;
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
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			grow(capacity << 1, capacity);
		}

		constexpr void grow(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			helper_resize<ResizeOperation::GROW>(newCapacity);
		}

		constexpr void reserve(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity > capacity)
				grow(newCapacity);
		}

		constexpr void resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity >= capacity)
				grow(newCapacity);
			else
				shrink(newCapacity);
		}

		constexpr void shrink(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			helper_resize<ResizeOperation::SHRINK>(newCapacity);
		}

		constexpr void shrink_to_fit()
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			shrink(get_size());
		}

#pragma endregion

#pragma endregion

	};

}
