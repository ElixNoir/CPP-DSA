#pragma once

#include "Collection.hpp"

#include <bit>

namespace DSA {

	template <concepts::StoragePolicy StoragePolicy>
	class FenwickTree : public Collection<StoragePolicy, uintmax_t> {
	public:

		using Base = Collection<StoragePolicy, uintmax_t>;
		using typename Base::INDEX;
		using typename Base::T;

		constexpr static T BitCount = std::numeric_limits<T>::digits;
		constexpr static T BitShift = std::bit_width(BitCount - 1);

#pragma region Constructors & Destructors

		FenwickTree(T* data, size_t size) noexcept
			requires (!Base::is_dynamic)
		{
			for (size_t index = 0; index < size; ++index) {
				while (index < size) {
					Base::get(index) += data[index];
					index += index & -index;
				}
			}
			Base::set_size(size);
		}

		FenwickTree(T* data, size_t size)
			noexcept (concepts::NothrowAllocatedStorage<Base, T>)
			requires Base::is_dynamic
		: Base(size) {
			for (size_t index = 0; index < size; ++index) {
				while (index < size) {
					Base::get(index) += data[index];
					index += index & -index;
				}
			}
			Base::set_size(size);
		}

		//void destroy() noexcept {
			//Base::destroy();
		//}

#pragma endregion

#pragma region Methods

		constexpr void build_all(size_t length) noexcept {
			for (size_t index = 0; index < length; ++index)
				Base::get(index) = index & -index;
		}

		constexpr T find_kth(T length, T k) {
			INDEX bit = 1 << (BitCount - std::countl_zero(length) - 1);
			INDEX index = 0;
			while (bit > 0) {
				INDEX next = index + bit;
				T data = Base::get(next);
				if (next <= length && data < k) {
					k -= data;
					index = next;
				}
				bit >>= 1;
			}
			return index;
		}

		constexpr T prefix_sum(size_t index) const noexcept {
			T sum = 0;
			while (index > 0) {
				sum += Base::get(index);
				index -= index & -index;
			}
			return sum;
		}

		constexpr T range_sum(size_t left, size_t right) const noexcept {
			return prefix_sum(right) - prefix_sum(left - 1);
		}

		constexpr T point_query(size_t index) const noexcept {
			return range_sum(index, index);
		}

		constexpr void set_value(size_t index, T data) noexcept {
			update(index, data - point_query(index));
		}

		constexpr void update(size_t index, T delta) noexcept {
			const INDEX size = Base::get_size();
			while (index < size) {
				Base::get(index) += delta;
				index += index & -index;
			}
		}

#pragma endregion

	};

}
