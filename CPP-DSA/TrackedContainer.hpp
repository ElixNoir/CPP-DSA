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
					move_construct_at(newData + index, move(oldData[index]));
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

		TrackedContainer(INDEX initialCapacity) :
			Base(initialCapacity),
			tracked((initialCapacity + Bitmask<uintmax_t>::BitCount - 1) >> Bitmask<uintmax_t>::BitShift) {}

		~TrackedContainer() requires (
			!std::is_trivially_destructible_v<T>
		) {
			helper_destroy();
		}

		~TrackedContainer() = default;

#pragma endregion

#pragma region Methods

#pragma region Track

		constexpr void track(INDEX index) noexcept {
			tracked[index >> Bitmask<uintmax_t>::BitShift].set(index & (Bitmask<uintmax_t>::BitCount - 1));
		}

#pragma endregion

#pragma region IncrementalContainer

		constexpr void empty() noexcept {
			helper_destroy();
			tracked.empty();
			size = 0;
		}

#pragma endregion

#pragma region Memory Management

		void double_capacity() noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
			&& !concepts::ResizableContainer<CONTAINER>
		) {
			grow(capacity << 1);
		}
		using Base::double_capacity;

		void grow(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
			&& !concepts::ResizableContainer<CONTAINER>
		) {
			helper_resize<true>(newCapacity);
		}
		using Base::grow;

		void reserve(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
			&& !concepts::ResizableContainer<CONTAINER>
		) {
			if (newCapacity > capacity)
				grow(newCapacity);
		}
		using Base::reserve;

		void resize(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
			&& !concepts::ResizableContainer<CONTAINER>
		) {
			if (newCapacity > capacity)
				grow(newCapacity);
			else
				shrink(newCapacity);
		}
		using Base::resize;

		void shrink(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
			&& !concepts::ResizableContainer<CONTAINER>
		) {
			helper_resize<false>(newCapacity);
		}
		using Base::shrink;

#pragma endregion

#pragma endregion

	};

}