#pragma once

#include <iostream>

namespace DSA {

	template <typename T = float>
	struct TransitionMatrix2 {

		struct SteadyState {

			T pi1;
			T pi2;

			friend std::ostream& operator<<(std::ostream& ostream, const SteadyState t) {
				ostream << '{' << t.pi1 << ", " << t.pi2 << '}';
				return ostream;
			}

		};

		T p11; // probability of staying in 1
		T p12; // probability of going to 2
		T p21; // probability of going to 1
		T p22; // probability of staying in 2

		constexpr void advance() noexcept {
			const T p11t = p11;
			p12 *= p11t + p22;
			p11 = 1 - p12;
			p21 *= p11t + p22;
			p22 = 1 - p21;
		}

		constexpr SteadyState find_steady_state() const noexcept {
			T pi1 = p21 / (p12 + p21);
			return { pi1, 1 - pi1 };
		}

		friend std::ostream& operator<<(std::ostream& ostream, const TransitionMatrix2 t) {
			ostream << '{' << t.p11 << ", " << t.p12 << ", " << t.p21 << ", " << t.p22 << '}';
			return ostream;
		}

	};

}