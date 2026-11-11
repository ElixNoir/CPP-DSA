#pragma once

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>

#pragma region Evaluation

template <auto VALUE>
concept is_const_evaluable = true;

template <typename OPERATION, typename... ARGUMENTS>
concept is_const_invocable = requires(OPERATION&& operation, ARGUMENTS&&... arguments) {
	typename std::integral_constant<bool, (operation(arguments...), true)>;
};

#pragma endregion

#pragma region Integral Types

template <typename T, typename U>
using copy_signedness_t = std::conditional_t<
	std::is_signed_v<T>,
	std::make_signed_t<U>,
	std::make_unsigned_t<U>
>;

template <std::integral T>
using demoted_t = copy_signedness_t<
	T,
	std::conditional_t<
		(sizeof(T) > 4),
		uint32_t,
		std::conditional_t<
			(sizeof(T) > 2),
			uint16_t,
			uint8_t
		>
	>
>;

template <std::integral T>
using promoted_t = copy_signedness_t<
	T,
	std::conditional_t<
		(sizeof(T) < 2),
		uint16_t,
		std::conditional_t<
			(sizeof(T) < 4),
			uint32_t,
			std::conditional_t<
				(sizeof(T) < 8),
				uint64_t,
				T
			>
		>
	>
>;

template <uintmax_t T>
using smallest_uint_t =
std::conditional_t<
	T <= std::numeric_limits<uint8_t>::max(),
	uint8_t,
	std::conditional_t<
		T <= std::numeric_limits<uint16_t>::max(),
		uint16_t,
		std::conditional_t<
			T <= std::numeric_limits<uint32_t>::max(),
			uint32_t,
			uint64_t
		>
	>
>;

template <intmax_t T>
using smallest_int_t =
std::conditional_t<
	(T >= std::numeric_limits<int8_t>::min() && T <= std::numeric_limits<int8_t>::max()),
	int8_t,
	std::conditional_t<
		(T >= std::numeric_limits<int16_t>::min() && T <= std::numeric_limits<int16_t>::max()),
		int16_t,
		std::conditional_t<
			(T >= std::numeric_limits<int32_t>::min() && T <= std::numeric_limits<int32_t>::max()),
			int32_t,
			int64_t
		>
	>
>;

template <std::integral A, std::integral B>
using smallest_sum_t = std::conditional_t<
	std::unsigned_integral<A>&& std::unsigned_integral<B>,
	smallest_uint_t<(uintmax_t{ 1 } << std::max(
		std::numeric_limits<A>::digits,
		std::numeric_limits<B>::digits
	))>,
	smallest_int_t<(intmax_t{ 1 } << std::max(
		std::numeric_limits<A>::digits,
		std::numeric_limits<B>::digits
	))>
>;

#pragma endregion

#pragma region Interpretation

template <typename T>
using cast_pointer_t = std::conditional_t<std::is_pointer_v<T>, T, std::add_pointer_t<T>>;

template <typename TO, typename FROM>
constexpr TO* reinterpret_cast_single_pointer(FROM source) noexcept {
	return reinterpret_cast<cast_pointer_t<TO>>(source);
}

#pragma endregion