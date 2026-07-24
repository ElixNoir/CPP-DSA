#pragma once

#include <cstdint>
#include <type_traits>

template <uint64_t Maximum>
using smallest_uint_t = std::conditional_t<Maximum <= UINT8_MAX, uint8_t,
    std::conditional_t<Maximum <= UINT16_MAX, uint16_t,
        std::conditional_t<Maximum <= UINT32_MAX, uint32_t, uint64_t>
    >
>;
