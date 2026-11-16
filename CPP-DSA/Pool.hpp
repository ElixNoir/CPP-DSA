#pragma once

#include "Bitset.hpp"
#include "heap_sort.hpp"
#include "Math.hpp"
#include "Stack.hpp"

namespace DSA {

	template <concepts::StoragePolicy StoragePolicy, typename T>
		requires std::is_nothrow_destructible_v<T>
	class Pool : public Collection<StoragePolicy, T> {
	public:

		using Base = Collection<StoragePolicy, T>;
		using typename Base::INDEX;
		using typename Base::T;

		using BitsetPolicy = std::conditional_t<
			concepts::DynamicStoragePolicy<StoragePolicy>,
			HeapStoragePolicy<INDEX, typename StoragePolicy::ALLOCATOR_TEMPLATE>,
			InlineStoragePolicy<((StoragePolicy::maximum_capacity + Bitmask<>::bit_count - 1) >> Bitmask<>::bit_shift)>
		>;

	protected:

		INDEX allocated_top = 0;
		Stack<StoragePolicy, INDEX> free_stack;

#pragma region Methods

		void helper_destroy() noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			const INDEX capacity = Base::get_capacity();
			constexpr INDEX bitsetCapacity = (capacity + Bitmask<>::bit_count - 1) >> Bitmask<>::bit_shift;

			Bitset<BitsetPolicy> bitset;
			if constexpr (concepts::DynamicStorage<Base>) {
				if constexpr (concepts::NothrowResizableStorage<Base, T>)
					bitset = Bitset<BitsetPolicy>(bitsetCapacity);
				else {
					try {
						bitset = Bitset<BitsetPolicy>(bitsetCapacity);
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

						destroy_range(Base::get_data() + freeStackSize, Base::get_data() + allocated_top);

						return;
					}
				}
			}
			bitset.zero_raw(capacity);

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

		void helper_resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			using ALLOCATOR = Base::ALLOCATOR;

			free_stack.grow(newCapacity);
			
			const INDEX capacity = Base::get_capacity();
			constexpr INDEX bitsetCapacity = (capacity + Bitmask<>::bit_count - 1) >> Bitmask<>::bit_shift;

			Bitset<BitsetPolicy> bitset(bitsetCapacity);
			bitset.zero_raw(capacity);

			const INDEX size = Base::get_size();

			bitset.zero_raw(capacity);
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

		constexpr void destroy() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy();
		}

#pragma endregion

#pragma region Methods

#pragma region Add & Remove

		[[nodiscard]] constexpr INDEX add() noexcept {
			INDEX index = free_stack.is_empty() ? allocated_top++ : free_stack.pop();
			++Base::size;
			return index;
		}

		constexpr INDEX add(const T& data) 
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			INDEX index;
			if (!free_stack.is_empty()) {
				index = free_stack.pop();
				if constexpr (std::is_nothrow_copy_constructible_v<T>)
					copy_construct_at(Base::get_data() + index, data);
				else {
					try {
						copy_construct_at(Base::get_data() + index, data);
					}
					catch (...) {
						free_stack.push(index);
						throw;
					}
				}
			}
			else {
				index = allocated_top;
				copy_construct_at(Base::get_data() + index, data);
				++allocated_top;
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
				if constexpr (std::is_nothrow_copy_constructible_v<T>)
					move_construct_at(Base::get_data() + index, std::move(data));
				else {
					try {
						move_construct_at(Base::get_data() + index, std::move(data));
					}
					catch (...) {
						free_stack.push(index);
						throw;
					}
				}
			}
			else {
				index = allocated_top;
				move_construct_at(Base::get_data() + index, std::move(data));
				++allocated_top;
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

		constexpr INDEX allocate() noexcept {
			return add();
		}

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

		void double_capacity()
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::get_capacity();
			grow(capacity << 1, capacity);
		}

		void expand(float ratio = 1.5)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::get_capacity();
			grow(Math::ceiling(capacity * ratio), capacity);
		}

		constexpr void empty()
			noexcept (concepts::NothrowResizableStorage<Base, T>)
		{
			helper_destroy();
			free_stack.empty();
			allocated_top = 0;
			Base::set_size(0);
		}

		void grow(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			helper_resize(newCapacity, capacity);
		}

		void reserve(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity > capacity)
				grow(newCapacity);
		}

#pragma endregion

#pragma endregion

	};

}
