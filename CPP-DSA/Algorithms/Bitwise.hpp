#pragma once

#include <concepts>

namespace DSA {

	union byte {

		int8_t i;
		uint8_t u;

		constexpr static byte set() noexcept {
			return { .u = 0xFF };
		}

	};

	union word {

		int16_t i;
		uint16_t u;

		constexpr static word set() noexcept {
			return { .u = 0xFFFF };
		}

	};

	union dword {

		float f;
		int32_t i;
		uint32_t u;

		constexpr static dword set() noexcept {
			return { .u = 0xFFFFFFFF };
		}

	};

	union qword {

		double f;
		int64_t i;
		uint64_t u;

		constexpr static qword set() noexcept {
			return { .u = 0xFFFFFFFFFFFFFFFF };
		}

	};

}

namespace Algorithms::Bitwise {

#pragma region SWAR

	[[nodiscard]] constexpr static uint64_t add_epi8(const uint64_t lhs, const uint64_t rhs) noexcept {
		constexpr static uint64_t LOW  = 0x7F7F7F7F7F7F7F7F;
		constexpr static uint64_t HIGH = 0x8080808080808080;
		const uint64_t low = (lhs & LOW) + (rhs & LOW);
		return (low & LOW) | (((lhs ^ rhs) & HIGH) ^ (low & HIGH));
	}

	[[nodiscard]] constexpr static uint64_t add_epi16(const uint64_t lhs, const uint64_t rhs) noexcept {
		constexpr static uint64_t LOW  = 0x7FFF7FFF7FFF7FFF;
		constexpr static uint64_t HIGH = 0x8000800080008000;
		const uint64_t low = (lhs & LOW) + (rhs & LOW);
		return (low & LOW) | (((lhs ^ rhs) & HIGH) ^ (low & HIGH));
	}

	template <bool IGNORE_CARRY = false>
	[[nodiscard]] constexpr static uint64_t adds_epi8(const uint64_t lhs, const uint64_t rhs) noexcept {
		constexpr static uint64_t SIGN = 0x8080808080808080;
		constexpr static uint64_t LOW  = 0x7F7F7F7F7F7F7F7F;
		if constexpr (IGNORE_CARRY) {
			const uint64_t sum = lhs + rhs;
			const uint64_t overflow = ~(lhs ^ rhs) & (lhs ^ sum) & SIGN;
			const uint64_t saturation = (lhs & SIGN) | LOW;
			return (sum & ~overflow) | (saturation & overflow);
		}
		const uint64_t low = (lhs & LOW) + (rhs & LOW);
		const uint64_t sum = (low & LOW) | (((lhs ^ rhs) & SIGN) ^ (low & SIGN));
		const uint64_t overflow = ~(lhs ^ rhs) & (lhs ^ sum) & SIGN;
		const uint64_t saturation = (lhs & SIGN) | LOW;
		return (sum & ~overflow) | (saturation & overflow);
	}

	template <bool IGNORE_CARRY = false>
	[[nodiscard]] constexpr static uint64_t adds_epi16(const uint64_t lhs, const uint64_t rhs) noexcept {
		constexpr static uint64_t SIGN = 0x8000800080008000;
		constexpr static uint64_t LOW  = 0x7FFF7FFF7FFF7FFF;
		if constexpr (IGNORE_CARRY) {
			const uint64_t sum = lhs + rhs;
			const uint64_t overflow = ~(lhs ^ rhs) & (lhs ^ sum) & SIGN;
			const uint64_t saturation = (lhs & SIGN) | LOW;
			return (sum & ~overflow) | (saturation & overflow);
		}
		const uint64_t low = (lhs & LOW) + (rhs & LOW);
		const uint64_t sum = (low & LOW) | (((lhs ^ rhs) & SIGN) ^ (low & SIGN));
		const uint64_t overflow = ~(lhs ^ rhs) & (lhs ^ sum) & SIGN;
		const uint64_t saturation = (lhs & SIGN) | LOW;
		return (sum & ~overflow) | (saturation & overflow);
	}

#pragma endregion

	template <std::unsigned_integral T>
	[[nodiscard]] constexpr static bool is_multiple_of_power_of_2(T value, T powerOf2Base) noexcept {
		return (value & (powerOf2Base - 1)) == 0;
	}

	template <std::unsigned_integral T>
	[[nodiscard]] constexpr static bool is_power_of_2(T value) noexcept {
		return is_multiple_of_power_of_2(value, value);
	}

}
