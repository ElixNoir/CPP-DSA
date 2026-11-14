#pragma once

#include "Math.hpp"
#include "hash.hpp"
#include "Traits.hpp"

#include <array>
#include <iostream>

#ifdef __SSE__
#include <xmmintrin.h>
constexpr bool has_sse = true;
#elif defined(_M_IX86_FP) && (_M_IX86_FP >= 1)
#include <xmmintrin.h>
constexpr bool has_sse = true;
#elif defined(_M_AMD64) || defined(_M_X64)
#include <xmmintrin.h>
constexpr bool has_sse = true;
#else
constexpr bool has_sse = false;
#endif

template<typename T, size_t CAPACITY, bool HAS_SSE>
struct simd_storage;

template <>
struct simd_storage<float, 4, false> {
	using T = std::array<float, 4>;
};

template<>
struct simd_storage<float, 4, true> {
	using T = __m128;
};

template <>
struct simd_storage<double, 2, false> {
	using T = std::array<double, 2>;
};

template<>
struct simd_storage<double, 2, true> {
	using T = __m128d;
};

template <>
struct simd_storage<int32_t, 4, false> {
	using T = std::array<int32_t, 4>;
};

template<>
struct simd_storage<int32_t, 4, true> {
	using T = __m128i;
};

template <typename T, size_t CAPACITY, bool HAS_SSE = has_sse>
	requires std::is_fundamental_v<T>
struct simd {

	using T = simd_storage<T, CAPACITY, has_sse>::T;

	T data;

#pragma region Constructors & Destructors

	[[nodiscard]] constexpr simd() noexcept = default;

	[[nodiscard]] constexpr simd(float value) noexcept : data(_mm_set_ps1(value)) {}

	[[nodiscard]] constexpr simd(float e0, float e1, float e2, float e3) noexcept : data(_mm_setr_ps(e0, e1, e2, e3)) {}

	[[nodiscard]] constexpr simd(T value) noexcept : data(value) {}

#pragma endregion

#pragma region Methods

#pragma region SSE

#pragma region Arithmetic

	/// <summary>
	/// Add packed single-precision (32-bit) floating-point elements in this and other.
	/// </summary>
	[[nodiscard]] constexpr simd add_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_add_ps(data, other);
		else
			return { data[0] + other[0], data[1] + other[1], data[2] + other[2], data[3] + other[3]};
	}

	/// <summary>
	/// Add the lower single-precision (32-bit) floating-point element in this and other and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd add_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_add_ss(data, other);
		else {
			simd result;
			std::memcpy(result, other, sizeof(T));
			result[0] = data[0] + other[0];
			return result;
		}
	}

	/// <summary>
	/// Divide packed single-precision (32-bit) floating-point elements in this by packed elements in other.
	/// </summary>
	[[nodiscard]] constexpr simd div_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_div_ps(data, other);
		else
			return { data[0] / other[0], data[1] / other[1], data[2] / other[2], data[3] / other[3] };
	}

	/// <summary>
	/// Divide the lower single-precision (32-bit) floating-point element in this by the lower single-precision (32-bit) floating-point element in other and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd div_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_div_ss(data, other);
		else {
			simd result;
			std::memcpy(result, other, sizeof(T));
			result[0] = data[0] / other[0];
			return result;
		}
	}

	/// <summary>
	/// Multiply packed single-precision (32-bit) floating-point elements in this and other.
	/// </summary>
	[[nodiscard]] constexpr simd mul_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_mul_ps(data, other);
		else
			return { data[0] * other[0], data[1] * other[1], data[2] * other[2], data[3] * other[3] };
	}

	/// <summary>
	/// Multiply the lower single-precision (32-bit) floating-point element in this and other, and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd mul_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_mul_ss(data, other);
		else {
			simd result;
			std::memcpy(result, other, sizeof(T));
			result[0] = data[0] * other[0];
			return result;
		}
	}

	/// <summary>
	/// Compute the approximate reciprocal of packed single-precision (32-bit) floating-point elements in this. The maximum relative error for this approximation is less than 1.5 * 2^-12.
	/// </summary>
	[[nodiscard]] constexpr simd rcp_ps() const noexcept {
		if constexpr (has_sse)
			return _mm_rcp_ps(data);
		else
			return { 1 / data[0], 1 / data[1], 1 / data[2], 1 / data[3] };
	}

	/// <summary>
	/// Compute the approximate reciprocal of the lower single-precision (32-bit) floating-point element in this and copy the upper 3 packed elements from this. The maximum relative error for this approximation is less than 1.5*2^-12.
	/// </summary>
	[[nodiscard]] constexpr simd rcp_ss() const noexcept {
		if constexpr (has_sse)
			return _mm_rcp_ss(data);
		else {
			simd result;
			std::memcpy(result, other, sizeof(T));
			result[0] = 1 / data[0]; 
			return result;
		}
	}

	/// <summary>
	/// Compute the approximate reciprocal square root of packed single-precision (32-bit) floating-point elements in this. The maximum relative error for this approximation is less than 1.5*2^-12.
	/// </summary>
	[[nodiscard]] constexpr simd rsqrt_ps() const noexcept {
		if constexpr (has_sse)
			return _mm_rsqrt_ps(data);
		else
			return { 1 / Math::square_root(data[0]), 1 / Math::square_root(data[1]), 1 / Math::square_root(data[2]), 1 / Math::square_root(data[3]) };
	}

	/// <summary>
	/// Compute the approximate reciprocal square root of the lower single-precision (32-bit) floating-point element in this and copy the upper 3 packed elements from this. The maximum relative error for this approximation is less than 1.5*2^-12.
	/// </summary>
	[[nodiscard]] constexpr simd rsqrt_ss() const noexcept {
		if constexpr (has_sse)
			return _mm_rsqrt_ss(data);
		else {
			simd result;
			std::memcpy(result, other, sizeof(T));
			result[0] = 1 / Math::square_root(data[0]);
			return result;
		}
	}

	/// Compute the square root of packed single-precision (32-bit) floating-point elements in this.
	/// </summary>
	[[nodiscard]] constexpr simd sqrt_ps() const noexcept {
		if constexpr (has_sse)
			return _mm_sqrt_ps(data);
		else
			return { Math::square_root(data[0]), Math::square_root(data[1]), Math::square_root(data[2]), Math::square_root(data[3]) };
	}

	/// <summary>
	/// Compute the square root of the lower single-precision (32-bit) floating-point element in this and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd sqrt_ss() const noexcept {
		if constexpr (has_sse)
			return _mm_sqrt_ss(data);
		else {
			simd result;
			std::memcpy(result, other, sizeof(T));
			result[0] = Math::square_root(data[0]);
			return result;
		}
	}

	/// <summary>
	/// Subtract packed single-precision (32-bit) floating-point elements in other from packed single-precision (32-bit) floating-point elements in this.
	/// </summary>
	[[nodiscard]] constexpr simd sub_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_sub_ps(data, other);
		else
			return { data[0] - other[0], data[1] - other[1], data[2] - other[2], data[3] - other[3] };
	}

	/// <summary>
	/// Subtract the lower single-precision (32-bit) floating-point element in other from the lower single-precision (32-bit) floating-point element in this, and copy the upper 3 packed elements from this to the upper elements of dst.
	/// </summary>
	[[nodiscard]] constexpr simd sub_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_sub_ss(data, other);
		else {
			simd result;
			std::memcpy(result, other, sizeof(T));
			result[0] = data[0] - other[0];
			return result;
		}
	}

#pragma endregion

#pragma region Comparison

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for equality.
	/// </summary>
	[[nodiscard]] constexpr simd cmpeq_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpeq_ps(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for equality, and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd cmpeq_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpeq_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for greater-than-or-equal.
	/// </summary>
	[[nodiscard]] constexpr simd cmpge_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpge_ps(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for greater-than-or-equal, and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd cmpge_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpge_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for greater-than.
	/// </summary>
	[[nodiscard]] constexpr simd cmpgt_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpgt_ps(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for greater-than, and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd cmpgt_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpgt_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for less-than-or-equal.
	/// </summary>
	[[nodiscard]] constexpr simd cmple_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmple_ps(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for less-than-or-equal, and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd cmple_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmple_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for less-than.
	/// </summary>
	[[nodiscard]] constexpr simd cmplt_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmplt_ps(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for less-than, and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd cmplt_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmplt_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for not-equal.
	/// </summary>
	[[nodiscard]] constexpr simd cmpneq_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpneq_ps(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for not-equal, and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd cmpneq_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpneq_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for not-greater-than-or-equal.
	/// </summary>
	[[nodiscard]] constexpr simd cmpnge_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpnge_ps(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for not-greater-than-or-equal, and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd cmpnge_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpnge_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for not-greater-than.
	/// </summary>
	[[nodiscard]] constexpr simd cmpngt_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpngt_ps(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for not-greater-than, and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd cmpngt_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpngt_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for not-less-than-or-equal.
	/// </summary>
	[[nodiscard]] constexpr simd cmpnle_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpnle_ps(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for not-less-than-or-equal, and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd cmpnle_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpnle_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for not-less-than.
	/// </summary>
	[[nodiscard]] constexpr simd cmpnlt_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpnlt_ps(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for not-less-than, and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd cmpnlt_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpnlt_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other to see if neither is NaN.
	/// </summary>
	[[nodiscard]] constexpr simd cmpord_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpord_ps(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other to see if neither is NaN and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd cmpord_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpord_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other to see if either is NaN.
	/// </summary>
	[[nodiscard]] constexpr simd cmpunord_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpunord_ps(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other to see if either is NaN and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd cmpunord_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_cmpunord_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for equality.
	/// </summary>
	[[nodiscard]] constexpr int comieq_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_comieq_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for greater-than-or-equal.
	/// </summary>
	[[nodiscard]] constexpr int comige_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_comige_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for greater-than.
	/// </summary>
	[[nodiscard]] constexpr int comigt_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_comigt_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for less-than-or-equal.
	/// </summary>
	[[nodiscard]] constexpr int comile_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_comile_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for less-than.
	/// </summary>
	[[nodiscard]] constexpr int comilt_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_comilt_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for not-equal
	/// </summary>
	[[nodiscard]] constexpr int comineq_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_comineq_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for equality. simd instruction will not signal an exception for QNaNs.
	/// </summary>
	[[nodiscard]] constexpr int ucomieq_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_ucomieq_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for greater-than-or-equal. simd instruction will not signal an exception for QNaNs.
	/// </summary>
	[[nodiscard]] constexpr int ucomige_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_ucomige_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for greater-than. simd instruction will not signal an exception for QNaNs.
	/// </summary>
	[[nodiscard]] constexpr int ucomigt_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_ucomigt_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for less-than-or-equal. simd instruction will not signal an exception for QNaNs.
	/// </summary>
	[[nodiscard]] constexpr int ucomile_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_ucomile_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for less-than. simd instruction will not signal an exception for QNaNs.
	/// </summary>
	[[nodiscard]] constexpr int ucomilt_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_ucomilt_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for not-equal. simd instruction will not signal an exception for QNaNs.
	/// </summary>
	[[nodiscard]] constexpr int ucomineq_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_ucomineq_ss(data, other);
		else
			return {};
	}

#pragma endregion

#pragma region Logical

	/// <summary>
	/// Compute the bitwise AND of packed single-precision (32-bit) floating-point elements in this and other.
	/// </summary>
	[[nodiscard]] constexpr simd and_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_and_ps(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compute the bitwise AND-NOT of packed single-precision (32-bit) floating-point elements in this and other.
	/// </summary>
	[[nodiscard]] constexpr simd andnot_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_andnot_ps(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compute the bitwise OR of packed single-precision (32-bit) floating-point elements in this and other.
	/// </summary>
	[[nodiscard]] constexpr simd or_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_or_ps(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compute the bitwise XOR of packed single-precision (32-bit) floating-point elements in this and other.
	/// </summary>
	[[nodiscard]] constexpr simd xor_ps(simd other) const noexcept {
		if constexpr (has_sse)
		return _mm_xor_ps(data, other);
		else
			return {};
	}

#pragma endregion

#pragma region Miscellaneous

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other.
	/// </summary>
	[[nodiscard]] constexpr simd max_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_max_ps(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd max_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_max_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other.
	/// </summary>
	[[nodiscard]] constexpr simd min_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_min_ps(data, other);
		else
			return {};
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd min_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_min_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Set each bit of mask dst based on the most significant bit of the corresponding packed single-precision (32-bit) floating-point element in this.
	/// </summary>
	[[nodiscard]] constexpr int movemask_ps() const noexcept {
		if constexpr (has_sse)
			return _mm_movemask_ps(data);
		else
			return {};
	}

#pragma endregion

#pragma region Move

	/// <summary>
	/// Move the lower single-precision (32-bit) floating-point element from other and copy the upper 3 packed elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd move_ss(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_move_ss(data, other);
		else
			return {};
	}

	/// <summary>
	/// Move the upper 2 single-precision (32-bit) floating-point elements from other to the lower 2 elements and copy the upper 2 elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd movehl_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_movehl_ps(data, other);
		else
			return {};
	}

	/// <summary>
	/// Move the lower 2 single-precision (32-bit) floating-point elements from other to the upper 2 elements and copy the lower 2 elements from this.
	/// </summary>
	[[nodiscard]] constexpr simd movelh_ps(simd other) const noexcept {
		if constexpr (has_sse)
			return _mm_movelh_ps(data, other);
		else
			return {};
	}

#pragma endregion

#pragma region Set

	/// <summary>
	/// Set packed single-precision (32-bit) floating-point elements in this with the supplied values.
	/// </summary>
	constexpr void set_ps(float e0, float e1, float e2, float e3) noexcept {
		if constexpr (has_sse)
			data = _mm_setr_ps(e0, e1, e2, e3);
		else
			return {};
	}

	/// <summary>
	/// Broadcast single-precision (32-bit) floating-point value to all elements of this.
	/// </summary>
	constexpr void set_ps1(float value) noexcept {
		if constexpr (has_sse)
			data = _mm_set_ps1(value);
		else
			return {};
	}

	/// <summary>
	/// Copy single-precision (32-bit) floating-point element, value, to the lower element of this, and zero the upper 3 elements.
	/// </summary>
	constexpr void set_ss(float value) noexcept {
		if constexpr (has_sse)
			data = _mm_set_ss(value);
		else
			return {};
	}

	/// <summary>
	/// Set all elements to zero in this.
	/// </summary>
	constexpr void setzero_ps() noexcept {
		if constexpr (has_sse)
			data = _mm_setzero_ps();
		else
			return {};
	}

#pragma endregion

#pragma region Store

	/// <summary>
	/// Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this into destination. destination must be aligned on a 16-byte boundary or a general-protection exception may be generated.
	/// </summary>
	constexpr void store_ps(float* destination) const noexcept {
		if constexpr (has_sse)
			_mm_store_ps(destination, data);
		else
			return {};
	}

	/// <summary>
	/// Store the lower single-precision (32-bit) floating-point element from this into 4 contiguous elements in memory. destination must be aligned on a 16-byte boundary or a general-protection exception may be generated.
	/// </summary>
	constexpr void store_ps1(float* destination) const noexcept {
		if constexpr (has_sse)
			_mm_store_ps1(destination, data);
		else
			return {};
	}

	/// <summary>
	/// Store the lower single-precision (32-bit) floating-point element from this into destination. destination does not need to be aligned on any particular boundary.
	/// </summary>
	constexpr void store_ss(float* destination) const noexcept {
		if constexpr (has_sse)
			_mm_store_ss(destination, data);
		else
			return {};
	}

	/// <summary>
	/// Store 4 single-precision (32-bit) floating-point elements from this into destination in reverse order. destination must be aligned on a 16-byte boundary or a general-protection exception may be generated.
	/// </summary>
	constexpr void storer_ps(float* destination) const noexcept {
		if constexpr (has_sse)
			_mm_storer_ps(destination, data);
		else
			return {};
	}

	/// <summary>
	/// Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this into destination. destination does not need to be aligned on any particular boundary.
	/// </summary>
	constexpr void storeu_ps(float* destination) const noexcept {
		if constexpr (has_sse)
			_mm_storeu_ps(destination, data);
		else
			return {};
	}

	/// <summary>
	/// Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point elements) from this into destination using a non-temporal memory hint. destination must be aligned on a 16-byte boundary or a general-protection exception may be generated.
	/// </summary>
	constexpr void stream_ps(float* destination) const noexcept {
		if constexpr (has_sse)
			_mm_stream_ps(destination, data);
		else
			return {};
	}

#pragma endregion

#pragma endregion

#pragma region Other

	[[nodiscard]] constexpr uintmax_t hash() const noexcept {
		return Algorithms::hash(data);
	}

	constexpr void swap(simd& other) noexcept {
		T temporary = other.data;
		other.data = data;
		data = temporary;
	}

#pragma endregion

#pragma region Operators

	[[nodiscard]] constexpr simd operator+(simd other) const noexcept {
		return add_ps(other);
	}

	[[nodiscard]] constexpr simd operator/(simd other) const noexcept {
		return div_ps(other);
	}

	[[nodiscard]] constexpr simd operator*(simd other) const noexcept {
		return mul_ps(other);
	}

	[[nodiscard]] constexpr simd operator-(simd other) const noexcept {
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

	friend std::ostream& operator<<(std::ostream& ostream, const simd t) {
		ostream << '{' << t[0] << ", " << t[1] << ", " << t[2] << ", " << t[3] << '}';
		return ostream;
	}

#pragma endregion

#pragma endregion

};