#pragma once

/*

	[[nodiscard]] constexpr i32x4(const int8_t value) noexcept : data(_mm_set1_epi8(value)) {}
	[[nodiscard]] constexpr i32x4(const int16_t value) noexcept : data(_mm_set1_epi16(value)) {}
	[[nodiscard]] constexpr i32x4(const int64_t value) noexcept : data(_mm_set1_epi64(value)) {}

	[[nodiscard]] constexpr i32x4(const int8_t e0, const int8_t e1, const int8_t e2, const int8_t e3,
		const int8_t e4, const int8_t e5, const int8_t e6, const int8_t e7,
		const int8_t e8, const int8_t e9, const int8_t e10, const int8_t e11,
		const int8_t e12, const int8_t e13, const int8_t e14, const int8_t e15)
		noexcept : data(_mm_setr_epi8(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, e15)) {}
	[[nodiscard]] constexpr i32x4(const int16_t e0, const int16_t e1, const int16_t e2, const int16_t e3,
		const int16_t e4, const int16_t e5, const int16_t e6, const int16_t e7)
		noexcept : data(_mm_setr_epi16(e0, e1, e2, e3, e4, e5, e6, e7)) {}
	[[nodiscard]] constexpr i32x4(const int64_t e0, const int64_t e1)
		noexcept : data(_mm_setr_epi64(e0, e1)) {}

#pragma region Addition

	/// <summary>
	/// Add packed 8-bit integers in <code>this</code> and <code>other</code>.
	/// </summary>
	template <bool IGNORE_CARRY = false>
	[[nodiscard]] constexpr i32x4 add_epi8(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_add_epi8(data, other);
		else {
			const auto x = std::bit_cast<std::array<uint64_t, 2>>(data);
			const auto y = std::bit_cast<std::array<uint64_t, 2>>(other);
			if constexpr (IGNORE_CARRY) {
				x[0] += y[0];
				x[1] += y[1];
			}
			else {
				x[0] = Algorithms::Bitwise::add_epi8(x[0], y[0]);
				x[1] = Algorithms::Bitwise::add_epi8(x[1], y[1]);
			}
			return std::bit_cast<T>(x);
		}
	}

	/// <summary>
	/// Add packed 16-bit integers in <code>this</code> and <code>other</code>.
	/// </summary>
	template <bool IGNORE_CARRY = false>
	[[nodiscard]] constexpr i32x4 add_epi16(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_add_epi16(data, other);
		else {
			const auto x = std::bit_cast<std::array<uint64_t, 2>>(data);
			const auto y = std::bit_cast<std::array<uint64_t, 2>>(other);
			if constexpr (IGNORE_CARRY) {
				x[0] += y[0];
				x[1] += y[1];
			}
			else {
				x[0] = Algorithms::Bitwise::add_epi16(x[0], y[0]);
				x[1] = Algorithms::Bitwise::add_epi16(x[1], y[1]);
			}
			return std::bit_cast<T>(x);
		}
	}

	/// <summary>
	/// Add packed 64-bit integers in <code>this</code> and <code>other</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 add_epi64(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_add_epi64(data, other);
		else {
			const auto x = std::bit_cast<std::array<uint64_t, 2>>(data);
			const auto y = std::bit_cast<std::array<uint64_t, 2>>(other);
			x[0] += y[0];
			x[1] += y[1];
			return std::bit_cast<T>(x);
		}
	}

	/// <summary>
	/// Add the lower single-precision (32-bit) floating-point element in <code>this</code> and <code>other</code> and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 adds_epi8(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_adds_epi8(data, other);
		else {
			const auto x = std::bit_cast<std::array<uint64_t, 2>>(data);
			const auto y = std::bit_cast<std::array<uint64_t, 2>>(other);
			x[0] = Algorithms::Bitwise::adds_epi8(x[0], y[0]);
			x[1] = Algorithms::Bitwise::adds_epi8(x[1], y[1]);
			return std::bit_cast<T>(x);
		}
	}

	/// <summary>
	/// Add the lower single-precision (32-bit) floating-point element in <code>this</code> and <code>other</code> and copy the upper 3 packed elements from <code>this</code>.
	/// </summary>
	[[nodiscard]] constexpr i32x4 adds_epi16(i32x4 other) const noexcept {
		if constexpr (has_sse)
			return _mm_adds_epi16(data, other);
		else {
			const auto x = std::bit_cast<std::array<uint64_t, 2>>(data);
			const auto y = std::bit_cast<std::array<uint64_t, 2>>(other);
			x[0] = Algorithms::Bitwise::adds_epi16(x[0], y[0]);
			x[1] = Algorithms::Bitwise::adds_epi16(x[1], y[1]);
			return std::bit_cast<T>(x);
		}
	}

#pragma endregion

*/