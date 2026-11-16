#pragma once

#include "Bitset.hpp"
#include "Collection.hpp"

namespace DSA {

	template <concepts::ContainerPolicy ContainerPolicy, typename T>
		requires std::is_nothrow_destructible_v<T>
	class TrackedContainer : public Collection<ContainerPolicy, T> {
	public:

		using Base = Collection<ContainerPolicy, T>;
		using typename Base::ALLOCATOR;
		using typename Base::INDEX;
		using typename Base::T;

		constexpr static auto bit_count = Bitmask<>::bit_count;
		constexpr static auto bit_shift = Bitmask<>::bit_shift;

		using TRACKED = Bitset<
			std::conditional_t<
				ContainerPolicy::is_dynamic,
				HeapStoragePolicy<INDEX, typename ContainerPolicy::ALLOCATOR_TEMPLATE>,
				InlineStoragePolicy<((Base::maximum_index + bit_count - 1) >> bit_shift)>
			>
		>;

		TRACKED tracked;

	protected:

#pragma region Methods

		template <ResizeOperation RESIZE_OPERATION>
			requires (RESIZE_OPERATION != ResizeOperation::UNKNOWN)
		constexpr void helper_resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::get_capacity();
			const INDEX size = Base::get_size();

			T* const oldData = Base::get_data();
			T* const newData = ALLOCATOR::allocate(newCapacity);

			INDEX count;
			if constexpr (RESIZE_OPERATION == ResizeOperation::GROW)
				count = size;
			else
				count = Aggregation::minimum(newCapacity, size);
			const INDEX newSize = count;

			if constexpr (std::is_nothrow_move_constructible_v<T>) {
				for (INDEX index : tracked.ones()) {
					if (count == 0)
						break;
					reconstruct_at(newData + index, std::move(oldData[index]));
					--count;
				}
			}
			else {
				try {
					for (INDEX index : tracked.ones()) {
						if (count == 0)
							break;
						move_construct_at(newData + index, std::move(oldData[index]));
						--count;
					}
				}
				catch (...) {
					if constexpr (!std::is_trivially_destructible_v<T>) {
						for (INDEX index : tracked.ones()) {
							if (count == newSize)
								break;
							destroy_at(newData + index);
							++count;
						}
					}
					ALLOCATOR::deallocate(newData);
					throw;
				}

				if constexpr (!std::is_trivially_destructible_v<T>) {
					for (INDEX index : tracked.ones()) {
						if (count == newSize)
							break;
						destroy_at(oldData + index);
						++count;
					}
				}
			}

			ALLOCATOR::deallocate(oldData);

			Base::set_capacity(newCapacity);
			Base::set_data(newData);

			if constexpr (RESIZE_OPERATION == ResizeOperation::GROW)
				tracked.grow(get_bitset_capacity(newCapacity));
			else {
				Base::set_size(newSize);
				tracked.shrink(get_bitset_capacity(newCapacity));
			}
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		TrackedContainer() noexcept = default;

		TrackedContainer(INDEX initialCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires (concepts::DynamicStorage<Base>)
		: Base(initialCapacity), tracked((initialCapacity + Bitmask<>::bit_count - 1) >> Bitmask<>::bit_shift) {}

		~TrackedContainer() noexcept = default;

		~TrackedContainer() noexcept
			requires concepts::Container<Base>
		{
			destroy();
		}

		constexpr void destroy() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>) {
				const INDEX capacity = Base::get_capacity();
				INDEX count = Base::get_size();
				for (INDEX index : tracked.ones()) {
					if (count == 0)
						break;
					destroy_at(Base::get_data() + index);
					--count;
				}
			}
		}

#pragma endregion

#pragma region Methods

#pragma region Getters

		[[nodiscard]] constexpr INDEX get_bitset_capacity() const noexcept {
			const INDEX capacity = Base::get_capacity();
			return (capacity + bit_count - 1) >> bit_shift;
		}

#pragma endregion

#pragma region Tracking

		[[nodiscard]] constexpr INDEX find_first_tracked() const noexcept {
			return tracked.find_first_bit<true>();
		}

		[[nodiscard]] constexpr INDEX find_first_tracked_at_or_after(INDEX begin) const noexcept {
			return tracked.find_first_bit_at_or_after<true>(begin);
		}

		[[nodiscard]] constexpr INDEX find_last_tracked() const noexcept {
			return tracked.find_last_bit<true>();
		}

		[[nodiscard]] constexpr INDEX find_last_tracked_at_or_before(INDEX end) const noexcept {
			return tracked.find_last_bit_at_or_before<true>(end);
		}

		[[nodiscard]] constexpr INDEX find_first_untracked() const noexcept {
			return tracked.find_first_bit<false>();
		}

		[[nodiscard]] constexpr INDEX find_first_untracked_at_or_after(INDEX begin) const noexcept {
			return tracked.find_first_bit_at_or_after<false>(begin);
		}

		[[nodiscard]] constexpr INDEX find_last_untracked() const noexcept {
			return tracked.find_last_bit<false>();
		}

		[[nodiscard]] constexpr INDEX find_last_untracked_at_or_before(INDEX end) const noexcept {
			return tracked.find_last_bit_at_or_before<false>(end);
		}

		[[nodiscard]] constexpr bool is_tracking(INDEX index) const noexcept {
			return tracked.get_bit_at(index);
		}

		[[nodiscard]] constexpr void track(INDEX index) noexcept {
			tracked.set_bit_at(index);
		}

		[[nodiscard]] constexpr void untrack(INDEX index) noexcept {
			tracked.reset_bit_at(index);
		}

#pragma endregion

#pragma region Memory Management

		void double_capacity()
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::get_capacity();
			grow(capacity << 1);
		}

		constexpr void empty() noexcept {
			const INDEX capacity = Base::get_capacity();
			destroy(capacity);
			tracked.zero_raw(capacity);
			Base::set_size(0);
		}

		void expand(float ratio = 1.5)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::get_capacity();
			grow(Math::ceiling(capacity * ratio), capacity);
		}

		void grow(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::get_capacity();
			helper_resize<ResizeOperation::GROW>(newCapacity);
		}

		void reserve(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity > capacity)
				grow(newCapacity);
		}

		void resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity >= capacity)
				grow(newCapacity);
			else
				shrink(newCapacity);
		}

		void shrink(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::get_capacity();
			helper_resize<ResizeOperation::SHRINK>(newCapacity);
		}

#pragma endregion

#pragma endregion

	};

}
