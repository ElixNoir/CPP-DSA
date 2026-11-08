#pragma once

namespace Algorithms {

	namespace Statistics {

		/*constexpr static double e() noexcept {}

		constexpr static double cdf() noexcept {

		}

		constexpr static double pdf() noexcept {

		}*/

		constexpr static double exponential() noexcept {

		}

		constexpr static double gamma() noexcept {

		}

		constexpr static double normal() noexcept {

		}

		constexpr static double uniform() noexcept {

		}

		constexpr static double probability_of_switch_to_a(double probabilityOfA, double probASwitch, double probBSwitch) noexcept {
			return probabilityOfA * (1 - probASwitch) + (1 - probabilityOfA) * probBSwitch;
		}

	}

}