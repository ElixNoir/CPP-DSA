#pragma once

#include "Constants.hpp"

#include <concepts>

namespace Algorithms {

	namespace Arithmetic {

#pragma region Ranges

		template <std::unsigned_integral T>
		[[nodiscard]] constexpr static T product(T minimum, T maximum) noexcept {
			return Constants::Factorials[maximum] / Constants::Factorials[minimum - 1];
		}

		template <std::unsigned_integral T>
		[[nodiscard]] constexpr static T sum(T minimum, T maximum) noexcept {
			return (maximum - minimum + 1) * (maximum + minimum) >> 1;
		}

#pragma endregion

	}

}