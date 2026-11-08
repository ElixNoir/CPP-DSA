#pragma once

#include "Bitset.hpp"
#include "Collection.hpp"

namespace DSA {

	template <concepts::StoragePolicy ContainerConfiguration, typename T>
		requires std::is_nothrow_destructible_v<T>
	class TrackedContainer : public Collection<ContainerConfiguration, T> {
	public:

		using Base = Collection<ContainerConfiguration, T>;
		using typename Base::INDEX;
		using typename Base::T;

		constexpr static auto BitCount = Bitmask<>::BitCount;
		constexpr static auto BitShift = Bitmask<>::BitShift;

		using TRACKED = Bitset<
			std::conditional_t<
				concepts::DynamicStoragePolicy<ContainerConfiguration>,
				HeapStoragePolicy<INDEX, typename ContainerConfiguration::ALLOCATOR_TEMPLATE>,
				StaticContainerConfiguration<((Base::maximum_index + BitCount - 1) >> BitShift)>
			>
		>;

		TRACKED tracked;

	protected:

#pragma region Methods

		void helper_destroy(INDEX capacity = Base::default_capacity()) noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			INDEX count = Base::get_size();

			for (INDEX index : tracked.ones(get_bitset_capacity(capacity))) {
				if (count == 0)
					break;
				destroy_at(Base::get_data() + index);
				--count;
			}
		}

		template <bool GROW>
		void helper_resize(INDEX newCapacity, INDEX oldCapacity = Base::default_capacity())
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			using ALLOCATOR = Base::ALLOCATOR;

			const INDEX size = Base::get_size();

			const INDEX oldBitsetCapacity = get_bitset_capacity(oldCapacity);

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

		void destroy(INDEX capacity = Base::default_capacity()) noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy(capacity);
		}

#pragma endregion

#pragma region Methods

#pragma region Getters

		[[nodiscard]] constexpr INDEX get_bitset_capacity(INDEX capacity = Base::default_capacity()) const noexcept {
			return (capacity + BitCount - 1) >> BitShift;
		}

#pragma endregion

#pragma region Tracking

		[[nodiscard]] constexpr bool contains_tracked_index(INDEX index, INDEX capacity = Base::default_capacity()) const noexcept {
			return index < capacity && tracked.get_bit_at(index);
		}

		[[nodiscard]] constexpr INDEX find_first_tracked(INDEX capacity = Base::default_capacity()) const noexcept {
			return tracked.find_first_bit<true, true, true>(capacity);
		}

		[[nodiscard]] constexpr INDEX find_first_tracked(INDEX start, INDEX capacity = Base::default_capacity()) const noexcept {
			return tracked.find_first_bit_after<true, true, true>(start, capacity);
		}

		[[nodiscard]] constexpr INDEX find_last_tracked(INDEX capacity = Base::default_capacity()) const noexcept {
			return tracked.find_first_bit<false, true, true>(capacity);
		}

		[[nodiscard]] constexpr INDEX find_last_tracked(INDEX start, INDEX capacity = Base::default_capacity()) const noexcept {
			return tracked.find_first_bit_after<false, true, true>(start, capacity);
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

		void double_capacity(INDEX oldCapacity = Base::default_capacity())
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			grow(oldCapacity << 1);
		}

		constexpr void empty(INDEX capacity = Base::default_capacity()) noexcept {
			destroy(capacity);
			tracked.zero_raw(capacity);
			Base::set_size(0);
		}

		void grow(INDEX newCapacity, INDEX oldCapacity = Base::default_capacity())
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			helper_resize<true>(newCapacity, oldCapacity);
		}

		void reserve(INDEX newCapacity, INDEX oldCapacity = Base::default_capacity())
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			if (newCapacity > oldCapacity)
				grow(newCapacity, oldCapacity);
		}

		void resize(INDEX newCapacity, INDEX oldCapacity = Base::default_capacity())
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			if (newCapacity > oldCapacity)
				grow(newCapacity);
			else
				shrink(newCapacity);
		}

		void shrink(INDEX newCapacity, INDEX oldCapacity = Base::default_capacity())
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			helper_resize<false>(newCapacity, oldCapacity);
		}

#pragma endregion

#pragma endregion

	};

}