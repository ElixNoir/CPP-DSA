#pragma once

#include "Bitset.hpp"
#include "IncrementalContainer.hpp"

namespace DSA {

	template <concepts::Container CONTAINER>
	class TrackedContainer : public IncrementalContainer<CONTAINER> {
	public:

		using Base = IncrementalContainer<CONTAINER>;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

		using Base::data;
		using Base::size;

		Bitset<CONTAINER> tracked;

#pragma region Methods

		void helper_destroy() noexcept requires (
			!std::is_trivially_destructible_v<T>
		) {
			INDEX count = size;

			for (INDEX index : tracked.ones()) {
				if (count == 0)
					break;
				destroy_at(Base::get_data() + index);
				count--;
			}
		}

		template <bool GROW>
		void helper_resize(INDEX newCapacity) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) {
			using ALLOCATOR = CONTAINER::ALLOCATOR;

			T* oldData = Base::get_data();
			T* newData = ALLOCATOR::allocate(newCapacity);

			INDEX count = size;

			try {
				for (INDEX index : tracked.ones()) {
					if (count == 0)
						break;
					move_construct_at(newData + index, std::move(oldData[index]));
					count--;
				}
			}
			catch (...) {
				if constexpr (!std::is_trivially_destructible_v<T>) {
					for (INDEX index : tracked.ones()) {
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
				for (INDEX index : tracked.ones()) {
					if (count == size)
						break;

					destroy_at(oldData + index);
					count++;
				}
			}

			ALLOCATOR::deallocate(oldData);

			data = reinterpret_cast<std::byte*>(newData);
			Base::capacity = newCapacity;

			if constexpr (GROW)
				tracked.grow((newCapacity + Bitmask<uintmax_t>::BitCount - 1) >> Bitmask<uintmax_t>::BitShift);
			else
				tracked.shrink((newCapacity + Bitmask<uintmax_t>::BitCount - 1) >> Bitmask<uintmax_t>::BitShift);
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		TrackedContainer(INDEX initialCapacity) requires (
			concepts::DynamicContainer<CONTAINER>
		) :
			Base(initialCapacity),
			tracked((initialCapacity + Bitmask<uintmax_t>::BitCount - 1) >> Bitmask<uintmax_t>::BitShift) {}

		TrackedContainer() = default;

		~TrackedContainer() requires (
			!std::is_trivially_destructible_v<T>
		) {
			helper_destroy();
		}

		~TrackedContainer() = default;

#pragma endregion

#pragma region Methods

#pragma region Find & Track

		constexpr INDEX find_occupied(INDEX index = 0) const noexcept {
			const INDEX capacity = Base::get_capacity();
			while (index < capacity) {
				int bitIndex = tracked.get(index >> Bitmask<uintmax_t>::BitShift).index_of_trailing_one();;
				if (bitIndex != 64)
					return index + bitIndex;
				index++;
			}
			return capacity;
		}

		constexpr INDEX find_unoccupied(INDEX index = 0) const noexcept {
			const INDEX capacity = Base::get_capacity();
			while (index < capacity) {
				int bitIndex = tracked.get(index >> Bitmask<uintmax_t>::BitShift).index_of_trailing_zero();;
				if (bitIndex != 64)
					return index + bitIndex;
				index++;
			}
			return capacity;
		}

		constexpr bool get_tracked(INDEX index) const noexcept {
			return tracked.get_bit_at(index);
		}

		constexpr void set_tracked(INDEX index) noexcept {
			tracked.set_bit_at(index);
		}

#pragma endregion

#pragma region IncrementalContainer

		constexpr void empty() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy();
			tracked.zero();
			size = 0;
		}

#pragma endregion

#pragma region Memory Management

		void double_capacity() noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
		) {
			if constexpr (concepts::ResizableContainer<CONTAINER>)
				Base::double_capacity();
			else
				grow(Base::get_capacity() << 1);
		}

		void grow(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
		) {
			if constexpr (concepts::ResizableContainer<CONTAINER>)
				Base::grow(newCapacity);
			else
				helper_resize<true>(newCapacity);
		}

		void reserve(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
		) {
			if constexpr (concepts::ResizableContainer<CONTAINER>)
				Base::reserve(newCapacity);
			else if (newCapacity > Base::get_capacity())
				grow(newCapacity);
		}

		void resize(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
		) {
			if constexpr (concepts::ResizableContainer<CONTAINER>)
				Base::resize(newCapacity);
			else if (newCapacity > Base::get_capacity())
				grow(newCapacity);
			else
				shrink(newCapacity);
		}

		void shrink(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
		) {
			if constexpr (concepts::ResizableContainer<CONTAINER>)
				Base::shrink(newCapacity);
			else
				helper_resize<false>(newCapacity);
		}

#pragma endregion

#pragma endregion

	};

}