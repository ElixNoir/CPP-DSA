#pragma once

#include <cstdint>

namespace Constants {

	constexpr static double apery_constant = 1.202056903100000; // The sum of the reciprocals of positive cubes of 1 to infinity.
	constexpr static double catalan_constant = 0.915965594100000; // Alternating sum of reciprocal odd squares.
	constexpr static double e = 2.718281828459045; // The base of the natural logarithm; central to continuous growth, decay, and calculus.
	constexpr static double euler_mascheroni_constant = 0.57721566490153286; // The limiting difference between the harmonic series and the natural logarithm.
	constexpr static double feigenbaum_alpha = 2.502907875095893; // The limiting ratio of the differences between consecutive bifurcation parameter values where period-doubling occurs.
	constexpr static double feigenbaum_delta = 4.669201609102991; // The reduction ratio between the widths of the tines in the phase space at each period-doubling bifurcation.
	constexpr static double golden_ratio = 1.618033988749895;
	constexpr static double pi = 3.14159265359; // The ratio of a circle's circumference to its diameter.
	
	constexpr static double natural_logarithm_of_2 = 0.693147180559945;
	constexpr static double squared_natural_logarithm_of_2 = 0.480453013918201;
	
	constexpr static double pies = Constants::pi * 2;
	constexpr static double square_root_of_pi = 1.772453850905516;

	constexpr static double inverse_square_root_of_2 = 0.7071067811865475;
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