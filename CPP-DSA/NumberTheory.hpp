#pragma once

#include "Math.hpp"
#include "swap.hpp"

#include <bit>

namespace Algorithms {

	namespace NumberTheory {

#pragma region Numeric Properties

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static bool is_abundant(T value) noexcept {
			return false; //. TO-DO
		}

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static bool is_coprime(T value) noexcept {
			return false; //. TO-DO
		}

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static bool is_deficient(T value) noexcept {
			return false; //. TO-DO
		}

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static bool is_even(T value) noexcept {
			if constexpr (std::is_floating_point_v<T>)
				return value % 2 == 0;
			else
				return value & 1 == 0;
		}

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static bool is_fibonnaci(T value) noexcept {
			return false; //. TO-DO
		}

		template <std::integral T, bool SAFE = false>
		[[nodiscard]] constexpr static bool is_multiple_of(T value, T base)
			noexcept (SAFE)
		{
			if constexpr (SAFE)
				if (base == 0)
					return false;
			return value % base == 0;
		}

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static bool is_natural(T value) noexcept {
			if constexpr (std::is_floating_point_v<T>)
				return value >= 1 && value % 1 == 0;
			else
				return value >= 1;
		}

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static bool is_odd(T value) noexcept {
			return !is_even(value);
		}

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static bool is_palindromic(T value) noexcept {
			return false; //. TO-DO
		}

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static bool is_perfect(T value) noexcept {
			return false; //. TO-DO
		}

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static bool is_prime(T value) noexcept { //. sieve next?
			if (value <= 1)
				return false;
			if (value <= 3)
				return true;
			if (value % 2 == 0 || value % 3 == 0)
				return false;

			for (long long i = 5; i * i <= value; i += 6)
				if (value % i == 0 || value % (i + 2) == 0)
					return false;

			return true;
		}

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static bool is_rational(T value) noexcept {
			if constexpr (std::is_floating_point_v<T>)
				return value % 1 != 0;
			else
				return false;
		}

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static bool is_signed(T value) noexcept {
			if constexpr (std::is_unsigned_v<T>)
				return false;
			else
				return value < 0;
		}

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static bool is_unsigned(T value) noexcept {
			if constexpr (std::is_signed_v<T>)
				return value >= 0;
			else
				return true;
		}

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static bool is_whole(T value) noexcept {
			if constexpr (std::is_floating_point_v<T>)
				return value % 1 == 0;
			else if constexpr (std::is_signed_v<T>)
				return value >= 0;
			else
				return true;
		}

		template <typename T>
			requires std::is_fundamental_v<T>
		[[nodiscard]] constexpr static auto magnitude_of(T value) noexcept {
			return std::abs(value);
		}

#pragma endregion

		template <std::integral T, auto gcd_callback>
		constexpr static T gcd(T dividend, T divisor) noexcept {
			return gcd_callback(dividend, divisor);
		}

		template <std::integral T, auto gcd_callback>
		constexpr static T gcd(T* arguments, size_t length) noexcept {
			T gcd = arguments[0];
			for (size_t index = 1; index < length; ++index)
				gcd = gcd_callback(gcd, arguments[index]);

			return gcd;
		}

		template <std::integral T>
		constexpr static T gcd_binary(T dividend, T divisor) noexcept {
			if (dividend == 0)
				return divisor;
			if (divisor == 0)
				return dividend;

			int shift = std::countr_zero(dividend | divisor);
			dividend >>= std::countr_zero(dividend);
			do {
				divisor >>= std::countr_zero(divisor);
				swap_if(dividend, divisor, dividend > divisor);
				divisor -= dividend;
			} while (divisor != 0);

			return dividend << shift;
		}

		template <std::integral T>
		constexpr static T gcd_euclidean(T dividend, T divisor) noexcept {
			while (dividend) {
				T t = divisor;
				divisor = dividend % divisor;
				dividend = t;
			}

			return dividend;
		}

		constexpr static double harmonic_sum(size_t n) noexcept {
			const size_t square = n * n;
			const size_t square2 = square * square;
			return Math::natural_logarithm(n) + 1.0 / (n << 1) - 1.0 / (square * 12) + 1.0 / (square2 * 120) - 1.0 / (square * square2 * 252) + Constants::euler_mascheroni_constant;
		}

		template <std::integral T, auto gcd_callback>
		constexpr static T lcm(T dividend, T divisor) noexcept {
			return (dividend / gcd_callback(dividend, divisor)) * divisor;
		}

		template <std::integral T, auto gcd_callback>
		constexpr static T lcm(T* arguments, size_t length) noexcept {
			T lcm = arguments[0];
			for (size_t index = 1; index < length; ++index) {
				T value = arguments[index];
				lcm = (lcm / gcd_callback(lcm, value)) * value;
			}

			return lcm;
		}

		template <std::unsigned_integral T, std::unsigned_integral U>
		constexpr static void load_euler_totient(T* golden_ratio, const U* primes, const uint64_t* sieve, U end, U start = 3) noexcept {
			for (U integer = start | 1; integer <= end; integer += 2) {
				if (!((sieve[integer >> 6] >> (integer & 0x3F)) & 1ULL))
					continue;
				for (U index = integer; index <= end; index += integer << 1)
					golden_ratio[index] -= golden_ratio[index] / integer;
			}
		}

		template<std::unsigned_integral T>
		constexpr static size_t load_primes(T* primes, const uint64_t* sieve, T end, T start = 3) noexcept {
			size_t top = 0;
			for (T integer = start | 1; integer <= end; integer += 2)
				if ((sieve[integer >> 6] >> (integer & 0x3F)) & 1ULL)
					primes[top++] = integer;
			return top;
		}

		constexpr static void load_primes_sieve(uint64_t* sieve, size_t end, size_t start = 3) noexcept {
			sieve[0] |= 4;
			for (size_t integer = start | 1; integer * integer <= end; integer += 2) {
				if (!((sieve[integer >> 6] >> (integer & 0x3F)) & 1ULL))
					continue;
				for (size_t index = integer * integer; index <= end; index += integer << 1)
					sieve[index >> 6] &= ~(1ULL << (index & 0x3F));
			}
		}

	}

}