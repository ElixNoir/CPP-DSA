#pragma once

#include <concepts>

namespace Algorithms {

	namespace Bitwise {

		template <std::unsigned_integral T>
		[[nodiscard]] constexpr static bool is_multiple_of_power_of_2(T value, T powerOf2Base) noexcept {
			return (value & (powerOf2Base - 1)) == 0;
		}

		template <std::unsigned_integral T>
		[[nodiscard]] constexpr static bool is_power_of_2(T value) noexcept {
			return is_multiple_of_power_of_2(value, value);
		}

	}

}