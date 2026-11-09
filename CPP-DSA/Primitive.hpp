#pragma once

#include <concepts>
#include <xmmintrin.h>

struct fx4 {

	using T = __m128;

	T data;

#pragma region Constructors & Destructors

	constexpr fx4(T value) noexcept : data(value) {}

#pragma endregion

#pragma region Methods

#pragma region Arithmetic

	/// <summary>
	/// Add packed single-precision (32-bit) floating-point elements in this and other.
	/// </summary>
	constexpr fx4 add_ps(fx4 other) const noexcept {
		return _mm_add_ps(data, other);
	}

	/// <summary>
	/// Add the lower single-precision (32-bit) floating-point element in this and other and copy the upper 3 packed elements from this.
	/// </summary>
	constexpr fx4 add_ss(fx4 other) const noexcept {
		return _mm_add_ss(data, other);
	}

	/// <summary>
	/// Divide packed single-precision (32-bit) floating-point elements in this by packed elements in other.
	/// </summary>
	constexpr fx4 div_ps(fx4 other) const noexcept {
		return _mm_div_ps(data, other);
	}

	/// <summary>
	/// Divide the lower single-precision (32-bit) floating-point element in this by the lower single-precision (32-bit) floating-point element in other and copy the upper 3 packed elements from this.
	/// </summary>
	constexpr fx4 div_ss(fx4 other) const noexcept {
		return _mm_div_ss(data, other);
	}

	/// <summary>
	/// Multiply packed single-precision (32-bit) floating-point elements in this and other.
	/// </summary>
	constexpr fx4 mul_ps(fx4 other) const noexcept {
		return _mm_mul_ps(data, other);
	}

	/// <summary>
	/// Multiply the lower single-precision (32-bit) floating-point element in this and other, and copy the upper 3 packed elements from this.
	/// </summary>
	constexpr fx4 mul_ss(fx4 other) const noexcept {
		return _mm_mul_ss(data, other);
	}

	/// <summary>
	/// Compute the approximate reciprocal of packed single-precision (32-bit) floating-point elements in this. The maximum relative error for this approximation is less than 1.5 * 2^-12.
	/// </summary>
	constexpr fx4 rcp_ps() const noexcept {
		return _mm_rcp_ps(data);
	}

	/// <summary>
	/// Compute the approximate reciprocal of the lower single-precision (32-bit) floating-point element in this and copy the upper 3 packed elements from this. The maximum relative error for this approximation is less than 1.5*2^-12.
	/// </summary>
	constexpr fx4 rcp_ss() const noexcept {
		return _mm_rcp_ss(data);
	}

	/// <summary>
	/// Compute the approximate reciprocal square root of packed single-precision (32-bit) floating-point elements in this. The maximum relative error for this approximation is less than 1.5*2^-12.
	/// </summary>
	constexpr fx4 rsqrt_ps() const noexcept {
		return _mm_rsqrt_ps(data);
	}

	/// <summary>
	/// Compute the approximate reciprocal square root of the lower single-precision (32-bit) floating-point element in this and copy the upper 3 packed elements from this. The maximum relative error for this approximation is less than 1.5*2^-12.
	/// </summary>
	constexpr fx4 rsqrt_ss() const noexcept {
		return _mm_rsqrt_ss(data);
	}

	/// Compute the square root of packed single-precision (32-bit) floating-point elements in this.
	/// </summary>
	constexpr fx4 sqrt_ps() const noexcept {
		return _mm_sqrt_ps(data);
	}

	/// <summary>
	/// Compute the square root of the lower single-precision (32-bit) floating-point element in this and copy the upper 3 packed elements from this.
	/// </summary>
	constexpr fx4 sqrt_ss() const noexcept {
		return _mm_sqrt_ss(data);
	}

	/// <summary>
	/// Subtract packed single-precision (32-bit) floating-point elements in other from packed single-precision (32-bit) floating-point elements in this.
	/// </summary>
	constexpr fx4 sub_ps(fx4 other) const noexcept {
		return _mm_sub_ps(data, other);
	}

	/// <summary>
	/// Subtract the lower single-precision (32-bit) floating-point element in other from the lower single-precision (32-bit) floating-point element in this, and copy the upper 3 packed elements from this to the upper elements of dst.
	/// </summary>
	constexpr fx4 sub_ss(fx4 other) const noexcept {
		return _mm_sub_ss(data, other);
	}

#pragma endregion

#pragma region Comparison

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for equality.
	/// </summary>
	constexpr fx4 cmpeq_ps(fx4 other) const noexcept {
		return _mm_cmpeq_ps(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for equality, and copy the upper 3 packed elements from this.
	/// </summary>
	constexpr fx4 cmpeq_ss(fx4 other) const noexcept {
		return _mm_cmpeq_ss(data, other);
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for greater-than-or-equal.
	/// </summary>
	constexpr fx4 cmpge_ps(fx4 other) const noexcept {
		return _mm_cmpge_ps(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for greater-than-or-equal, and copy the upper 3 packed elements from this.
	/// </summary>
	constexpr fx4 cmpge_ss(fx4 other) const noexcept {
		return _mm_cmpge_ss(data, other);
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for greater-than.
	/// </summary>
	constexpr fx4 cmpgt_ps(fx4 other) const noexcept {
		return _mm_cmpgt_ps(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for greater-than, and copy the upper 3 packed elements from this.
	/// </summary>
	constexpr fx4 cmpgt_ss(fx4 other) const noexcept {
		return _mm_cmpgt_ss(data, other);
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for less-than-or-equal.
	/// </summary>
	constexpr fx4 cmple_ps(fx4 other) const noexcept {
		return _mm_cmple_ps(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for less-than-or-equal, and copy the upper 3 packed elements from this.
	/// </summary>
	constexpr fx4 cmple_ss(fx4 other) const noexcept {
		return _mm_cmple_ss(data, other);
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for less-than.
	/// </summary>
	constexpr fx4 cmplt_ps(fx4 other) const noexcept {
		return _mm_cmplt_ps(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for less-than, and copy the upper 3 packed elements from this.
	/// </summary>
	constexpr fx4 cmplt_ss(fx4 other) const noexcept {
		return _mm_cmplt_ss(data, other);
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for not-equal.
	/// </summary>
	constexpr fx4 cmpneq_ps(fx4 other) const noexcept {
		return _mm_cmpneq_ps(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for not-equal, and copy the upper 3 packed elements from this.
	/// </summary>
	constexpr fx4 cmpneq_ss(fx4 other) const noexcept {
		return _mm_cmpneq_ss(data, other);
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for not-greater-than-or-equal.
	/// </summary>
	constexpr fx4 cmpnge_ps(fx4 other) const noexcept {
		return _mm_cmpnge_ps(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for not-greater-than-or-equal, and copy the upper 3 packed elements from this.
	/// </summary>
	constexpr fx4 cmpnge_ss(fx4 other) const noexcept {
		return _mm_cmpnge_ss(data, other);
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for not-greater-than.
	/// </summary>
	constexpr fx4 cmpngt_ps(fx4 other) const noexcept {
		return _mm_cmpngt_ps(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for not-greater-than, and copy the upper 3 packed elements from this.
	/// </summary>
	constexpr fx4 cmpngt_ss(fx4 other) const noexcept {
		return _mm_cmpngt_ss(data, other);
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for not-less-than-or-equal.
	/// </summary>
	constexpr fx4 cmpnle_ps(fx4 other) const noexcept {
		return _mm_cmpnle_ps(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for not-less-than-or-equal, and copy the upper 3 packed elements from this.
	/// </summary>
	constexpr fx4 cmpnle_ss(fx4 other) const noexcept {
		return _mm_cmpnle_ss(data, other);
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other for not-less-than.
	/// </summary>
	constexpr fx4 cmpnlt_ps(fx4 other) const noexcept {
		return _mm_cmpnlt_ps(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other for not-less-than, and copy the upper 3 packed elements from this.
	/// </summary>
	constexpr fx4 cmpnlt_ss(fx4 other) const noexcept {
		return _mm_cmpnlt_ss(data, other);
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other to see if neither is NaN.
	/// </summary>
	constexpr fx4 cmpord_ps(fx4 other) const noexcept {
		return _mm_cmpord_ps(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other to see if neither is NaN and copy the upper 3 packed elements from this.
	/// </summary>
	constexpr fx4 cmpord_ss(fx4 other) const noexcept {
		return _mm_cmpord_ss(data, other);
	}

	/// <summary>
	/// Compare packed single-precision (32-bit) floating-point elements in this and other to see if either is NaN.
	/// </summary>
	constexpr fx4 cmpunord_ps(fx4 other) const noexcept {
		return _mm_cmpunord_ps(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point elements in this and other to see if either is NaN and copy the upper 3 packed elements from this.
	/// </summary>
	constexpr fx4 cmpunord_ss(fx4 other) const noexcept {
		return _mm_cmpunord_ss(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for equality. fx4 instruction will not signal an exception for QNaNs.
	/// </summary>
	constexpr int ucomieq_ss(fx4 other) const noexcept {
		return _mm_ucomieq_ss(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for greater-than-or-equal. fx4 instruction will not signal an exception for QNaNs.
	/// </summary>
	constexpr int ucomige_ss(fx4 other) const noexcept {
		return _mm_ucomige_ss(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for greater-than. fx4 instruction will not signal an exception for QNaNs.
	/// </summary>
	constexpr int ucomigt_ss(fx4 other) const noexcept {
		return _mm_ucomigt_ss(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for less-than-or-equal. fx4 instruction will not signal an exception for QNaNs.
	/// </summary>
	constexpr int ucomile_ss(fx4 other) const noexcept {
		return _mm_ucomile_ss(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for less-than. fx4 instruction will not signal an exception for QNaNs.
	/// </summary>
	constexpr int ucomilt_ss(fx4 other) const noexcept {
		return _mm_ucomilt_ss(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for not-equal. fx4 instruction will not signal an exception for QNaNs.
	/// </summary>
	constexpr int ucomineq_ss(fx4 other) const noexcept {
		return _mm_ucomineq_ss(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for equality. fx4 instruction will not signal an exception for QNaNs.
	/// </summary>
	constexpr int ucomieq_ss(fx4 other) const noexcept {
		return _mm_ucomieq_ss(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for greater-than-or-equal. fx4 instruction will not signal an exception for QNaNs.
	/// </summary>
	constexpr int ucomige_ss(fx4 other) const noexcept {
		return _mm_ucomige_ss(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for greater-than. fx4 instruction will not signal an exception for QNaNs.
	/// </summary>
	constexpr int ucomigt_ss(fx4 other) const noexcept {
		return _mm_ucomigt_ss(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for less-than-or-equal. fx4 instruction will not signal an exception for QNaNs.
	/// </summary>
	constexpr int ucomile_ss(fx4 other) const noexcept {
		return _mm_ucomile_ss(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for less-than. fx4 instruction will not signal an exception for QNaNs.
	/// </summary>
	constexpr int ucomilt_ss(fx4 other) const noexcept {
		return _mm_ucomilt_ss(data, other);
	}

	/// <summary>
	/// Compare the lower single-precision (32-bit) floating-point element in this and other for not-equal. fx4 instruction will not signal an exception for QNaNs.
	/// </summary>
	constexpr int ucomineq_ss(fx4 other) const noexcept {
		return _mm_ucomineq_ss(data, other);
	}

#pragma endregion

#pragma region Logical

	/// <summary>
	/// Compute the bitwise AND of packed single-precision (32-bit) floating-point elements in this and other.
	/// </summary>
	constexpr fx4 and_ps(fx4 other) const noexcept {
		return _mm_and_ps(data, other);
	}

	/// <summary>
	/// Compute the bitwise AND-NOT of packed single-precision (32-bit) floating-point elements in this and other.
	/// </summary>
	constexpr fx4 andnot_ps(fx4 other) const noexcept {
		return _mm_andnot_ps(data, other);
	}

	/// <summary>
	/// Compute the bitwise OR of packed single-precision (32-bit) floating-point elements in this and other.
	/// </summary>
	constexpr fx4 or_ps(fx4 other) const noexcept {
		return _mm_or_ps(data, other);
	}

	/// <summary>
	/// Compute the bitwise XOR of packed single-precision (32-bit) floating-point elements in this and other.
	/// </summary>
	constexpr fx4 xor_ps(fx4 other) const noexcept {
		return _mm_xor_ps(data, other);
	}

#pragma endregion

#pragma region Miscellaneous

	/// <summary>
	/// Set each bit of mask dst based on the most significant bit of the corresponding packed single-precision (32-bit) floating-point element in this.
	/// </summary>
	constexpr int movemask_ps() const noexcept {
		return _mm_movemask_ps(data);
	}

#pragma endregion

#pragma region Move

	/// <summary>
	/// Move the lower single-precision (32-bit) floating-point element from other and copy the upper 3 packed elements from this.
	/// </summary>
	constexpr fx4 move_ss(fx4 other) const noexcept {
		return _mm_move_ss(data, other);
	}

	/// <summary>
	/// Move the upper 2 single-precision (32-bit) floating-point elements from other to the lower 2 elements and copy the upper 2 elements from this.
	/// </summary>
	constexpr fx4 movehl_ps(fx4 other) const noexcept {
		return _mm_movehl_ps(data, other);
	}

	/// <summary>
	/// Move the lower 2 single-precision (32-bit) floating-point elements from other to the upper 2 elements and copy the lower 2 elements from this.
	/// </summary>
	constexpr fx4 movelh_ps(fx4 other) const noexcept {
		return _mm_movelh_ps(data, other);
	}

#pragma endregion

#pragma region Set

	constexpr void swap(fx4& other) noexcept {
		T temporary = other.data;
		other.data = data;
		data = temporary;
	}

#pragma endregion

#pragma region Operators

	constexpr fx4 operator+(fx4 other) const noexcept {
		return add_ps(other);
	}

	constexpr fx4 operator/(fx4 other) const noexcept {
		return div_ps(other);
	}

	constexpr fx4 operator*(fx4 other) const noexcept {
		return mul_ps(other);
	}

	constexpr fx4 operator-(fx4 other) const noexcept {
		return sub_ps(other);
	}

	constexpr operator T() const noexcept {
		return data;
	}

	constexpr T hash() const noexcept {
		return data;
	}

#pragma endregion

#pragma endregion

};