#pragma once

#include "Bitset.hpp"
#include "Collection.hpp"

namespace DSA {

	template <concepts::StoragePolicy StoragePolicy, typename T>
		requires std::is_nothrow_destructible_v<T>
	class TrackedContainer : public Collection<StoragePolicy, T> {
	public:

		using Base = Collection<StoragePolicy, T>;
		using typename Base::INDEX;
		using typename Base::T;

		constexpr static auto BitCount = Bitmask<>::BitCount;
		constexpr static auto BitShift = Bitmask<>::BitShift;

		using TRACKED = Bitset<
			std::conditional_t<
				StoragePolicy::is_dynamic,
				HeapStoragePolicy<INDEX, typename StoragePolicy::ALLOCATOR_TEMPLATE>,
				InlineStoragePolicy<((Base::maximum_index + BitCount - 1) >> BitShift)>
			>
		>;

		TRACKED tracked;

	protected:

#pragma region Methods

		void helper_destroy(Base::Capacity explicitCapacity = {}) noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			INDEX count = Base::get_size();
			for (INDEX index : tracked.ones(get_bitset_capacity(capacity))) {
				if (count == 0)
					break;
				destroy_at(Base::get_data() + index);
				--count;
			}
		}

		template <bool GROW>
		void helper_resize(INDEX newCapacity, Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			using ALLOCATOR = Base::ALLOCATOR;

			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			const INDEX oldBitsetCapacity = get_bitset_capacity(capacity);
			const INDEX size = Base::get_size();

			T* const oldData = Base::get_data();
			T* const newData = ALLOCATOR::allocate(newCapacity);

			INDEX count = size;

			try {
				for (INDEX index : tracked.ones(oldBitsetCapacity)) {
					if (count == 0)
						break;
					move_construct_at(newData + index, std::move(oldData[index]));
					--count;
				}
			}
			catch (...) {
				if constexpr (!std::is_trivially_destructible_v<T>) {
					for (INDEX index : tracked.ones(oldBitsetCapacity)) {
						if (count == size)
							break;
						destroy_at(newData + index);
						++count;
					}
				}
				ALLOCATOR::deallocate(newData);
				throw;
			}

			if constexpr (!std::is_trivially_destructible_v<T>) {
				for (INDEX index : tracked.ones(oldBitsetCapacity)) {
					if (count == size)
						break;
					destroy_at(oldData + index);
					++count;
				}
			}

			ALLOCATOR::deallocate(oldData);

			Base::set_data(reinterpret_cast<std::byte*>(newData));
			Base::set_capacity(newCapacity);

			if constexpr (GROW)
				tracked.grow(get_bitset_capacity(newCapacity), oldBitsetCapacity);
			else
				tracked.shrink(get_bitset_capacity(newCapacity));
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		TrackedContainer() noexcept = default;

		TrackedContainer(INDEX initialCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires (concepts::DynamicStorage<Base>)
		: Base(initialCapacity), tracked((initialCapacity + Bitmask<>::BitCount - 1) >> Bitmask<>::BitShift) {}

		~TrackedContainer() noexcept
			requires concepts::Container<Base>
		{
			destroy();
		}

		~TrackedContainer() noexcept = default;

		void destroy(Base::Capacity explicitCapacity = {}) noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy(explicitCapacity);
		}

#pragma endregion

#pragma region Methods

#pragma region Getters

		[[nodiscard]] constexpr INDEX get_bitset_capacity(Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			return (capacity + BitCount - 1) >> BitShift;
		}

#pragma endregion

#pragma region Tracking

		[[nodiscard]] constexpr bool contains_tracked_index(INDEX index, Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			return index < capacity && tracked.get_bit_at(index);
		}

		[[nodiscard]] constexpr INDEX find_first_tracked(Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			return tracked.find_first_bit<true, true, true>(capacity);
		}

		[[nodiscard]] constexpr INDEX find_first_tracked_after(INDEX start, Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			return tracked.find_first_bit_after<true, true, true>(start, capacity);
		}

		[[nodiscard]] constexpr INDEX find_last_tracked(Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			return tracked.find_first_bit<false, false, true>(capacity);
		}

		[[nodiscard]] constexpr INDEX find_last_tracked_after(INDEX start, Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			return tracked.find_first_bit_after<false, false, true>(start, capacity);
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

		void double_capacity(Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			grow(capacity << 1, explicitCapacity);
		}

		constexpr void empty(Base::Capacity explicitCapacity = {}) noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			destroy(explicitCapacity);
			tracked.zero_raw(explicitCapacity);
			Base::set_size(0);
		}

		void expand(float ratio = 1.5, Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			grow(capacity * ratio + 1, explicitCapacity);
		}

		void grow(INDEX newCapacity, Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			helper_resize<true>(newCapacity, explicitCapacity);
		}

		void reserve(INDEX newCapacity, Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			if (newCapacity > capacity)
				grow(newCapacity, explicitCapacity);
		}

		void resize(INDEX newCapacity, Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			if (newCapacity > capacity)
				grow(newCapacity, explicitCapacity);
			else
				shrink(newCapacity, explicitCapacity);
		}

		void shrink(INDEX newCapacity, Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			helper_resize<false>(newCapacity, explicitCapacity);
		}

#pragma endregion

#pragma endregion

	};

}
