#pragma once

#include "Collection.hpp"
#include "Iterator.hpp"

#include <bit>

namespace DSA {

	template <concepts::ContainerPolicy ContainerPolicy>
	class FenwickTree : public Collection<ContainerPolicy, uintmax_t> {
	public:

		using Base = Collection<ContainerPolicy, uintmax_t>;
		using typename Base::INDEX;
		using typename Base::T;

		constexpr static T bit_count = std::numeric_limits<T>::digits;
		constexpr static T bit_shift = std::bit_width(bit_count - 1);

		constexpr static bool is_dynamic = Base::is_dynamic;

#pragma region Constructors & Destructors

		FenwickTree(std::initializer_list<T> initializerList) noexcept : Base(initializerList.size()) {
			const INDEX size = Base::get_size();
			Base::zero();
			const auto iterator = initializerList.begin();
			for (INDEX index = 0; index < size; ++index) {
				Base::get(index) += *(iterator++);
				INDEX parentIndex = index + (index & -index);
				if (parentIndex < size)
					Base::get(parentIndex) += Base::get(index);
			}
		}

		template <concepts::InputIterator<T> ITERATOR>
		FenwickTree(ITERATOR iterator, INDEX count) noexcept : Base(count) {
			Base::zero();
			for (INDEX index = 0; index < count; ++index) {
				Base::get(index) += *(iterator++);
				INDEX parentIndex = index + (index & -index);
				if (parentIndex < count)
					Base::get(parentIndex) += Base::get(index);
			}
		}

		template <concepts::InputIterator<T> ITERATOR>
		constexpr void build(ITERATOR iterator) {
			const INDEX size = Base::get_size();

			INDEX index = 0;
			for (const auto& value : iterator) {
				if (index == size)
					break;

				Base::get(index) = value;
				++index;
			}

			for (index = 0; index < size; ++index) {
				INDEX parentIndex = index | (index + 1);
				if (parentIndex < size)
					Base::get(parentIndex) += Base::get(index);
			}
		}

		constexpr void build_ones(const INDEX length) noexcept {
			for (INDEX index = 0; index < length; ++index)
				Base::get(index) = index & -index;
		}

#pragma endregion

#pragma region Methods

#pragma region Lower Bound & Upper Bound

		[[nodiscard]] constexpr INDEX lower_bound(T target) const noexcept {
			INDEX index = 0;
			T sum{};

			// Start from the highest power of 2 <= n_
			const INDEX size = Base::get_size();
			for (INDEX step = highest_power_of_two(size); step > 0; step >>= 1) {
				INDEX next = index + step;
				const T value = Base::get(next);
				if (next <= size && sum + value < target) {
					index = next;
					sum += value;
				}
			}

			return index + 1;
		}

		[[nodiscard]] constexpr INDEX upper_bound(T target) const noexcept {
			INDEX index = 0;
			T sum{};

			const INDEX size = Base::get_size();
			for (INDEX step = highest_power_of_two(size); step > 0; step >>= 1) {
				INDEX next = index + step;
				const T value = Base::get(next);
				if (next <= size && sum + value <= target) {
					index = next;
					sum += value;
				}
			}

			return index + 1;
		}

#pragma endregion

#pragma region Mutation

		[[nodiscard]] constexpr bool can_set_value(INDEX index, T newValue) const noexcept {
			return newValue >= Base::get(index);
		}

		constexpr void set_value(INDEX index, T data) noexcept {
			update(index, data - Base::get(index));
		}

		constexpr void update(INDEX index, T delta) noexcept {
			const INDEX size = Base::get_size();
			while (index < size) {
				Base::get(index) += delta;
				index |= index + 1;
			}
		}

#pragma endregion

#pragma region Queries

		[[nodiscard]] constexpr T prefix_sum(INDEX index) const noexcept {
			T sum = Base::get(0);

			while (index > 0) {
				sum += Base::get(index);
				index -= index & -index;
			}

			return sum;
		}

		[[nodiscard]] constexpr T range_sum(INDEX left, INDEX right) const noexcept {
			return prefix_sum(right) - prefix_sum(left);
		}

		[[nodiscard]] constexpr T total_sum() const noexcept {
			T sum = Base::get(0);

			const INDEX size = Base::get_size();
			for (INDEX index = size; index > 0; index -= (index & -index))
				sum += Base::get(index);

			return sum;
		}

#pragma endregion

#pragma endregion

	};

}