#pragma once

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>

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