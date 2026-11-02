#pragma once

#include "Bitset.hpp"
#include "Collection.hpp"

namespace DSA {

	template <concepts::StorageConfiguration ContainerConfiguration, typename T>
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
				concepts::DynamicStorageConfiguration<ContainerConfiguration>,
				HeapStorageConfiguration<INDEX>,
				StaticContainerConfiguration<((Base::maximum_capacity + BitCount - 1) >> BitShift)>
			>
		>;

		TRACKED tracked;

	protected:

#pragma region Methods

		void helper_destroy()
			noexcept
			requires (concepts::Container<Base> && !std::is_trivially_destructible_v<T>)
		{
			if constexpr (concepts::DynamicStorageConfiguration<ContainerConfiguration>)
				helper_destroy(Base::get_capacity());
			else {
				INDEX count = Base::get_size();

				for (INDEX index : tracked.ones() {
					if (count == 0)
						break;
					destroy_at(Base::get_data() + index);
					count--;
				}
			}
		}

		void helper_destroy(INDEX capacity)
			noexcept
			requires (concepts::Storage<Base> && !std::is_trivially_destructible_v<T>)
		{
			INDEX count = Base::get_size();

			for (INDEX index : tracked.ones(((capacity + BitCount - 1) >> BitShift)) {
				if (count == 0)
					break;
				destroy_at(Base::get_data() + index);
				count--;
			}
		}

		template <bool GROW>
		void helper_resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicContainer<Base>
		{
			helper_resize<GROW>(newCapacity, Base::get_capacity());
		}

		template <bool GROW>
		void helper_resize(INDEX newCapacity, INDEX oldCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
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

			Base::set_data(reinterpret_cast<std::byte*>(newData));
			Base::set_capacity(newCapacity);

			if constexpr (GROW)
				tracked.grow(((newCapacity + BitCount - 1) >> BitShift), ((oldCapacity + BitCount - 1) >> BitShift));
			else
				tracked.shrink(((newCapacity + BitCount - 1) >> BitShift));
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		TrackedContainer(INDEX initialCapacity)
			requires (concepts::DynamicStorage<Base>) :
			Base(initialCapacity),
			tracked((initialCapacity + Bitmask<>::BitCount - 1) >> Bitmask<>::BitShift) {}

		TrackedContainer() noexcept = default;

		void destroy(INDEX capacity)
			noexcept
			requires concepts::Container<Base>
		{
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy(capacity);
		}

		~TrackedContainer()
			noexcept
			requires concepts::Container<Base>
		{
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy();
		}

#pragma endregion

#pragma region Methods

#pragma region Tracking

		[[nodiscard]] constexpr bool contains_tracked_index(INDEX index)
			const noexcept
			requires concepts::Container<Base>
		{
			return contains_tracked_index(Base::get_capacity());
		}

		[[nodiscard]] constexpr bool contains_tracked_index(INDEX index, INDEX capacity)
			const noexcept
			requires concepts::Storage<Base>
		{
			return index < capacity && tracked.get_bit_at(index);
		}

		[[nodiscard]] constexpr INDEX find_first_tracked()
			const noexcept
			requires concepts::Container<Base>
		{
			return find_first_tracked_and_capacity(Base::get_capacity());
		}

		[[nodiscard]] constexpr INDEX find_first_tracked_and_capacity(INDEX capacity)
			const noexcept
			requires concepts::Storage<Base>
		{
			return tracked.find_first_bit_and_capacity<true, true, true>(0, capacity);
		}

		[[nodiscard]] constexpr INDEX find_first_tracked(INDEX start)
			const noexcept
			requires concepts::Container<Base>
		{
			return find_first_tracked_and_capacity(start, Base::get_capacity());
		}

		[[nodiscard]] constexpr INDEX find_first_tracked_and_capacity(INDEX start, INDEX capacity)
			const noexcept
			requires concepts::Storage<Base>
		{
			return tracked.find_first_bit_and_capacity<true, true, true>(start, capacity);
		}

		[[nodiscard]] constexpr INDEX find_last_tracked()
			const noexcept
			requires concepts::Container<Base>
		{
			return find_last_tracked_and_capacity(Base::get_capacity());
		}

		[[nodiscard]] constexpr INDEX find_last_tracked_and_capacity(INDEX capacity)
			const noexcept
			requires concepts::Storage<Base>
		{
			return tracked.find_first_bit_and_capacity<false, true, true>(0, capacity);
		}

		[[nodiscard]] constexpr INDEX find_last_tracked(INDEX start)
			const noexcept
			requires concepts::Container<Base>
		{
			return find_last_tracked_and_capacity(start, Base::get_capacity());
		}

		[[nodiscard]] constexpr INDEX find_last_tracked_and_capacity(INDEX start, INDEX capacity)
			const noexcept
			requires concepts::Storage<Base>
		{
			return tracked.find_first_bit_and_capacity<false, true, true>(start, capacity);
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
			requires concepts::DynamicContainer<Base>
		{
			double_capacity(Base::get_capacity() << 1);
		}

		void double_capacity(INDEX oldCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			grow(oldCapacity << 1);
		}

		constexpr void empty() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy();

			tracked.zero(Base::get_capacity());
			Base::set_size(0);
		}

		void grow(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicContainer<Base>
		{
			grow(newCapacity, Base::get_capacity());
		}

		void grow(INDEX newCapacity, INDEX oldCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			helper_resize<true>(newCapacity, oldCapacity);
		}

		void reserve(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicContainer<Base>
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity > capacity)
				reserve(newCapacity, capacity);
		}

		void reserve(INDEX newCapacity, INDEX oldCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			if (newCapacity > oldCapacity)
				grow(newCapacity, oldCapacity);
		}

		void resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicContainer<Base>
		{
			resize(newCapacity, Base::get_capacity());
		}

		void resize(INDEX newCapacity, INDEX oldCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			if (newCapacity > oldCapacity)
				grow(newCapacity);
			else
				shrink(newCapacity);
		}

		void shrink(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicContainer<Base>
		{
			shrink(newCapacity, Base::get_capacity());
		}

		void shrink(INDEX newCapacity, INDEX oldCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			helper_resize<false>(newCapacity, oldCapacity);
		}

#pragma endregion

#pragma endregion

	};

}