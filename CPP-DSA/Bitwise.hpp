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

	template<std::unsigned_integral T>
	[[nodiscard]] constexpr static bool is_multiple_of_power_of_2(T value, T powerOf2Base) noexcept {
		return (value & (powerOf2Base - 1)) == 0;
	}

	template<std::unsigned_integral T>
	[[nodiscard]] constexpr static bool is_power_of_2(T value) noexcept {
		return is_multiple_of_power_of_2(value, value);
	}

}
