#pragma once

#include <cmath>
#include <concepts>

namespace Algorithms {

	namespace NumberTheory {

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

	}

}