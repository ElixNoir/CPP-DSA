#pragma once

#include "Bitset.hpp"
#include "Stack.hpp"

namespace DSA {

	template <concepts::StorageConfiguration ContainerConfiguration, typename T>
		requires std::is_nothrow_destructible_v<T>
	class Pool : public Collection<ContainerConfiguration, T> {
	public:

		using Base = Collection<ContainerConfiguration, T>;
		using typename Base::INDEX;
		using typename Base::T;

		using BitsetConfiguration = std::conditional_t<
			concepts::DynamicStorageConfiguration<ContainerConfiguration>,
			HeapStorageConfiguration<INDEX, typename ContainerConfiguration::ALLOCATOR_TEMPLATE>,
			StaticContainerConfiguration<ContainerConfiguration::maximum_index>
		>;

	protected:

		INDEX free_top = 0;
		Stack<ContainerConfiguration, INDEX> free_stack;

#pragma region Methods

		void helper_destroy(INDEX oldCapacity = Base::default_capacity()) noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			constexpr INDEX bitsetCapacity = (oldCapacity + Bitmask<>::BitCount - 1) >> Bitmask<>::BitShift;

			Bitset<BitsetConfiguration> bitset;
			if constexpr (concepts::DynamicStorage<Base>) {
				if constexpr (concepts::NothrowResizableStorage<Base, T>)
					bitset = Bitset<BitsetConfiguration>(bitsetCapacity);
				else {
					try {
						bitset = Bitset<BitsetConfiguration>(bitsetCapacity);
					}
					catch (...) {
						// TO-DO: destroy without needing bitset
						return;
					}
				}
			}
			bitset.zero_raw(oldCapacity);

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

		void helper_resize(INDEX newCapacity, INDEX oldCapacity = Base::default_capacity())
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			using ALLOCATOR = Base::ALLOCATOR;

			constexpr INDEX bitsetCapacity = (oldCapacity + Bitmask<>::BitCount - 1) >> Bitmask<>::BitShift;

			Bitset<BitsetConfiguration> bitset(bitsetCapacity);
			bitset.zero_raw(oldCapacity);

			const INDEX size = Base::get_size();

			bitset.zero_raw(oldCapacity);
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

		constexpr void destroy(INDEX capacity = Base::default_capacity()) noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy(capacity);
		}

#pragma endregion

#pragma region Methods

#pragma region Add & Remove

		constexpr INDEX add(const T& value) 
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			INDEX index;
			if (!free_stack.is_empty()) {
				index = free_stack.pop();
				try {
					copy_construct_at(Base::get_data() + index, value);
				}
				catch (...) {
					free_stack.push(index);
					throw;
				}
			}
			else {
				index = free_top;
				copy_construct_at(Base::get_data() + index, value);
				++free_top;
			}

			++Base::size;

			return index;
		}

		constexpr INDEX add(T&& value) 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			INDEX index;
			if (!free_stack.is_empty()) {
				index = free_stack.pop();
				try {
					move_construct_at(Base::get_data() + index, std::move(value));
				}
				catch (...) {
					free_stack.push(index);
					throw;
				}
			}
			else {
				index = free_top;
				move_construct_at(Base::get_data() + index, std::move(value));
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

		constexpr INDEX allocate(const T& value) 
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			return add(value);
		}

		constexpr INDEX allocate(T&& value) 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			return add(std::move(value));
		}

		constexpr void deallocate(INDEX index) noexcept {
			remove(index);
		}

#pragma endregion

#pragma region Memory Management

		void double_capacity(INDEX oldCapacity = Base::default_capacity())
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			grow(oldCapacity << 1, oldCapacity);
		}

		constexpr void empty(INDEX capacity = Base::default_capacity())
			noexcept (concepts::NothrowResizableStorage<Base, T>)
		{
			helper_destroy(capacity);
			free_stack.empty();
			free_top = 0;
			Base::set_size(0);
		}

		void grow(INDEX newCapacity, INDEX oldCapacity = Base::default_capacity())
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			helper_resize(newCapacity, oldCapacity);
			free_stack.grow(newCapacity, oldCapacity);
		}

		void reserve(INDEX newCapacity, INDEX oldCapacity = Base::default_capacity())
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			if (newCapacity > oldCapacity)
				grow(newCapacity);
		}

#pragma endregion

#pragma endregion

	};

}