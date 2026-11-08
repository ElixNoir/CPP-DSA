#pragma once

#include "Addable.hpp"
#include "Multipliable.hpp"

namespace Algorithms {

	namespace Arithmetic {

		template <typename T>
			requires DSA::concepts::Multipliable<T>
		constexpr static T square(T value) noexcept {
			return value * value;
		}

		//template <typename T>
			//requires DSA::concepts::Addable<T>
		//constexpr static void sum() noexcept {

		//}

	}

}