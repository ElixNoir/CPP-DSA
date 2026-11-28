#pragma once

#include "Collection.hpp"

#include <bit>

namespace DSA {

	template <concepts::ContainerPolicy ContainerPolicy, typename T>
	class SegmentTree : public Collection<ContainerPolicy, T> { //! incomplete
	public:

		using Base = Collection<ContainerPolicy, T>;
		using typename Base::INDEX;

	protected:

		const T identity;

#pragma region Methods

		[[nodiscard]] constexpr T merge() const noexcept { //! incomplete
			return T{};
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		SegmentTree(const T& newIdentity, size_t count) : Base(std::bit_ceil(count)), identity(newIdentity) {
			const INDEX size = Base::get_size();
			Base::fill_to_size(newIdentity);
			T* const data = Base::get_data();
			for (size_t index = size - 1; index > 0; --index)
				data[index] = merge(data[index << 1], data[(index << 1) + 1]);
		}

#pragma endregion

#pragma region Methods

#pragma region Getters

		[[nodiscard]] constexpr T get_identity() const noexcept {
			return identity;
		}

#pragma endregion

#pragma region Mutation

		constexpr void update(size_t index, const T& value) noexcept {
			index += Base::get_size();

			T* const data = Base::get_data();
			copy_construct_at(data + index, value);
			while (index > 1) {
				index >>= 1;
				copy_construct_at(data + index, merge(data[index << 1], data[(index << 1) | 1]));
			}
		}

#pragma endregion

#pragma region Queries

		[[nodiscard]] constexpr T query(size_t left, size_t right) noexcept {
			T resultLeft = identity;
			T resultRight = resultLeft;

			const INDEX size = Base::get_size();
			left += size;
			right += size + 1;

			T* const data = Base::get_data();
			while (left < right) {
				if (left & 1)
					resultLeft = merge(resultLeft, data[left++]);

				if (right & 1)
					resultRight = merge(data[--right], resultRight);

				left >>= 1;
				right >>= 1;
			}

			return merge(resultLeft, resultRight);
		}

#pragma endregion

		/*
		Point accumulation
		Point assignment
		Range query
		Root aggregate query
		*/

		// Persistent/Sparse/Dual/Beats/2D/Lazy (?)

#pragma endregion

	};

}