#pragma once

#include "Collection.hpp"
#include "swap.hpp"

#include <bit>

#include <iostream>

namespace DSA {

	enum class HeapType : uint8_t {

		BIDIRECTIONAL,
		MAXIMUM,
		MINIMUM

	};

	template <concepts::ContainerPolicy ContainerPolicy, typename T, HeapType HEAP_TYPE = HeapType::MAXIMUM>
		requires std::is_nothrow_destructible_v<T>
	class Heap : public Collection<ContainerPolicy, T> {
	public:

		using Base = Collection<ContainerPolicy, T>;
		using typename Base::ALLOCATOR;
		using typename Base::INDEX;
		using typename Base::T;

		constexpr static INDEX invalid_index = Base::invalid_index;

	protected:

#pragma region Methods

		constexpr bool is_min_level(INDEX index) const noexcept {
			return ((std::bit_width(INDEX(index + INDEX(1))) - 1) & 1) == 0;
		}

		constexpr INDEX parent_of(INDEX index) const noexcept { return (index - 1) >> 1; }
		constexpr INDEX left_of(INDEX index) const noexcept { return (index << 1) + 1; }
		constexpr INDEX right_of(INDEX index) const noexcept { return (index << 1) + 2; }

		template <bool MAXIMUM>
		constexpr INDEX extreme_descendant(INDEX index) const noexcept {
			INDEX extreme = index;

			const INDEX size = Base::get_size();
			const INDEX firstChild = left_of(index);
			const INDEX lastChild = right_of(index);
			for (INDEX child = firstChild; child < size && child <= lastChild; ++child) {
				if constexpr (MAXIMUM) {
					if (Base::get(child) > Base::get(extreme))
						extreme = child;
				}
				else {
					if (Base::get(child) < Base::get(extreme))
						extreme = child;
				}

				const INDEX firstGrandchild = left_of(child);
				const INDEX lastGrandchild = right_of(child);

				for (INDEX grandchild = firstGrandchild; grandchild < size && grandchild <= lastGrandchild; ++grandchild) {
					if constexpr (MAXIMUM) {
						if (Base::get(grandchild) > Base::get(extreme))
							extreme = grandchild;
					}
					else {
						if (Base::get(grandchild) < Base::get(extreme))
							extreme = grandchild;
					}
				}
			}

			return extreme;
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		using Base::Base;

#pragma endregion

#pragma region Methods

#pragma region Add & Remove

		constexpr void add(const T& value)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			const INDEX index = Base::get_size();
			copy_construct_at(Base::get_data() + index, value);
			++Base::size;
			bubble_up(index);
		}

		constexpr void add(T&& value)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX index = Base::get_size();
			move_construct_at(Base::get_data() + index, std::move(value));
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

			if (index != 0) {
				const INDEX parentIndex = parent_of(index);

				if constexpr (HEAP_TYPE == HeapType::BIDIRECTIONAL) {
					if (is_min_level(index)
						? Base::get(index) > Base::get(parentIndex)
						: Base::get(index) < Base::get(parentIndex))
					{
						bubble_up(index);
						return;
					}
				}
				else if constexpr (HEAP_TYPE == HeapType::MAXIMUM) {
					if (Base::get(index) > Base::get(parentIndex)) {
						bubble_up(index);
						return;
					}
				}
				else {
					if (Base::get(index) < Base::get(parentIndex)) {
						bubble_up(index);
						return;
					}
				}
			}

			trickle_down(index);
		}

#pragma endregion

#pragma region Bubble Up & Trickle Down

		constexpr void bubble_up(INDEX index)
			noexcept (std::is_nothrow_swappable_v<T>)
		{
			if constexpr (HEAP_TYPE == HeapType::BIDIRECTIONAL) {
				const INDEX parentIndex = parent_of(index);
				if (is_min_level(index)) {
					if (Base::get(index) > Base::get(parentIndex)) {
						Algorithms::swap(Base::get(index), Base::get(parentIndex));
						bubble_up_max(parentIndex);
					}
					else
						bubble_up_min(index);
				}
				else {
					if (Base::get(index) < Base::get(parentIndex)) {
						Algorithms::swap(Base::get(index), Base::get(parentIndex));
						bubble_up_min(parentIndex);
					}
					else
						bubble_up_max(index);
				}
			}
			else if constexpr (HEAP_TYPE == HeapType::MAXIMUM)
				bubble_up_max(index);
			else
				bubble_up_min(index);
		}

		constexpr void bubble_up_max(INDEX index)
			noexcept (std::is_nothrow_swappable_v<T>)
			requires (HEAP_TYPE == HeapType::BIDIRECTIONAL || HEAP_TYPE == HeapType::MAXIMUM)
		{
			if constexpr (HEAP_TYPE == HeapType::BIDIRECTIONAL) {
				while (index >= 3) {
					const INDEX grandparent = parent_of(parent_of(index));
					if (Base::get(index) <= Base::get(grandparent))
						break;

					Algorithms::swap(Base::get(index), Base::get(grandparent));
					index = grandparent;
				}
			}
			else {
				while (index != 0) {
					const INDEX parentIndex = parent_of(index);
					if (Base::get(parentIndex) >= Base::get(index))
						return;

					Algorithms::swap(Base::get(index), Base::get(parentIndex));
					index = parentIndex;
				}
			}
		}

		constexpr void bubble_up_min(INDEX index)
			noexcept (std::is_nothrow_swappable_v<T>)
			requires (HEAP_TYPE == HeapType::BIDIRECTIONAL || HEAP_TYPE == HeapType::MINIMUM)
		{
			if constexpr (HEAP_TYPE == HeapType::BIDIRECTIONAL) {
				while (index >= 3) {
					const INDEX grandparent = parent_of(parent_of(index));
					if (Base::get(index) >= Base::get(grandparent))
						break;

					Algorithms::swap(Base::get(index), Base::get(grandparent));
					index = grandparent;
				}
			}
			else {
				while (index != 0) {
					const INDEX parentIndex = parent_of(index);
					if (Base::get(parentIndex) <= Base::get(index))
						return;

					Algorithms::swap(Base::get(index), Base::get(parentIndex));
					index = parentIndex;
				}
			}
		}

		constexpr void trickle_down(INDEX index)
			noexcept (std::is_nothrow_swappable_v<T>)
		{
			if constexpr (HEAP_TYPE == HeapType::BIDIRECTIONAL) {
				if (is_min_level(index))
					trickle_down_min(index);
				else
					trickle_down_max(index);
			}
			else if constexpr (HEAP_TYPE == HeapType::MAXIMUM)
				trickle_down_max(index);
			else
				trickle_down_min(index);
		}

		constexpr void trickle_down_max(INDEX index)
			noexcept (std::is_nothrow_swappable_v<T>)
			requires (HEAP_TYPE == HeapType::BIDIRECTIONAL || HEAP_TYPE == HeapType::MAXIMUM)
		{
			const INDEX size = Base::get_size();
			if constexpr (HEAP_TYPE == HeapType::BIDIRECTIONAL) {
				while (true) {
					const INDEX extreme = extreme_descendant<true>(index);
					if (extreme == index)
						return;

					const INDEX parentIndex = parent_of(extreme);
					if (parentIndex == index) {
						if (Base::get(extreme) > Base::get(index))
							Algorithms::swap(Base::get(extreme), Base::get(index));

						return;
					}
					if (Base::get(extreme) > Base::get(index)) {
						Algorithms::swap(Base::get(extreme), Base::get(index));
						if (Base::get(extreme) < Base::get(parentIndex))
							Algorithms::swap(Base::get(extreme), Base::get(parentIndex));

						index = extreme;
					}
				}
			}
			else {
				while (true) {
					INDEX extreme = index;

					const INDEX leftIndex = left_of(index);
					const INDEX rightIndex = right_of(index);
					if (leftIndex < size && Base::get(leftIndex) > Base::get(extreme))
						extreme = leftIndex;
					if (rightIndex < size && Base::get(rightIndex) > Base::get(extreme))
						extreme = rightIndex;
					if (extreme == index)
						return;

					Algorithms::swap(Base::get(index), Base::get(extreme));
					index = extreme;
				}
			}
		}

		constexpr void trickle_down_min(INDEX index)
			noexcept (std::is_nothrow_swappable_v<T>)
			requires (HEAP_TYPE == HeapType::BIDIRECTIONAL || HEAP_TYPE == HeapType::MINIMUM)
		{
			const INDEX size = Base::get_size();
			if constexpr (HEAP_TYPE == HeapType::BIDIRECTIONAL) {
				while (true) {
					const INDEX extreme = extreme_descendant<false>(index);
					if (extreme == index)
						return;

					const INDEX parentIndex = parent_of(extreme);
					if (parentIndex == index) {
						if (Base::get(extreme) < Base::get(index))
							Algorithms::swap(Base::get(extreme), Base::get(index));

						return;
					}
					if (Base::get(extreme) < Base::get(index)) {
						Algorithms::swap(Base::get(extreme), Base::get(index));
						if (Base::get(extreme) > Base::get(parentIndex))
							Algorithms::swap(Base::get(extreme), Base::get(parentIndex));

						index = extreme;
					}
				}
			}
			else {
				while (true) {
					INDEX extreme = index;

					const INDEX leftIndex = left_of(index);
					const INDEX rightIndex = right_of(index);
					if (leftIndex < size && Base::get(leftIndex) < Base::get(extreme))
						extreme = leftIndex;
					if (rightIndex < size && Base::get(rightIndex) < Base::get(extreme))
						extreme = rightIndex;
					if (extreme == index)
						return;

					Algorithms::swap(Base::get(index), Base::get(extreme));
					index = extreme;
				}
			}
		}

#pragma endregion

#pragma region Extract & Insert

		[[nodiscard]] constexpr bool can_extract() const noexcept {
			return !Base::is_empty();
		}

		constexpr T extract()
			noexcept (std::is_nothrow_move_constructible_v<T> && std::is_nothrow_swappable_v<T>)
		{
			T root = std::move(Base::get(0));

			const INDEX lastIndex = Base::get_size() - 1;
			if (lastIndex != 0)
				reconstruct_at(Base::get_data(), std::move(Base::get(lastIndex)));

			destroy_at(Base::get_data());

			--Base::size;
			if (Base::get_size() != 0)
				trickle_down(0);

			return root;
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