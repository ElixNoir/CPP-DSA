#pragma once

#include "Collection.hpp"
#include "swap.hpp"

#include <iostream>

namespace DSA {

	enum HeapType : uint8_t {

		MAX_HEAP,
		MIN_HEAP

	};

	template <concepts::StoragePolicy ContainerConfiguration, typename T, HeapType HEAP_TYPE = HeapType::MAX_HEAP>
		requires std::is_nothrow_destructible_v<T>
	class Heap : public Collection<ContainerConfiguration, T> {
	public:

		using Base = Collection<ContainerConfiguration, T>;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

#pragma region Methods

		constexpr INDEX parent(INDEX i) const noexcept { return (i - 1) >> 1; }
		constexpr INDEX left(INDEX i) const noexcept { return (i << 1) + 1; }
		constexpr INDEX right(INDEX i) const noexcept { return (i << 1) + 2; }

		void helper_destroy() noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			destroy_range_backward(Base::get_data(), Base::get_data() + Base::get_size());
		}

		template <bool GROW>
		void helper_resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			using ALLOCATOR = Base::ALLOCATOR;

			T* const oldData = Base::get_data();
			T* const newData = ALLOCATOR::allocate(newCapacity);

			try {
				if constexpr (GROW)
					reconstruct_range_backward(newData, newData + Base::get_size(), oldData);
				else {
					reconstruct_range_backward(newData, newData + newCapacity, oldData);
					destroy_range_backward(oldData + newCapacity, oldData + Base::get_size());
					Base::set_size(newCapacity);
				}
			}
			catch (...) {
				ALLOCATOR::deallocate(newData);
				throw;
			}

			ALLOCATOR::deallocate(oldData);

			Base::set_data(newData);
			if constexpr (concepts::DynamicContainer<Base>)
				Base::set_capacity(newCapacity);
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		using Base::Base;

		~Heap() noexcept = default;

		~Heap() noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			destroy();
		}

		void destroy() noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			helper_destroy();
		}

#pragma endregion

#pragma region Methods

#pragma region Add & Remove

		constexpr void add(const T& data)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			const INDEX index = Base::get_size();
			Base::get(index) = data;

			++Base::size;
			bubble_up(index);
		}

		constexpr void add(T&& data)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX index = Base::get_size();
			Base::get(index) = std::move(data);

			++Base::size;
			bubble_up(index);
		}

		template <typename... Arguments>
		constexpr void emplace(Arguments&&... arguments)
			noexcept (std::is_nothrow_constructible_v<T, Arguments...>)
		{
			const INDEX index = Base::get_size();
			construct_at<T>(Base::get_data() + index, forward<Arguments>(arguments)...);

			++Base::size;
			bubble_up(index);
		}

		constexpr void remove(INDEX index)
			noexcept (concepts::is_nothrow_swappable_v<T>)
		{
			const INDEX size = Base::get_size();

			destroy_at(Base::get_data() + index);

			if (index >= size)
				return;
			if (index == size - 1) {
				--Base::size;
				return;
			}

			swap(Base::get(index), Base::get(size - 1));
			--Base::size;

			heapify(index);
		}

#pragma endregion

#pragma region Bubble Up, Extract, Heapify, & Insert

		constexpr void bubble_up(INDEX index)
			noexcept (concepts::is_nothrow_swappable_v<T>)
		{
			if constexpr (HEAP_TYPE == MAX_HEAP) {
				while (index != 0) {
					const INDEX parentIndex = parent(index);
					if (Base::get(parentIndex) < Base::get(index))
						return;
					swap(Base::get(index), Base::get(parentIndex));
					index = parentIndex;
				}
			}
			else {
				while (index != 0) {
					const INDEX parentIndex = parent(index);
					if (Base::get(parentIndex) < Base::get(index))
						return;
					swap(Base::get(index), Base::get(parentIndex));
					index = parentIndex;
				}
			}
		}

		[[nodiscard]] constexpr bool can_extract() const noexcept {
			return !Base::is_empty();
		}

		constexpr T extract()
			noexcept (concepts::is_nothrow_swappable_v<T>)
		{
			const INDEX size = Base::get_size();

			T root = std::move(Base::get(0));
			destroy_at(Base::get_data());

			if (size == 1) {
				--Base::size;
				return root;
			}

			swap(Base::get(0), Base::get(size - 1));
			--Base::size;

			heapify(0);

			return root;
		}

		constexpr void heapify(INDEX i)
			noexcept (concepts::is_nothrow_swappable_v<T>)
		{
			const INDEX size = Base::get_size();

			INDEX l = left(i);
			INDEX r = right(i);
			INDEX extremeIndex = i;

			if constexpr (HEAP_TYPE == MAX_HEAP) {
				if ((l < size) && (Base::get(l) > Base::get(extremeIndex)))
					extremeIndex = l;
				if ((r < size) && (Base::get(r) > Base::get(extremeIndex)))
					extremeIndex = r;
			}
			else {
				if ((l < size) && (Base::get(l) < Base::get(extremeIndex)))
					extremeIndex = l;
				if ((r < size) && (Base::get(r) < Base::get(extremeIndex)))
					extremeIndex = r;
			}

			if (extremeIndex != i) {
				swap(Base::get(i), Base::get(extremeIndex));
				heapify(extremeIndex);
			}
		}

		[[nodiscard]] constexpr bool can_insert(INDEX count = 1) const noexcept {
			return Base::can_add(count);
		}

		constexpr void insert(const T& data)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			add(data);
		}

		constexpr void insert(T&& data)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			add(std::move(data));
		}

#pragma endregion

#pragma region Memory Management

		void double_capacity(INDEX oldCapacity = Base::default_capacity())
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicContainer<Base>
		{
			grow(oldCapacity << 1);
		}

		constexpr void empty() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy();
			Base::set_size(0);
		}

		void grow(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			helper_resize<true>(newCapacity);
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
				grow(newCapacity, oldCapacity);
			else
				shrink(newCapacity);
		}

		void shrink(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			helper_resize<false>(newCapacity);
		}

#pragma endregion

#pragma region Operators

		friend std::ostream& operator<<(std::ostream& ostream, const Heap& heap) {
			INDEX power = 0;
			INDEX data = 1;
			for (INDEX index = 0; index < heap.get_size(); ++index) {
				if (index == data) {
					ostream << '\n';
					data += (1 << ++power);
				}
				ostream << heap.get(index) << "  ";
			}

			ostream << '\n';

			return ostream;
		}

#pragma endregion

#pragma region Iteration

		/*StackIterator<T> begin() noexcept {
			return StackIterator<T>(Base::get_data());
		}

		StackIterator<const T> begin() const noexcept {
			return StackIterator<const T>(Base::get_data());
		}

		StackIterator<T> end() noexcept {
			return StackIterator<T>(Base::get_data() + Base::get_size());
		}

		StackIterator<const T> end() const noexcept {
			return StackIterator<const T>(Base::get_data() + Base::get_size());
		}*/

#pragma endregion

#pragma endregion

	};

}