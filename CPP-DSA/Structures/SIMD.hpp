#pragma once

#include "../Algorithms/Bitwise.hpp"
#include "../Algorithms/hash.hpp"
#include "../Algorithms/Math.hpp"

#include <array>
#include <iostream>

#ifdef __SSE__
#include <xmmintrin.h>
constexpr bool has_sse = true;
#elif defined(_M_AMD64) || defined(_M_X64)
#include <xmmintrin.h>
constexpr bool has_sse = true;
#elif defined(_M_IX86_FP) && (_M_IX86_FP >= 1)
#include <xmmintrin.h>
constexpr bool has_sse = true;
#else
constexpr bool has_sse = false;
#endif

#ifdef __SSE2__
constexpr bool has_sse2 = true;
#elif defined(_M_AMD64) || defined(_M_X64)
constexpr bool has_sse2 = true;
#elif defined(_M_IX86_FP) && (_M_IX86_FP >= 2)
constexpr bool has_sse2 = true;
#else
constexpr bool has_sse2 = false;
#endif