#pragma once

#include "Collection.hpp"
#include "swap.hpp"

#include <iostream>

namespace DSA {

	enum HeapType : uint8_t {

		MAX_HEAP,
		MIN_HEAP

	};

	template <concepts::StorageConfiguration ContainerConfiguration, typename T, HeapType HEAP_TYPE = HeapType::MAX_HEAP>
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

		// need the memory helpers lol

#pragma endregion

	public:

#pragma region Constructors & Destructors

		using Base::Base;

#pragma endregion

#pragma region Methods

#pragma region Add & Remove

		constexpr void add(const T& value) noexcept {
			INDEX index = Base::get_size();
			Base::get(index) = value;

			++Base::size;
			bubble_up(index);
		}

		constexpr void add(T&& value) noexcept {
			INDEX index = Base::get_size();
			Base::get(index) = std::move(value);

			++Base::size;
			bubble_up(index);
		}

		constexpr void remove(INDEX index) noexcept {
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

		constexpr void bubble_up(INDEX index) noexcept {
			if constexpr (HEAP_TYPE == MAX_HEAP) {
				while (index != 0 && Base::get(parent(index)) < Base::get(index)) {
					const INDEX newIndex = parent(index);
					swap(Base::get(index), Base::get(newIndex));
					index = newIndex;
				}
			}
			else {
				while (index != 0 && Base::get(parent(index)) > Base::get(index)) {
					const INDEX newIndex = parent(index);
					swap(Base::get(index), Base::get(newIndex));
					index = newIndex;
				}
			}
		}

		[[nodiscard]] constexpr bool can_extract() const noexcept {
			return !Base::is_empty();
		}

		constexpr T extract() noexcept {
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

		constexpr void heapify(INDEX i) noexcept {
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

		constexpr void insert(const T& value) noexcept {
			add(value);
		}

		constexpr void insert(T&& value) noexcept {
			add(std::move(value));
		}

#pragma endregion

#pragma region Memory Management
		
#pragma endregion

#pragma region Operators

		friend std::ostream& operator<<(std::ostream& ostream, const Heap& heap) {
			INDEX power = 0;
			INDEX value = 1;
			for (INDEX index = 0; index < heap.get_size(); ++index) {
				if (index == value) {
					ostream << '\n';
					value += (1 << ++power);
				}
				ostream << heap.get(index) << "  ";
			}

			ostream << '\n';

			return ostream;
		}

#pragma endregion

#pragma endregion

	};

}