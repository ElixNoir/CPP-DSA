#pragma once

#include "SIMD.hpp"

template <bool HAS_SSE>
struct f32x4 {

	using T = std::conditional_t<HAS_SSE, __m128, std::array<float, 4>>;

	T data;

#pragma region Constructors & Destructors

	f32x4() noexcept = default;

	[[nodiscard]] constexpr f32x4(const float value) noexcept : data(_mm_set_ps1(value)) {}

	[[nodiscard]] constexpr f32x4(const float e0, const float e1, const float e2, const float e3)
		noexcept : data(_mm_setr_ps(e0, e1, e2, e3)) {}

	[[nodiscard]] constexpr f32x4(T value) noexcept : data(value) {}

#pragma endregion

#pragma region Methods

#pragma region SSE

#pragma region Arithmetic

	/// <summary>
	/// Add packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 add_ps(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_add_ps(data, other);
		else
			return { data[0] + other[0], data[1] + other[1], data[2] + other[2], data[3] + other[3] };
	}

	/// <summary>
	/// Add the lower single-precision (32-bit) floating-point element in <code>this</code> and <code>other</code> and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 add_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_add_ss(data, other);
		else {
			f32x4 result;
			std::memcpy(result, other, sizeof(T));
			result[0] = data[0] + other[0];
			return result;
		}
	}

	/// <summary>
	/// Divide packed single-precision (32-bit) floating-point elements in <code>this</code> by packed elements in <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 div_ps(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_div_ps(data, other);
		else
			return { data[0] / other[0], data[1] / other[1], data[2] / other[2], data[3] / other[3] };
	}

	/// <summary>
	/// Divide the lower single-precision (32-bit) floating-point element in <code>this</code> by the lower single-precision (32-bit) floating-point element in <code>other</code> and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 div_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_div_ss(data, other);
		else {
			f32x4 result;
			std::memcpy(result, other, sizeof(T));
			result[0] = data[0] / other[0];
			return result;
		}
	}

	/// <summary>
	/// Multiply packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 mul_ps(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_mul_ps(data, other);
		else
			return { data[0] * other[0], data[1] * other[1], data[2] * other[2], data[3] * other[3] };
	}

	/// <summary>
	/// Multiply the lower single-precision (32-bit) floating-point element in <code>this</code> and <code>other</code>, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 mul_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_mul_ss(data, other);
		else {
			f32x4 result;
			std::memcpy(result, other, sizeof(T));
			result[0] = data[0] * other[0];
			return result;
		}
	}

	/// <summary>
	/// Compute the approximate reciprocal of packed single-precision (32-bit) floating-point elements in <code>this</code>. The maximum relative error for this approximation is less than 1.5 * 2^-12.
	/// </summary>
	[[nodiscard]] constexpr f32x4 rcp_ps() const noexcept {
		if constexpr (has_sse)
			return _mm_rcp_ps(data);
		else
			return { 1 / data[0], 1 / data[1], 1 / data[2], 1 / data[3] };
	}

	/// <summary>
	/// Compute the approximate reciprocal of the lower single-precision (32-bit) floating-point element in <code>this</code> and copy the upper 3 packed elements from <code>this</code>. The maximum relative error for this approximation is less than 1.5*2^-12.
	/// </summary>
	[[nodiscard]] constexpr f32x4 rcp_ss() const noexcept {
		if constexpr (has_sse)
			return _mm_rcp_ss(data);
		else {
			f32x4 result;
			std::memcpy(result, data, sizeof(T));
			result[0] = 1.0 / data[0];
			return result;
		}
	}

	/// <summary>
	/// Compute the approximate reciprocal square root of packed single-precision (32-bit) floating-point elements in <code>this</code>. The maximum relative error for this approximation is less than 1.5*2^-12.
	/// </summary>
	[[nodiscard]] constexpr f32x4 rsqrt_ps() const noexcept {
		if constexpr (has_sse)
			return _mm_rsqrt_ps(data);
		else
			return {
				1.0 / Algorithms::Math::square_root(data[0]),
				1.0 / Algorithms::Math::square_root(data[1]),
				1.0 / Algorithms::Math::square_root(data[2]),
				1.0 / Algorithms::Math::square_root(data[3])
			};
	}

	/// <summary>
	/// Compute the approximate reciprocal square root of the lower single-precision (32-bit) floating-point element in <code>this</code> and copy the upper 3 packed elements from <code>this</code>. The maximum relative error for this approximation is less than 1.5*2^-12.
	/// </summary>
	[[nodiscard]] constexpr f32x4 rsqrt_ss() const noexcept {
		if constexpr (has_sse)
			return _mm_rsqrt_ss(data);
		else {
			f32x4 result;
			std::memcpy(result, data, sizeof(T));
			result[0] = 1.0 / Algorithms::Math::square_root(data[0]);
			return result;
		}
	}

	/// <summary>
	/// Compute the square root of packed single-precision (32-bit) floating-point elements in <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 sqrt_ps() const noexcept {
		if constexpr (has_sse)
			return _mm_sqrt_ps(data);
		else
			return {
				Algorithms::Math::square_root(data[0]),
				Algorithms::Math::square_root(data[1]),
				Algorithms::Math::square_root(data[2]),
				Algorithms::Math::square_root(data[3])
			};
	}

	/// <summary>
	/// Compute the square root of the lower single-precision (32-bit) floating-point element in <code>this</code> and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 sqrt_ss() const noexcept {
		if constexpr (has_sse)
			return _mm_sqrt_ss(data);
		else {
			f32x4 result;
			std::memcpy(result, data, sizeof(T));
			result[0] = Algorithms::Math::square_root(data[0]);
			return result;
		}
	}

	/// <summary>
	/// Subtract packed single-precision (32-bit) floating-point elements in <code>other</code> from packed single-precision (32-bit) floating-point elements in <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 sub_ps(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_sub_ps(data, other);
		else
			return { data[0] - other[0], data[1] - other[1], data[2] - other[2], data[3] - other[3] };
	}

	/// <summary>
	/// Subtract the lower single-precision (32-bit) floating-point element in <code>other</code> from the lower single-precision (32-bit) floating-point element in <code>this</code>, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 sub_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_sub_ss(data, other);
		else {
			f32x4 result;
			std::memcpy(result, other, sizeof(T));
			result[0] = data[0] - other[0];
			return result;
		}
	}

#pragma endregion

#pragma region Comparison

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for equality.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpeq_ps(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpeq_ps(data, other);
		else {
			const float set = DSA::dword::set().f;
			return {
				data[0] == other[0] ? set : 0,
				data[1] == other[1] ? set : 0,
				data[2] == other[2] ? set : 0,
				data[3] == other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for equality, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpeq_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpeq_ss(data, other);
		else
			return { data[0] == other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for greater-than-or-equal.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpge_ps(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpge_ps(data, other);
		else {
			const float set = DSA::dword::set().f;
			return {
				data[0] >= other[0] ? set : 0,
				data[1] >= other[1] ? set : 0,
				data[2] >= other[2] ? set : 0,
				data[3] >= other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for greater-than-or-equal, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpge_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpge_ss(data, other);
		else
			return { data[0] >= other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for greater-than.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpgt_ps(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpgt_ps(data, other);
		else {
			const float set = DSA::dword::set().f;
			return {
				data[0] > other[0] ? set : 0,
				data[1] > other[1] ? set : 0,
				data[2] > other[2] ? set : 0,
				data[3] > other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for greater-than, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpgt_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpgt_ss(data, other);
		else
			return { data[0] > other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for less-than-or-equal.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmple_ps(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmple_ps(data, other);
		else {
			const float set = DSA::dword::set().f;
			return {
				data[0] <= other[0] ? set : 0,
				data[1] <= other[1] ? set : 0,
				data[2] <= other[2] ? set : 0,
				data[3] <= other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for less-than-or-equal, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmple_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmple_ss(data, other);
		else
			return { data[0] <= other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for less-than.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmplt_ps(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmplt_ps(data, other);
		else {
			const float set = DSA::dword::set().f;
			return {
				data[0] < other[0] ? set : 0,
				data[1] < other[1] ? set : 0,
				data[2] < other[2] ? set : 0,
				data[3] < other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for less-than, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmplt_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmplt_ss(data, other);
		else
			return { data[0] < other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for not-equal.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpneq_ps(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpneq_ps(data, other);
		else {
			const float set = DSA::dword::set().f;
			return {
				data[0] != other[0] ? set : 0,
				data[1] != other[1] ? set : 0,
				data[2] != other[2] ? set : 0,
				data[3] != other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for not-equal, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpneq_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpneq_ss(data, other);
		else
			return { data[0] != other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for not-greater-than-or-equal.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpnge_ps(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpnge_ps(data, other);
		else {
			const float set = DSA::dword::set().f;
			return {
				data[0] < other[0] ? set : 0,
				data[1] < other[1] ? set : 0,
				data[2] < other[2] ? set : 0,
				data[3] < other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for not-greater-than-or-equal, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpnge_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpnge_ss(data, other);
		else
			return { data[0] < other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for not-greater-than.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpngt_ps(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpngt_ps(data, other);
		else {
			const float set = DSA::dword::set().f;
			return {
				data[0] <= other[0] ? set : 0,
				data[1] <= other[1] ? set : 0,
				data[2] <= other[2] ? set : 0,
				data[3] <= other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for not-greater-than, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpngt_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpngt_ss(data, other);
		else
			return { data[0] <= other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for not-less-than-or-equal.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpnle_ps(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpnle_ps(data, other);
		else {
			const float set = DSA::dword::set().f;
			return {
				data[0] > other[0] ? set : 0,
				data[1] > other[1] ? set : 0,
				data[2] > other[2] ? set : 0,
				data[3] > other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for not-less-than-or-equal, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpnle_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpnle_ss(data, other);
		else
			return { data[0] > other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for not-less-than.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpnlt_ps(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpnlt_ps(data, other);
		else {
			const float set = DSA::dword::set().f;
			return {
				data[0] >= other[0] ? set : 0,
				data[1] >= other[1] ? set : 0,
				data[2] >= other[2] ? set : 0,
				data[3] >= other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> for not-less-than, and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpnlt_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpnlt_ss(data, other);
		else
			return { data[0] >= other[0] ? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> to see if neither is NaN.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpord_ps(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpord_ps(data, other);
		else {
			const float set = DSA::dword::set().f;
			return {
				data[0] == other[0] ? set : 0,
				data[1] == other[1] ? set : 0,
				data[2] == other[2] ? set : 0,
				data[3] == other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> to see if neither is NaN and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpord_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpord_ss(data, other);
		else
			return { Algorithms::Math::is_nan(data[0]) && Algorithms::Math::is_nan(other[0])
				? 0 : DSA::dword::set().f, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> to see if either is NaN.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpunord_ps(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpunord_ps(data, other);
		else {
			const float set = DSA::dword::set().f;
			return {
				data[0] == other[0] ? set : 0,
				data[1] == other[1] ? set : 0,
				data[2] == other[2] ? set : 0,
				data[3] == other[3] ? set : 0
			};
		}
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> to see if either is NaN and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 cmpunord_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpunord_ss(data, other);
		else
			return { Algorithms::Math::is_nan(data[0]) && Algorithms::Math::is_nan(other[0])
				? DSA::dword::set().f : 0, data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in <code>this</code> and <code>other</code> for equality.
	/// </summary>
	[[nodiscard]] constexpr int comieq_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_comieq_ss(data, other);
		else
			return data[0] == other[0];
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in <code>this</code> and <code>other</code> for greater-than-or-equal.
	/// </summary>
	[[nodiscard]] constexpr int comige_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_comige_ss(data, other);
		else
			return data[0] >= other[0];
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in <code>this</code> and <code>other</code> for greater-than.
	/// </summary>
	[[nodiscard]] constexpr int comigt_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_comigt_ss(data, other);
		else
			return data[0] > other[0];
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in <code>this</code> and <code>other</code> for less-than-or-equal.
	/// </summary>
	[[nodiscard]] constexpr int comile_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_comile_ss(data, other);
		else
			return data[0] <= other[0];
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in <code>this</code> and <code>other</code> for less-than.
	/// </summary>
	[[nodiscard]] constexpr int comilt_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_comilt_ss(data, other);
		else
			return data[0] < other[0];
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in <code>this</code> and <code>other</code> for not-equal
	/// </summary>
	[[nodiscard]] constexpr int comineq_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_comineq_ss(data, other);
		else
			return data[0] != other[0]; //? qnan exception?
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in <code>this</code> and <code>other</code> for equality. f32x4 instruction will not signal an exception for QNaNs.
	/// </summary>
	[[nodiscard]] constexpr int ucomieq_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_ucomieq_ss(data, other);
		else
			return data[0] == other[0];
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in <code>this</code> and <code>other</code> for greater-than-or-equal. f32x4 instruction will not signal an exception for QNaNs.
	/// </summary>
	[[nodiscard]] constexpr int ucomige_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_ucomige_ss(data, other);
		else
			return data[0] >= other[0];
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in <code>this</code> and <code>other</code> for greater-than. f32x4 instruction will not signal an exception for QNaNs.
	/// </summary>
	[[nodiscard]] constexpr int ucomigt_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_ucomigt_ss(data, other);
		else
			return data[0] > other[0];
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in <code>this</code> and <code>other</code> for less-than-or-equal. f32x4 instruction will not signal an exception for QNaNs.
	/// </summary>
	[[nodiscard]] constexpr int ucomile_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_ucomile_ss(data, other);
		else
			return data[0] <= other[0];
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in <code>this</code> and <code>other</code> for less-than. f32x4 instruction will not signal an exception for QNaNs.
	/// </summary>
	[[nodiscard]] constexpr int ucomilt_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_ucomilt_ss(data, other);
		else
			return data[0] < other[0];
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in <code>this</code> and <code>other</code> for not-equal. f32x4 instruction will not signal an exception for QNaNs.
	/// </summary>
	[[nodiscard]] constexpr int ucomineq_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_ucomineq_ss(data, other);
		else
			return data[0] != other[0];
	}

#pragma endregion

#pragma region Logical

	/// <summary>
	/// Compute the bitwise AND of packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 and_ps(f32x4 other) const noexcept {
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
	/// Compute the bitwise AND-NOT of packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 andnot_ps(f32x4 other) const noexcept {
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
	/// Compute the bitwise OR of packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 or_ps(f32x4 other) const noexcept {
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
	/// Compute the bitwise XOR of packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 xor_ps(f32x4 other) const noexcept {
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
	/// Compare packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 max_ps(f32x4 other) const noexcept {
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
	/// Compare the lower single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 max_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_max_ss(data, other);
		else
			return { data[0] >= other[0] ? data[0] : other[0], data[1], data[2], data[3] };
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 min_ps(f32x4 other) const noexcept {
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
	/// Compare the lower single-precision (32-bit) floating-point elements in <code>this</code> and <code>other</code> and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 min_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_min_ss(data, other);
		else
			return { data[0] <= other[0] ? data[0] : other[0], data[1], data[2], data[3] };
	}

	/// <summary>
	/// Set each bit of mask <code>this</code> based on the most significant bit of the corresponding packed single-precision (32-bit) floating-point element in <code>this</code>.
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
	/// Move the lower single-precision (32-bit) floating-point element from <code>other</code> and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 move_ss(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_move_ss(data, other);
		else
			return { other[0], data[1], data[2], data[3] };
	}

	/// <summary>
	/// Move the upper 2 single-precision (32-bit) floating-point elements from <code>other</code> to the lower 2 elements and copy the upper 2 elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 movehl_ps(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_movehl_ps(data, other);
		else
			return { other[2], other[3], data[2], data[3] };
	}

	/// <summary>
	/// Move the lower 2 single-precision (32-bit) floating-point elements from <code>other</code> to the upper 2 elements and copy the lower 2 elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr f32x4 movelh_ps(f32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_movelh_ps(data, other);
		else
			return { data[0], data[1], other[0], other[1] };
	}

#pragma endregion

#pragma region Set

	/// <summary>
	/// Set packed single-precision (32-bit) floating-point elements in <code>this</code> with the supplied values.
	/// </summary>
	constexpr void set_ps(const float e0, const float e1, const float e2, const float e3) noexcept {
		if constexpr (has_sse)
			data = _mm_setr_ps(e0, e1, e2, e3);
		else
			data = { e0, e1, e2, e3 };
	}

	/// <summary>
	/// Broadcast single-precision (32-bit) floating-point value to all elements of this.
	/// </summary>
	constexpr void set_ps1(const float value) noexcept {
		if constexpr (has_sse)
			data = _mm_set_ps1(value);
		else
			data = { value, value, value, value };
	}

	/// <summary>
	/// Copy single-precision (32-bit) floating-point element, value, to the lower element of this, and zero the upper 3 elements.
	/// </summary>
	constexpr void set_ss(const float value) noexcept {
		if constexpr (has_sse)
			data = _mm_set_ss(value);
		else
			data = { value, 0, 0, 0 };
	}

	/// <summary>
	/// Set all elements to zero in <code>this</code>.
	/// </summary>
	constexpr void setzero_ps() noexcept {
		if constexpr (has_sse)
			data = _mm_setzero_ps();
		else
			data = { 0, 0, 0, 0 };
	}

#pragma endregion

#pragma region Store

	/// <summary>
	/// Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from <code>this</code> into destination. destination must be aligned on a 16-byte boundary or a general-protection exception may be generated.
	/// </summary>
	constexpr void store_ps(float* destination) const noexcept {
		if constexpr (has_sse)
			_mm_store_ps(destination, data);
		else
			std::memcpy(destination, data, sizeof(data));
	}

	/// <summary>
	/// Store the lower single-precision (32-bit) floating-point element from <code>this</code> into 4 contiguous elements in memory. destination must be aligned on a 16-byte boundary or a general-protection exception may be generated.
	/// </summary>
	constexpr void store_ps1(float* destination) const noexcept {
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
	/// Store the lower single-precision (32-bit) floating-point element from <code>this</code> into destination. destination does not need to be aligned on any particular boundary.
	/// </summary>
	constexpr void store_ss(float* destination) const noexcept {
		if constexpr (has_sse)
			_mm_store_ss(destination, data);
		else
			*destination = data[0];
	}

	/// <summary>
	/// Store 4 single-precision (32-bit) floating-point elements from <code>this</code> into destination in reverse order. destination must be aligned on a 16-byte boundary or a general-protection exception may be generated.
	/// </summary>
	constexpr void storer_ps(float* destination) const noexcept {
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
	/// Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from <code>this</code> into destination. destination does not need to be aligned on any particular boundary.
	/// </summary>
	constexpr void storeu_ps(float* destination) const noexcept {
		if constexpr (has_sse)
			_mm_storeu_ps(destination, data);
		else
			std::memcpy(destination, data, sizeof(data));
	}

	/// <summary>
	/// Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from <code>this</code> into destination using a non-temporal memory hint. destination must be aligned on a 16-byte boundary or a general-protection exception may be generated.
	/// </summary>
	constexpr void stream_ps(float* destination) const noexcept {
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

	constexpr void swap(f32x4& other) noexcept {
		T temporary = other.data;
		other.data = data;
		data = temporary;
	}

#pragma endregion

#pragma region Operators

	[[nodiscard]] constexpr f32x4 operator+(const f32x4 other) const noexcept {
		return add_ps(other);
	}

	[[nodiscard]] constexpr f32x4 operator/(const f32x4 other) const noexcept {
		return div_ps(other);
	}

	[[nodiscard]] constexpr f32x4 operator*(const f32x4 other) const noexcept {
		return mul_ps(other);
	}

	[[nodiscard]] constexpr f32x4 operator-(const f32x4 other) const noexcept {
		return sub_ps(other);
	}

	[[nodiscard]] constexpr operator float*() noexcept {
		return reinterpret_cast_single_pointer<float>(data);
	}

	[[nodiscard]] constexpr operator const float*() const noexcept {
		return reinterpret_cast_single_pointer<const float>(data);
	}

	[[nodiscard]] constexpr operator T() const noexcept {
		return data;
	}

	friend std::ostream& operator<<(std::ostream& ostream, const f32x4 t) {
		ostream << '{' << t[0] << ", " << t[1] << ", " << t[2] << ", " << t[3] << '}';
		return ostream;
	}

#pragma endregion

#pragma endregion

};