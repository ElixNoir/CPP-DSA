#pragma once

#include "Bitset.hpp"
#include "IncrementalContainer.hpp"
#include "Stack.hpp"

namespace DSA {

	template <concepts::Container CONTAINER>
	class Pool : public IncrementalContainer<CONTAINER> {
	public:

		using Base = CONTAINER;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

		using Bitmap = std::conditional_t<
			concepts::ContainerTraits<CONTAINER>::dynamic,
			Bitmask<uintmax_t>*,
			Bitmask<uintmax_t>[CONTAINER::capacity]
		>;

		using Base::size;

		Stack<DynamicContainer<T, INDEX>> free_stack;

		void helper_destroy() {
			Bitset bitset;

			INDEX count = Base::get_size();

			for (INDEX index : free_stack)
				bitset.get(index >> Bitmask<uintmax_t>::BitShift).set(index & (BitCount - 1));

			for (INDEX index : bitset) {
				if (count == 0)
					break;
				destroy_at(Base::get_data() + index);
				count--;
			}
		}

	public:

#pragma region Constructors & Destructors

		~Pool() requires (!std::is_trivially_destructible_v<T>) {
			helper_destroy();
		}

#pragma endregion

#pragma region Methods

		constexpr INDEX add(const T& value) noexcept {
			INDEX index;
			if (!free_stack.is_empty())
				index = free_stack.pop();
			else {
				index = free_stack.get_size();
				free_stack.push(value);
			}

			size++;

			return index;
		}

		constexpr INDEX add(const T&& value) noexcept {
			INDEX index;
			if (!free_stack.is_empty())
				index = free_stack.pop();
			else {
				index = free_stack.get_size();
				free_stack.push(value);
			}

			size++;

			return index;
		}

		constexpr void remove(INDEX index) noexcept {
			free_stack.push(index);
			size--;
		}

#pragma region Pool

		constexpr INDEX allocate(const T& value) noexcept {
			return add(value);
		}

		constexpr INDEX allocate(const T&& value) noexcept {
			return add(value);
		}

		constexpr void deallocate(INDEX index) noexcept {
			remove(index);
		}

#pragma region Memory Management

		constexpr void empty() noexcept {
			helper_destroy();
			free_stack.empty();
			size = 0;
		}

		constexpr void reserve(INDEX capacity) noexcept {
			free_stack.reserve(capacity);
		}

#pragma endregion

#pragma endregion

	};

}