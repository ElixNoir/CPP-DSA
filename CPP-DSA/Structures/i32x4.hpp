#pragma once

#include "SIMD.hpp"

/*

template <bool HAS_SSE2 = has_sse2>
struct i32x4 { //! incomplete

	using T = std::conditional_t<HAS_SSE2, __m128i, std::array<int32_t, 4>>;

	T data;

#pragma region Constructors & Destructors

	i32x4() noexcept = default;

	[[nodiscard]] constexpr i32x4(const int32_t value) noexcept : data(_mm_set1_epi32(value)) {}

	[[nodiscard]] constexpr i32x4(const int32_t e0, const int32_t e1, const int32_t e2, const int32_t e3)
		noexcept : data(_mm_setr_epi32(e0, e1, e2, e3)) {}

	[[nodiscard]] constexpr i32x4(T value) noexcept : data(value) {}

#pragma endregion

#pragma region Methods

#pragma region Arithmetic

	/// <summary>
	/// Add packed 32-bit integers in <code>this</code> and <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 add(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_add_epi32(data, other);
		else
			return { data[0] + other[0], data[1] + other[1],
				data[2] + other[2], data[3] + other[3] };
	}

	/// <summary>
	/// Divide packed 32-bit integers in <code>this</code> by packed elements in <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 div(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_div_ps(data, other); //! incomplete
		else
			return { data[0] / other[0], data[1] / other[1], data[2] / other[2], data[3] / other[3] };
	}

	/// <summary>
	/// Multiply packed 32-bit integers in <code>this</code> and <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 mul(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_mul_epu32(data, other);
		else
			return { data[0] * other[0], data[1] * other[1], data[2] * other[2], data[3] * other[3] };
	}

	/// <summary>
	/// Subtract packed 32-bit integers in <code>other</code> from packed 32-bit integers in <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 sub(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_sub_epi32(data, other);
		else
			return { data[0] - other[0], data[1] - other[1], data[2] - other[2], data[3] - other[3] };
	}

#pragma endregion

#pragma region Comparison

	/// <summary>
	/// Compare packed 32-bit integers in <code>this</code> and <code>other</code> for equality.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpeq_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpeq_ps(data, other);
		else {
			const int32_t set = DSA::dword::set().f;
			return {
				data[0] == other[0] ? set : 0,
				data[1] == other[1] ? set : 0,
				data[2] == other[2] ? set : 0,
				data[3] == other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower 32-bit integers in <code>this</code> and <code>other</code> for equality, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpeq_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpeq_ss(data, other);
		else
			return { data[0] == other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed 32-bit integers in <code>this</code> and <code>other</code> for greater-than-or-equal.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpge_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpge_ps(data, other);
		else {
			const int32_t set = DSA::dword::set().f;
			return {
				data[0] >= other[0] ? set : 0,
				data[1] >= other[1] ? set : 0,
				data[2] >= other[2] ? set : 0,
				data[3] >= other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower 32-bit integers in <code>this</code> and <code>other</code> for greater-than-or-equal, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpge_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpge_ss(data, other);
		else
			return { data[0] >= other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed 32-bit integers in <code>this</code> and <code>other</code> for greater-than.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpgt_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpgt_ps(data, other);
		else {
			const int32_t set = DSA::dword::set().f;
			return {
				data[0] > other[0] ? set : 0,
				data[1] > other[1] ? set : 0,
				data[2] > other[2] ? set : 0,
				data[3] > other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower 32-bit integers in <code>this</code> and <code>other</code> for greater-than, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpgt_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpgt_ss(data, other);
		else
			return { data[0] > other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed 32-bit integers in <code>this</code> and <code>other</code> for less-than-or-equal.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmple_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmple_ps(data, other);
		else {
			const int32_t set = DSA::dword::set().f;
			return {
				data[0] <= other[0] ? set : 0,
				data[1] <= other[1] ? set : 0,
				data[2] <= other[2] ? set : 0,
				data[3] <= other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower 32-bit integers in <code>this</code> and <code>other</code> for less-than-or-equal, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmple_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmple_ss(data, other);
		else
			return { data[0] <= other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed 32-bit integers in <code>this</code> and <code>other</code> for less-than.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmplt_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmplt_ps(data, other);
		else {
			const int32_t set = DSA::dword::set().f;
			return {
				data[0] < other[0] ? set : 0,
				data[1] < other[1] ? set : 0,
				data[2] < other[2] ? set : 0,
				data[3] < other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower 32-bit integers in <code>this</code> and <code>other</code> for less-than, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmplt_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmplt_ss(data, other);
		else
			return { data[0] < other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed 32-bit integers in <code>this</code> and <code>other</code> for not-equal.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpneq_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpneq_ps(data, other);
		else {
			const int32_t set = DSA::dword::set().f;
			return {
				data[0] != other[0] ? set : 0,
				data[1] != other[1] ? set : 0,
				data[2] != other[2] ? set : 0,
				data[3] != other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower 32-bit integers in <code>this</code> and <code>other</code> for not-equal, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpneq_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpneq_ss(data, other);
		else
			return { data[0] != other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed 32-bit integers in <code>this</code> and <code>other</code> for not-greater-than-or-equal.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpnge_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpnge_ps(data, other);
		else {
			const int32_t set = DSA::dword::set().f;
			return {
				data[0] < other[0] ? set : 0,
				data[1] < other[1] ? set : 0,
				data[2] < other[2] ? set : 0,
				data[3] < other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower 32-bit integers in <code>this</code> and <code>other</code> for not-greater-than-or-equal, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpnge_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpnge_ss(data, other);
		else
			return { data[0] < other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed 32-bit integers in <code>this</code> and <code>other</code> for not-greater-than.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpngt_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpngt_ps(data, other);
		else {
			const int32_t set = DSA::dword::set().f;
			return {
				data[0] <= other[0] ? set : 0,
				data[1] <= other[1] ? set : 0,
				data[2] <= other[2] ? set : 0,
				data[3] <= other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower 32-bit integers in <code>this</code> and <code>other</code> for not-greater-than, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpngt_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpngt_ss(data, other);
		else
			return { data[0] <= other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed 32-bit integers in <code>this</code> and <code>other</code> for not-less-than-or-equal.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpnle_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpnle_ps(data, other);
		else {
			const int32_t set = DSA::dword::set().f;
			return {
				data[0] > other[0] ? set : 0,
				data[1] > other[1] ? set : 0,
				data[2] > other[2] ? set : 0,
				data[3] > other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower 32-bit integers in <code>this</code> and <code>other</code> for not-less-than-or-equal, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpnle_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpnle_ss(data, other);
		else
			return { data[0] > other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed 32-bit integers in <code>this</code> and <code>other</code> for not-less-than.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpnlt_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpnlt_ps(data, other);
		else {
			const int32_t set = DSA::dword::set().f;
			return {
				data[0] >= other[0] ? set : 0,
				data[1] >= other[1] ? set : 0,
				data[2] >= other[2] ? set : 0,
				data[3] >= other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower 32-bit integers in <code>this</code> and <code>other</code> for not-less-than, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpnlt_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpnlt_ss(data, other);
		else
			return { data[0] >= other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed 32-bit integers in <code>this</code> and <code>other</code> to see if neither is NaN.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpord_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpord_ps(data, other);
		else {
			const int32_t set = DSA::dword::set().f;
			return {
				data[0] == other[0] ? set : 0,
				data[1] == other[1] ? set : 0,
				data[2] == other[2] ? set : 0,
				data[3] == other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower 32-bit integers in <code>this</code> and <code>other</code> to see if neither is NaN and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpord_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpord_ss(data, other);
		else
			return { Algorithms::Math::is_nan(data[0]) && Algorithms::Math::is_nan(other[0])
				? 0 : DSA::dword::set().f, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed 32-bit integers in <code>this</code> and <code>other</code> to see if either is NaN.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpunord_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpunord_ps(data, other);
		else {
			const int32_t set = DSA::dword::set().f;
			return {
				data[0] == other[0] ? set : 0,
				data[1] == other[1] ? set : 0,
				data[2] == other[2] ? set : 0,
				data[3] == other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower 32-bit integers in <code>this</code> and <code>other</code> to see if either is NaN and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 cmpunord_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpunord_ss(data, other);
		else
			return { Algorithms::Math::is_nan(data[0]) && Algorithms::Math::is_nan(other[0])
				? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare the lower 32-bit integer in <code>this</code> and <code>other</code> for equality.
	/// </summary>
	[[nodiscard]] constexpr int comieq_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_comieq_ss(data, other);
		else
			return data[0] == other[0];
	}

	/// <summary>
	/// Compare the lower 32-bit integer in <code>this</code> and <code>other</code> for greater-than-or-equal.
	/// </summary>
	[[nodiscard]] constexpr int comige_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_comige_ss(data, other);
		else
			return data[0] >= other[0];
	}

	/// <summary>
	/// Compare the lower 32-bit integer in <code>this</code> and <code>other</code> for greater-than.
	/// </summary>
	[[nodiscard]] constexpr int comigt_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_comigt_ss(data, other);
		else
			return data[0] > other[0];
	}

	/// <summary>
	/// Compare the lower 32-bit integer in <code>this</code> and <code>other</code> for less-than-or-equal.
	/// </summary>
	[[nodiscard]] constexpr int comile_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_comile_ss(data, other);
		else
			return data[0] <= other[0];
	}

	/// <summary>
	/// Compare the lower 32-bit integer in <code>this</code> and <code>other</code> for less-than.
	/// </summary>
	[[nodiscard]] constexpr int comilt_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_comilt_ss(data, other);
		else
			return data[0] < other[0];
	}

	/// <summary>
	/// Compare the lower 32-bit integer in <code>this</code> and <code>other</code> for not-equal
	/// </summary>
	[[nodiscard]] constexpr int comineq_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_comineq_ss(data, other);
		else
			return data[0] != other[0]; //? qnan exception?
	}

	/// <summary>
	/// Compare the lower 32-bit integer in <code>this</code> and <code>other</code> for equality. i32x4 instruction will not signal an exception for QNaNs.
	/// </summary>
	[[nodiscard]] constexpr int ucomieq_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_ucomieq_ss(data, other);
		else
			return data[0] == other[0];
	}

	/// <summary>
	/// Compare the lower 32-bit integer in <code>this</code> and <code>other</code> for greater-than-or-equal. i32x4 instruction will not signal an exception for QNaNs.
	/// </summary>
	[[nodiscard]] constexpr int ucomige_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_ucomige_ss(data, other);
		else
			return data[0] >= other[0];
	}

	/// <summary>
	/// Compare the lower 32-bit integer in <code>this</code> and <code>other</code> for greater-than. i32x4 instruction will not signal an exception for QNaNs.
	/// </summary>
	[[nodiscard]] constexpr int ucomigt_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_ucomigt_ss(data, other);
		else
			return data[0] > other[0];
	}

	/// <summary>
	/// Compare the lower 32-bit integer in <code>this</code> and <code>other</code> for less-than-or-equal. i32x4 instruction will not signal an exception for QNaNs.
	/// </summary>
	[[nodiscard]] constexpr int ucomile_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_ucomile_ss(data, other);
		else
			return data[0] <= other[0];
	}

	/// <summary>
	/// Compare the lower 32-bit integer in <code>this</code> and <code>other</code> for less-than. i32x4 instruction will not signal an exception for QNaNs.
	/// </summary>
	[[nodiscard]] constexpr int ucomilt_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_ucomilt_ss(data, other);
		else
			return data[0] < other[0];
	}

	/// <summary>
	/// Compare the lower 32-bit integer in <code>this</code> and <code>other</code> for not-equal. i32x4 instruction will not signal an exception for QNaNs.
	/// </summary>
	[[nodiscard]] constexpr int ucomineq_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_ucomineq_ss(data, other);
		else
			return data[0] != other[0];
	}

#pragma endregion

#pragma region Logical

	/// <summary>
	/// Compute the bitwise AND of packed 32-bit integers in <code>this</code> and <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 and_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_and_ps(data, other);
		else
			return {
				data[0] & other[0],
				data[1] & other[1],
				data[2] & other[2],
				data[3] & other[3]
			};
	}

	/// <summary>
	/// Compute the bitwise AND-NOT of packed 32-bit integers in <code>this</code> and <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 andnot_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_andnot_ps(data, other);
		else
			return {
				data[0] & ~other[0],
				data[1] & ~other[1],
				data[2] & ~other[2],
				data[3] & ~other[3]
			};
	}

	/// <summary>
	/// Compute the bitwise OR of packed 32-bit integers in <code>this</code> and <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 or_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_or_ps(data, other);
		else
			return {
				data[0] | other[0],
				data[1] | other[1],
				data[2] | other[2],
				data[3] | other[3]
			};
	}

	/// <summary>
	/// Compute the bitwise XOR of packed 32-bit integers in <code>this</code> and <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 xor_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_xor_ps(data, other);
		else
			return {
				data[0] ^ other[0],
				data[1] ^ other[1],
				data[2] ^ other[2],
				data[3] ^ other[3]
			};
	}

#pragma endregion

#pragma region Miscellaneous

	/// <summary>
	/// Compare packed 32-bit integers in <code>this</code> and <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 max_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_max_ps(data, other);
		else
			return {
				data[0] >= other[0] ? data[0] : other[0],
				data[1] >= other[1] ? data[1] : other[1],
				data[2] >= other[2] ? data[2] : other[2],
				data[3] >= other[3] ? data[3] : other[3]
			};
	}

	/// <summary>
	/// Compare the lower 32-bit integers in <code>this</code> and <code>other</code> and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 max_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_max_ss(data, other);
		else
			return { data[0] >= other[0] ? data[0] : other[0], data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed 32-bit integers in <code>this</code> and <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 min_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_min_ps(data, other);
		else
			return {
				data[0] <= other[0] ? data[0] : other[0],
				data[1] <= other[1] ? data[1] : other[1],
				data[2] <= other[2] ? data[2] : other[2],
				data[3] <= other[3] ? data[3] : other[3]
			};
	}

	/// <summary>
	/// Compare the lower 32-bit integers in <code>this</code> and <code>other</code> and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 min_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_min_ss(data, other);
		else
			return { data[0] <= other[0] ? data[0] : other[0], data[1], data[2], data[3] };
	}

	/// <summary>
	/// Set each bit of mask <code>this</code> based on the most significant bit of the corresponding packed 32-bit integer in <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr int movemask_ps() const noexcept {
		if constexpr (has_sse)
			return _mm_movemask_ps(data);
		else
			return ((data[0] & 0x80000000) | ((data[1] & 0x80000000) >> 1)
				| ((data[2] & 0x80000000) >> 2) | ((data[3] & 0x80000000) >> 3)) >> 28; //? is this even correct?
	}

#pragma endregion

#pragma region Move

	/// <summary>
	/// Move the lower 32-bit integer from <code>other</code> and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 move_ss(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_move_ss(data, other);
		else
			return { other[0], data[1], data[2], data[3] };
	}

	/// <summary>
	/// Move the upper 2 32-bit integers from <code>other</code> to the lower 2 elements and copy the upper 2 elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 movehl_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_movehl_ps(data, other);
		else
			return { other[2], other[3], data[2], data[3] };
	}

	/// <summary>
	/// Move the lower 2 32-bit integers from <code>other</code> to the upper 2 elements and copy the lower 2 elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 movelh_ps(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_movelh_ps(data, other);
		else
			return { data[0], data[1], other[0], other[1] };
	}

#pragma endregion

#pragma region Set

	/// <summary>
	/// Broadcast 32-bit integer to all elements of this.  This intrinsic may generate vpbroadcastd.
	/// </summary>
	constexpr void broadcast(const int32_t value) noexcept {
		if constexpr (has_sse)
			data = _mm_set1_epi32(value);
		else
			data = { value, value, value, value };
	}

	/// <summary>
	/// Set packed 32-bit integers in <code>this</code> with the supplied values.
	/// </summary>
	constexpr void set(const int32_t e0, const int32_t e1, const int32_t e2, const int32_t e3) noexcept {
		if constexpr (has_sse)
			data = _mm_setr_epi32(e0, e1, e2, e3);
		else
			data = { e0, e1, e2, e3 };
	}

	/// <summary>
	/// Set all elements to zero in <code>this</code>.
	/// </summary>
	constexpr void zero() noexcept {
		if constexpr (has_sse)
			data = _mm_setzero_si128();
		else
			data = { 0, 0, 0, 0 };
	}

#pragma endregion

#pragma region Store

	/// <summary>
	/// Store 128-bits (composed of 4 packed 32-bit integers) from <code>this</code> into destination. destination must be aligned on a 16-byte boundary or a general-protection exception may be generated.
	/// </summary>
	constexpr void store_ps(int32_t* destination) const noexcept {
		if constexpr (has_sse)
			_mm_store_ps(destination, data);
		else
			std::memcpy(destination, data, sizeof(data));
	}

	/// <summary>
	/// Store the lower 32-bit integer from <code>this</code> into 4 contiguous elements in memory. destination must be aligned on a 16-byte boundary or a general-protection exception may be generated.
	/// </summary>
	constexpr void store_ps1(int32_t* destination) const noexcept {
		if constexpr (has_sse)
			_mm_store_ps1(destination, data);
		else {
			destination[0] = data[0];
			destination[1] = data[1];
			destination[2] = data[2];
			destination[3] = data[3];
		}
	}

	/// <summary>
	/// Store the lower 32-bit integer from <code>this</code> into destination. destination does not need to be aligned on any particular boundary.
	/// </summary>
	constexpr void store_ss(int32_t* destination) const noexcept {
		if constexpr (has_sse)
			_mm_store_ss(destination, data);
		else
			*destination = data[0];
	}

	/// <summary>
	/// Store 4 32-bit integers from <code>this</code> into destination in reverse order. destination must be aligned on a 16-byte boundary or a general-protection exception may be generated.
	/// </summary>
	constexpr void storer_ps(int32_t* destination) const noexcept {
		if constexpr (has_sse)
			_mm_storer_ps(destination, data);
		else {
			destination[0] = data[3];
			destination[1] = data[2];
			destination[2] = data[1];
			destination[3] = data[0];
		}
	}

	/// <summary>
	/// Store 128-bits (composed of 4 packed 32-bit integers) from <code>this</code> into destination. destination does not need to be aligned on any particular boundary.
	/// </summary>
	constexpr void storeu_ps(int32_t* destination) const noexcept {
		if constexpr (has_sse)
			_mm_storeu_ps(destination, data);
		else
			std::memcpy(destination, data, sizeof(data));
	}

	/// <summary>
	/// Store 128-bits (composed of 4 packed 32-bit integers) from <code>this</code> into destination using a non-temporal memory hint. destination must be aligned on a 16-byte boundary or a general-protection exception may be generated.
	/// </summary>
	constexpr void stream_ps(int32_t* destination) const noexcept {
		if constexpr (has_sse)
			_mm_stream_ps(destination, data);
		else
			return; //! incomplete
	}

#pragma endregion

#pragma endregion

#pragma region Other

	[[nodiscard]] constexpr uintmax_t hash() const noexcept {
		return Algorithms::hash(data);
	}

	constexpr void swap(i32x4& other) noexcept {
		T temporary = other.data;
		other.data = data;
		data = temporary;
	}

#pragma endregion

#pragma region Operators

	[[nodiscard]] explicit constexpr operator __m128() const noexcept {
		return _mm_castsi128_ps(data);
	}

	[[nodiscard]] explicit constexpr operator __m128d() const noexcept {
		return _mm_castsi128_pd(data);
	}

	[[nodiscard]] explicit constexpr operator T() const noexcept {
		return data;
	}

	[[nodiscard]] constexpr i32x4 operator+(const i32x4 other) const noexcept {
		return add_ps(other);
	}

	[[nodiscard]] constexpr i32x4 operator/(const i32x4 other) const noexcept {
		return div_ps(other);
	}

	[[nodiscard]] constexpr i32x4 operator*(const i32x4 other) const noexcept {
		return mul_ps(other);
	}

	[[nodiscard]] constexpr i32x4 operator-(const i32x4 other) const noexcept {
		return sub_ps(other);
	}

	[[nodiscard]] constexpr operator int32_t*() noexcept {
		return reinterpret_cast_single_pointer<int32_t>(data);
	}

	[[nodiscard]] constexpr operator const int32_t*() const noexcept {
		return reinterpret_cast_single_pointer<const int32_t>(data);
	}

	friend std::ostream& operator<<(std::ostream& ostream, const i32x4 t) {
		ostream << '{' << t[0] << ", " << t[1] << ", " << t[2] << ", " << t[3] << '}';
		return ostream;
	}

#pragma endregion

};

*/