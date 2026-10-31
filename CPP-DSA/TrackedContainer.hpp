#pragma once

#include "Bitset.hpp"
#include "IncrementalContainer.hpp"

namespace DSA {

	template <typename ContainerConfiguration, typename T>
		requires std::is_nothrow_destructible_v<T>
	class TrackedContainer : public IncrementalContainer<ContainerConfiguration, T> {
	public:

		using Base = IncrementalContainer<ContainerConfiguration, T>;
		using typename Base::INDEX;
		using typename Base::T;
		using TRACKED = Bitset<
			std::conditional_t<
				concepts::DynamicContainer<Base>,
				AlternateDynamicContainerConfiguration<INDEX, typename Base::ALLOCATOR>,
				StaticContainerConfiguration<((Base::maximum_capacity + Bitmask<>::BitCount - 1) >> Bitmask<>::BitShift)>
			>
		>;

		TRACKED tracked;

	protected:

#pragma region Methods

		void helper_destroy()
			noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			INDEX count = Base::get_size();

			for (INDEX index : tracked.ones()) {
				if (count == 0)
					break;
				destroy_at(Base::get_data() + index);
				count--;
			}
		}

		template <bool GROW>
		void helper_resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableAllocator<Base::ALLOCATOR>
				&& std::is_nothrow_move_constructible_v<T>)
			requires concepts::DynamicContainer<Base>
		{
			using ALLOCATOR = Base::ALLOCATOR;

			const INDEX size = Base::get_size();

			T* const oldData = Base::get_data();
			T* const newData = ALLOCATOR::allocate(newCapacity);

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

			Base::data = reinterpret_cast<std::byte*>(newData);
			Base::capacity = newCapacity;

			if constexpr (GROW)
				tracked.grow(((newCapacity + Bitmask<>::BitCount - 1) >> Bitmask<>::BitShift), ((Base::get_capacity() + Bitmask<>::BitCount - 1) >> Bitmask<>::BitShift));
			else
				tracked.shrink(((newCapacity + Bitmask<>::BitCount - 1) >> Bitmask<>::BitShift));
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		TrackedContainer(INDEX initialCapacity)
			requires (concepts::DynamicContainer<Base>) :
			Base(initialCapacity),
			tracked((initialCapacity + Bitmask<>::BitCount - 1) >> Bitmask<>::BitShift) {}

		TrackedContainer() noexcept = default;

		~TrackedContainer() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy();
		}

#pragma endregion

#pragma region Methods

#pragma region Tracking

		[[nodiscard]] constexpr bool contains_tracked_index(INDEX index) const noexcept {
			return index < Base::get_capacity() && tracked.get_bit_at(index);
		}

		[[nodiscard]] constexpr INDEX find_first_tracked(INDEX start) const noexcept {
			return tracked.find_first_bit<true, true, true>(start);
		}

		[[nodiscard]] constexpr INDEX find_last_tracked(INDEX start) const noexcept {
			return tracked.find_first_bit<false, true, true>(start);
		}

		[[nodiscard]] constexpr bool is_tracking(INDEX index) const noexcept {
			return tracked.get_bit_at(index);
		}

		[[nodiscard]] constexpr bool track(INDEX index) const noexcept {
			return tracked.set_bit_at(index);
		}

		[[nodiscard]] constexpr bool untrack(INDEX index) const noexcept {
			return tracked.reset_bit_at(index);
		}

#pragma endregion

#pragma region Memory Management

		void double_capacity()
			noexcept (concepts::NothrowResizableAllocator<Base::ALLOCATOR>
				&& std::is_nothrow_move_constructible_v<T>)
			requires concepts::DynamicContainer<Base>
		{
			if constexpr (concepts::ResizableContainer<Base>)
				Base::double_capacity();
			else
				grow(Base::get_capacity() << 1);
		}

		constexpr void empty() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy();
			tracked.zero();
			Base::set_size(0);
		}

		void grow(INDEX newCapacity)
			noexcept (concepts::NothrowResizableAllocator<Base::ALLOCATOR>
				&& std::is_nothrow_move_constructible_v<T>)
			requires concepts::DynamicContainer<Base>
		{
			if constexpr (concepts::ResizableContainer<Base>)
				Base::grow(newCapacity);
			else
				helper_resize<true>(newCapacity);
		}

		void reserve(INDEX newCapacity)
			noexcept (concepts::NothrowResizableAllocator<Base::ALLOCATOR>
				&& std::is_nothrow_move_constructible_v<T>)
			requires concepts::DynamicContainer<Base>
		{
			if constexpr (concepts::ResizableContainer<Base>)
				Base::reserve(newCapacity);
			else if (newCapacity > Base::get_capacity())
				grow(newCapacity);
		}

		void resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableAllocator<Base::ALLOCATOR>
				&& std::is_nothrow_move_constructible_v<T>)
			requires concepts::DynamicContainer<Base>
		{
			if constexpr (concepts::ResizableContainer<Base>)
				Base::resize(newCapacity);
			else if (newCapacity > Base::get_capacity())
				grow(newCapacity);
			else
				shrink(newCapacity);
		}

		void shrink(INDEX newCapacity)
			noexcept (concepts::NothrowResizableAllocator<Base::ALLOCATOR>
				&& std::is_nothrow_move_constructible_v<T>)
			requires concepts::DynamicContainer<Base>
		{
			if constexpr (concepts::ResizableContainer<Base>)
				Base::shrink(newCapacity);
			else
				helper_resize<false>(newCapacity);
		}

#pragma endregion

#pragma endregion

	};

}