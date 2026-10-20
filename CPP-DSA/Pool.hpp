#pragma once

#include "Bitset.hpp"
#include "IncrementalContainer.hpp"
#include "Stack.hpp"

namespace DSA {

	template <typename ContainerConfiguration, typename T>
		requires std::is_nothrow_destructible_v<T>
	class Pool : public IncrementalContainer<ContainerConfiguration, T> {
	public:

		using Base = IncrementalContainer<ContainerConfiguration, T>;
		using typename Base::INDEX;
		using typename Base::T;

		using BitsetContainerConfiguration = std::conditional_t<
			concepts::DynamicContainer<Base>,
			AlternateDynamicContainerConfiguration<INDEX, ContainerConfiguration::template ALLOCATOR_TEMPLATE>,
			StaticContainerConfiguration<ContainerConfiguration::maximum_capacity>
		>;

	protected:

		INDEX free_top = 0;
		Stack<ContainerConfiguration, T> free_stack;

		void helper_destroy()
			noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			Bitset<BitsetContainerConfiguration> bitset((Base::get_capacity() + Bitmask<>::BitCount - 1) >> Bitmask<>::BitShift);
			bitset.zero();
			for (INDEX index : free_stack)
				bitset.set_bit_at(index);

			INDEX count = Base::get_size();
			for (INDEX index : bitset.zeros()) {
				if (count == 0)
					break;
				destroy_at(Base::get_data() + index);
				count--;
			}
		}

		void helper_resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableAllocator<Base::ALLOCATOR>
				&& std::is_nothrow_move_constructible_v<T>)
			requires (concepts::DynamicContainer<Base>
				&& !concepts::ResizableContainer<Base>)
		{
			using ALLOCATOR = Base::ALLOCATOR;

			const INDEX size = Base::get_size();

			Bitset<BitsetContainerConfiguration> bitset((Base::get_capacity() + Bitmask<>::BitCount - 1) >> Bitmask<>::BitShift);
			bitset.zero();
			for (INDEX index : free_stack)
				bitset.set_bit_at(index);

			T* const oldData = Base::get_data();
			T* const newData = ALLOCATOR::allocate(newCapacity);

			INDEX count = size;

			try {
				for (INDEX index : bitset.zeros()) {
					if (count == 0)
						break;
					move_construct_at(newData + index, std::move(oldData[index]));
					count--;
				}
			}
			catch (...) {
				if constexpr (!std::is_trivially_destructible_v<T>) {
					for (INDEX index : bitset.zeros()) {
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
				for (INDEX index : bitset.zeros()) {
					if (count == size)
						break;

					destroy_at(oldData + index);
					count++;
				}
			}

			ALLOCATOR::deallocate(oldData);

			Base::data = reinterpret_cast<std::byte*>(newData);
			Base::capacity = newCapacity;
		}

	public:

#pragma region Constructors & Destructors

		Pool() noexcept = default;

		Pool(INDEX initialCapacity) : Base(initialCapacity), free_stack(initialCapacity) {}

		~Pool()
			noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			helper_destroy();
		}

		~Pool() noexcept = default;

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
				index = free_top++;
				try {
					copy_construct_at(Base::get_data() + index, value);
				}
				catch (...) {
					free_top--;
					throw;
				}
			}

			Base::size++;

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
				index = free_top++;
				try {
					move_construct_at(Base::get_data() + index, std::move(value));
				}
				catch (...) {
					free_top--;
					throw;
				}
			}

			Base::size++;

			return index;
		}

		constexpr void remove(INDEX index) noexcept {
			destroy_at(Base::get_data() + index);
			free_stack.push(index);
			Base::size--;
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

		void double_capacity()
			noexcept (concepts::NothrowResizableAllocator<Base::ALLOCATOR>
				&& std::is_nothrow_move_constructible_v<T>)
			requires concepts::DynamicContainer<Base>
		{
			grow(Base::get_capacity() << 1);
		}

		constexpr void empty() noexcept {
			helper_destroy();
			free_stack.empty();
			free_top = 0;
			Base::size = 0;
		}

		void grow(INDEX newCapacity)
			noexcept (concepts::NothrowResizableAllocator<Base::ALLOCATOR>
				&& std::is_nothrow_move_constructible_v<T>)
			requires concepts::DynamicContainer<Base>
		{
			helper_resize(newCapacity);
			free_stack.grow(newCapacity);
		}

		void reserve(INDEX newCapacity)
			noexcept (concepts::NothrowResizableAllocator<Base::ALLOCATOR>
				&& std::is_nothrow_move_constructible_v<T>)
			requires concepts::DynamicContainer<Base>
		{
			if (newCapacity > Base::get_capacity())
				grow(newCapacity);
		}

#pragma endregion

#pragma endregion

	};

}