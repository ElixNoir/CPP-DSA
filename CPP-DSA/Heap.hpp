#pragma once

#include "Collection.hpp"
#include "swap.hpp"

#include <iostream>

namespace DSA {

	enum HeapType : uint8_t {

		MAX_HEAP,
		MIN_HEAP

	};

	template <concepts::StoragePolicy StoragePolicy, typename T, HeapType HEAP_TYPE = HeapType::MAX_HEAP>
		requires std::is_nothrow_destructible_v<T>
	class Heap : public Collection<StoragePolicy, T> {
	public:

		using Base = Collection<StoragePolicy, T>;
		using typename Base::ALLOCATOR;
		using typename Base::INDEX;
		using typename Base::T;

		constexpr static bool is_dynamic = Base::is_dynamic;

		constexpr static INDEX invalid_index = Base::invalid_index;

	protected:

#pragma region Methods

		constexpr INDEX parent_of(INDEX index) const noexcept { return (index - 1) >> 1; }
		constexpr INDEX left_of(INDEX index) const noexcept { return (index << 1) + 1; }
		constexpr INDEX right_of(INDEX index) const noexcept { return (index << 1) + 2; }

#pragma endregion

	public:

#pragma region Constructors & Destructors

		using Base::Base;

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

		template <typename... ARGUMENTS>
		constexpr void emplace(ARGUMENTS&&... arguments)
			noexcept (std::is_nothrow_constructible_v<T, ARGUMENTS...>)
		{
			const INDEX index = Base::get_size();
			construct_at<T>(Base::get_data() + index, forward<ARGUMENTS>(arguments)...);

			++Base::size;
			bubble_up(index);
		}

		constexpr void remove(INDEX index)
			noexcept (std::is_nothrow_swappable_v<T>)
		{
			const INDEX lastIndex = Base::get_size() - 1;
			if (index != lastIndex)
				Algorithms::swap(Base::get(index), Base::get(lastIndex));

			destroy_at(Base::get_data() + index);
			--Base::size;

			heapify(index);
		}

#pragma endregion

#pragma region Bubble Up, Extract, Heapify, & Insert

		constexpr void bubble_up(INDEX index)
			noexcept (std::is_nothrow_swappable_v<T>)
		{
			if constexpr (HEAP_TYPE == MAX_HEAP) {
				while (index != 0) {
					const INDEX parentIndex = parent(index);
					if (Base::get(parentIndex) >= Base::get(index))
						return;
					swap(Base::get(index), Base::get(parentIndex));
					index = parentIndex;
				}
			}
			else {
				while (index != 0) {
					const INDEX parentIndex = parent(index);
					if (Base::get(parentIndex) <= Base::get(index))
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
			noexcept (std::is_nothrow_swappable_v<T>)
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
			noexcept (std::is_nothrow_swappable_v<T>)
		{
			const INDEX size = Base::get_size();

			INDEX leftIndex = left_of(i);
			INDEX rightIndex = right_of(i);
			INDEX extremeIndex = i;

			if constexpr (HEAP_TYPE == MAX_HEAP) {
				if ((leftIndex < size) && (Base::get(leftIndex) > Base::get(extremeIndex)))
					extremeIndex = leftIndex;
				if ((rightIndex < size) && (Base::get(rightIndex) > Base::get(extremeIndex)))
					extremeIndex = rightIndex;
			}
			else {
				if ((leftIndex < size) && (Base::get(leftIndex) < Base::get(extremeIndex)))
					extremeIndex = leftIndex;
				if ((rightIndex < size) && (Base::get(rightIndex) < Base::get(extremeIndex)))
					extremeIndex = rightIndex;
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

		using Iterator = T*;
		using ConstIterator = const T*;

		[[nodiscard]] constexpr Iterator begin() noexcept {
			return Base::get_data();
		}

		[[nodiscard]] constexpr ConstIterator begin() const noexcept {
			return Base::get_data();
		}

		[[nodiscard]] constexpr ConstIterator cbegin() const noexcept {
			return Base::get_data();
		}

		[[nodiscard]] constexpr Iterator end() noexcept {
			return begin() + Base::get_size();
		}

		[[nodiscard]] constexpr ConstIterator end() const noexcept {
			return begin() + Base::get_size();
		}

		[[nodiscard]] constexpr ConstIterator cend() const noexcept {
			return cbegin() + Base::get_size();
		}

#pragma endregion

#pragma region Traversal

		template <std::invocable<INDEX> OPERATION>
		constexpr void in_order(OPERATION& operation, INDEX index)
			noexcept (std::is_nothrow_invocable_v<OPERATION&, INDEX>)
		{
			if (index == invalid_index)
				return;

			in_order(operation, left_of(index));
			std::invoke(operation, index);
			in_order(operation, right_of(index));
		}

		template <std::predicate<INDEX> PREDICATE>
		constexpr bool in_order(PREDICATE& predicate, INDEX index)
			noexcept (std::is_nothrow_invocable_v<PREDICATE&, INDEX>)
		{
			if (index == invalid_index)
				return true;

			if (!in_order(predicate, left_of(index)))
				return false;

			if (!std::invoke(predicate, index))
				return false;

			return in_order(predicate, right_of(index));
		}

		template <std::invocable<INDEX> OPERATION>
		constexpr void level_order(OPERATION operation)
			noexcept (std::is_nothrow_invocable_v<OPERATION&, INDEX>)
		{
			const INDEX size = Base::get_size();
			for (INDEX index = 0; index < size; ++index)
				operation(Base::get(index));
		}

		template <std::predicate<INDEX> PREDICATE>
		constexpr void level_order(PREDICATE predicate)
			noexcept (std::is_nothrow_invocable_v<PREDICATE&, INDEX>)
		{
			const INDEX size = Base::get_size();
			for (INDEX index = 0; index < size; ++index)
				if (!predicate(index))
					return;
		}

		template <std::invocable<INDEX> OPERATION>
		constexpr void post_order(OPERATION operation, INDEX index)
			noexcept (std::is_nothrow_invocable_v<OPERATION&, INDEX>)
		{
			if (index >= Base::get_size())
				return;

			operation(Base::get(index));
			pre_order(left_of(index));
			pre_order(right_of(index));
		}

		template <std::predicate<INDEX> PREDICATE>
		constexpr bool post_order(PREDICATE& predicate, INDEX index)
			noexcept (std::is_nothrow_invocable_v<PREDICATE&, INDEX>)
		{
			if (index == invalid_index)
				return true;

			if (!post_order(predicate, left_of(index)))
				return false;

			if (!post_order(predicate, right_of(index)))
				return false;

			return std::invoke(predicate, index);
		}

		template <std::invocable<INDEX> OPERATION>
		constexpr void pre_order(OPERATION operation, INDEX index)
			noexcept (std::is_nothrow_invocable_v<OPERATION&, INDEX>)
		{
			if (index >= Base::get_size())
				return;

			operation(Base::get(index));
			pre_order(left_of(index));
			pre_order(right_of(index));
		}

		template <std::predicate<INDEX> PREDICATE>
		constexpr bool pre_order(PREDICATE& predicate, INDEX index)
			noexcept (std::is_nothrow_invocable_v<PREDICATE&, INDEX>)
		{
			if (index == invalid_index)
				return true;

			if (!std::invoke(predicate, index))
				return false;

			if (!pre_order(predicate, left_of(index)))
				return false;

			return pre_order(predicate, right_of(index));
		}

#pragma endregion

#pragma endregion

	};

}