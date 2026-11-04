#pragma once

/*template <typename T>
class SegmentTree {
protected:

	T* data;

	const T identity;
	size_t size;

public:

#pragma region Constructors & Destructors

	SegmentTree(const T& id, size_t n) : identity(id), size(1 << bit_width(n)) {
		// local self = table_create(2 * size, identity) :: SegmentTree
		// table_move(array, 1, length, size, self)
		for (size_t index = size - 1; index > 0; --index)
			data[index] = merge(data[index << 1], data[(index << 1) + 1]);
	}

#pragma endregion

#pragma region Methods

	constexpr T& query(size_t left, size_t right) noexcept {
		T& resLeft = identity;
		T& resRight = resLeft;

		left += size - 1;
		right += size - 1;

		while (left <= right) {
			if ((left & 1) == 1) {
				resLeft = merge(resLeft, data[left])
					++left;
			}

			if ((right & 1) == 0) {
				resRight = merge(data[right], resRight);
				--right;
			}

			left >>= 1;
			right >>= 1;
		}

		return merge(resLeft, resRight);
	}

	constexpr void update(size_t index, T& value) noexcept {
		const T* data = Base::get_data();

		index += size - 1;
		data[index] = value;

		index = ensure_even(index);
		while (index >= 1) {
			data[index] = merge(data[index], data[index + 1]);
			index >>= 1;
		}
	}

#pragma endregion

};*/