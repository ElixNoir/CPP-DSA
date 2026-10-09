#pragma once

#include <cstdint>

constexpr inline static uintmax_t FNV1aOffset = (sizeof(uintmax_t) == sizeof(uint32_t))
? static_cast<uintmax_t>(0x811C9DC5u)
    : static_cast<uintmax_t>(0xCBF29CE484222325ULL);

constexpr inline static uintmax_t FNV1aPrime = (sizeof(uintmax_t) == sizeof(uint32_t))
? static_cast<uintmax_t>(0x01000193u)
    : static_cast<uintmax_t>(0x100000001b3ULL);