#pragma once

#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>

template <std::integral Demotee>
using demotion = decltype(-std::declval<Demotee>());

template <std::integral Promotee>
using promotion = decltype(+std::declval<Promotee>());

template <uint64_t Target>
using smallest_uint_t = 
    std::conditional_t<
        Maximum <= std::numeric_limits<uint8_t>::max(),
        uint8_t,
    std::conditional_t<
        Maximum <= std::numeric_limits<uint16_t>::max(),
        uint16_t,
    std::conditional_t<
        Maximum <= std::numeric_limits<uint32_t>::max(),
        uint32_t,
        uint64_t
    >>>;

template <int64_t Target>
using smallest_int_t =
    std::conditional_t<
        (Maximum >= std::numeric_limits<int8_t>::min() &&
         Maximum <= std::numeric_limits<int8_t>::max()),
        int8_t,
    std::conditional_t<
        (Maximum >= std::numeric_limits<int16_t>::min() &&
         Maximum <= std::numeric_limits<int16_t>::max()),
        int16_t,
    std::conditional_t<
        (Maximum >= std::numeric_limits<int32_t>::min() &&
         Maximum <= std::numeric_limits<int32_t>::max()),
        int32_t,
        int64_t
    >>>;
