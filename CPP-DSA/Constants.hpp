#pragma once

#include <cstdint>

namespace Constants {

	constexpr static double e = 2.718281828459045;
	constexpr static double natural_logarithm_of_2 = 0.693147180559945;

	constexpr static double phi = 2.118033988749895;
	constexpr static double pi = 3.14159265359;
	constexpr static double pies = Constants::pi * 2;

	constexpr static double inverted_square_root_of_2 = 0.7071067811865475;
	constexpr static double square_root_of_2 = 1.4142135623730951;
	constexpr static double square_root_of_3 = 1.7320508075688772;
	constexpr static double square_root_of_5 = 2.23606797749979;

	const static uint64_t Factorials[] = {
		1, 1, 2, 6, 24, 120, 720, 5040, 40320,
		362880, 3628800, 39916800, 479001600,
		6227020800, 87178291200, 1307674368000,
		20922789888000, 355687428096000, 6402373705728000
	};

}