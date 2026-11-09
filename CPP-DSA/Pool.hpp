#pragma once

#include "Bitset.hpp"
#include "heap_sort.hpp"
#include "Stack.hpp"

namespace DSA {

	template <concepts::StoragePolicy StoragePolicy, typename T>
		requires std::is_nothrow_destructible_v<T>
	class Pool : public Collection<StoragePolicy, T> {
	public:

		using Base = Collection<StoragePolicy, T>;
		using typename Base::INDEX;
		using typename Base::T;

		using BitsetConfiguration = std::conditional_t<
			concepts::DynamicStoragePolicy<StoragePolicy>,
			HeapStoragePolicy<INDEX, typename StoragePolicy::ALLOCATOR_TEMPLATE>,
			StaticStoragePolicy<StoragePolicy::maximum_index>
		>;

	protected:

		INDEX free_top = 0;
		Stack<StoragePolicy, INDEX> free_stack;

#pragma region Methods

		void helper_destroy(Base::Capacity explicitCapacity = {}) noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			constexpr INDEX bitsetCapacity = (capacity + Bitmask<>::BitCount - 1) >> Bitmask<>::BitShift;

			Bitset<BitsetConfiguration> bitset;
			if constexpr (concepts::DynamicStorage<Base>) {
				if constexpr (concepts::NothrowResizableStorage<Base, T>)
					bitset = Bitset<BitsetConfiguration>(bitsetCapacity);
				else {
					try {
						bitset = Bitset<BitsetConfiguration>(bitsetCapacity);
					}
					catch (...) { // destroy as efficiently as possible without needing to heap-allocate memory
						const INDEX freeStackSize = free_stack.get_size();
						Sorting::heap_sort(free_stack.get_data(), 0, freeStackSize);

						INDEX top = 0;
						INDEX startIndex = 0;
						while (top < freeStackSize) {
							const INDEX excludedIndex = free_stack.get(top);
							for (INDEX index = startIndex; index < excludedIndex; ++index)
								destroy_at(Base::get_data() + index);
							startIndex = excludedIndex + 1;
							++top;
						}

						destroy_range_forward(Base::get_data() + freeStackSize, Base::get_data() + free_top);

						return;
					}
				}
			}
			bitset.zero_raw(explicitCapacity);

			for (INDEX index : free_stack)
				bitset.set_bit_at(index);

			INDEX count = Base::get_size();
			for (INDEX index : bitset.zeros(bitsetCapacity)) {
				if (count == 0)
					break;
				destroy_at(Base::get_data() + index);
				--count;
			}
		}

		void helper_resize(INDEX newCapacity, Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			using ALLOCATOR = Base::ALLOCATOR;

			free_stack.grow(newCapacity);
			
			constexpr INDEX bitsetCapacity = (capacity + Bitmask<>::BitCount - 1) >> Bitmask<>::BitShift;

			Bitset<BitsetConfiguration> bitset(bitsetCapacity);
			bitset.zero_raw(explicitCapacity);

			const INDEX size = Base::get_size();

			bitset.zero_raw(explicitCapacity);
			for (INDEX index : free_stack)
				bitset.set_bit_at(index);

			T* const oldData = Base::get_data();
			T* const newData = ALLOCATOR::allocate(newCapacity);

			INDEX count = size;

			try {
				for (INDEX index : bitset.zeros(bitsetCapacity)) {
					if (count == 0)
						break;
					move_construct_at(newData + index, std::move(oldData[index]));
					--count;
				}
			}
			catch (...) {
				if constexpr (!std::is_trivially_destructible_v<T>) {
					for (INDEX index : bitset.zeros(bitsetCapacity)) {
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
				for (INDEX index : bitset.zeros(bitsetCapacity)) {
					if (count == size)
						break;
					destroy_at(oldData + index);
					++count;
				}
			}

			bitset.destroy(bitsetCapacity);

			ALLOCATOR::deallocate(oldData);

			Base::set_data(newData);
			if (concepts::HasMutableCapacity<Base>)
				Base::set_capacity(newCapacity);
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		Pool() noexcept = default;

		Pool(INDEX initialCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
		: Base(initialCapacity), free_stack(initialCapacity) {}

		~Pool() noexcept
			requires concepts::Container<Base>
		{
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy();
		}

		constexpr void destroy(Base::Capacity explicitCapacity = {}) noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy(explicitCapacity);
		}

#pragma endregion

#pragma region Methods

#pragma region Add & Remove

		constexpr INDEX add(const T& data) 
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			INDEX index;
			if (!free_stack.is_empty()) {
				index = free_stack.pop();
				try {
					copy_construct_at(Base::get_data() + index, data);
				}
				catch (...) {
					free_stack.push(index);
					throw;
				}
			}
			else {
				index = free_top;
				copy_construct_at(Base::get_data() + index, data);
				++free_top;
			}

			++Base::size;

			return index;
		}

		constexpr INDEX add(T&& data) 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			INDEX index;
			if (!free_stack.is_empty()) {
				index = free_stack.pop();
				try {
					move_construct_at(Base::get_data() + index, std::move(data));
				}
				catch (...) {
					free_stack.push(index);
					throw;
				}
			}
			else {
				index = free_top;
				move_construct_at(Base::get_data() + index, std::move(data));
				++free_top;
			}

			++Base::size;

			return index;
		}

		constexpr void remove(INDEX index) noexcept {
			destroy_at(Base::get_data() + index);
			free_stack.push(index);
			--Base::size;
		}

#pragma endregion

#pragma region Allocate & Deallocate

		constexpr INDEX allocate(const T& data) 
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			return add(data);
		}

		constexpr INDEX allocate(T&& data) 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			return add(std::move(data));
		}

		constexpr void deallocate(INDEX index) noexcept {
			remove(index);
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

		constexpr void empty(Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
		{
			helper_destroy(explicitCapacity);
			free_stack.empty();
			free_top = 0;
			Base::set_size(0);
		}

		void grow(INDEX newCapacity, Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			helper_resize(newCapacity, explicitCapacity);
		}

		void reserve(INDEX newCapacity, Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			if (newCapacity > capacity)
				grow(explicitCapacity);
		}

#pragma endregion

#pragma endregion

	};

}
